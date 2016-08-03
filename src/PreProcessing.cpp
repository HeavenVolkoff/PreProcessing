#include <cstdint>
#include <cstddef>
#include "../include/PreProcessing.h"

using namespace std;
using namespace motrix;

inline
size_t getBinSize(size_t sampleRate, size_t resolutionDuration, size_t binMinimumSize = 512) {
    int32_t ws = sampleRate * resolutionDuration / 1000;

    // Find next highest power of two of a 32 bit integer
    ws--;
    ws |= ws >> 1;
    ws |= ws >> 2;
    ws |= ws >> 4;
    ws |= ws >> 8;
    ws |= ws >> 16;
    ws++;

    return ws >= binMinimumSize ? ws : binMinimumSize;
}

vector<double> mixDownAudio(const vector<double> &audioData, int channelsNum, size_t frames) {
    vector<double> mono(frames);

    for (int i = 0; i < frames; i++) {
        mono[i] = 0;

        for (int j = 0; j < channelsNum; j++) {
            mono[i] += audioData[i * channelsNum + j];
        }

        mono[i] /= channelsNum;
    }

    return mono;
}

vector<double> getAudioData(SndfileHandle &audioInfo) {
    vector<double> output;
    sf_count_t readFrames;
    sf_count_t audioDataSize;

    audioDataSize = audioInfo.frames() * audioInfo.channels();
    output = vector<double>((size_t) audioDataSize);
    readFrames = audioInfo.read(output.data(), audioDataSize);

    if (readFrames != audioDataSize) {
        return {};
    }

    // Mix down multi-channel audio to mono
    if (audioInfo.channels() > 1) {
        return mixDownAudio(output, audioInfo.channels(), audioInfo.frames());
    }

    return output;
}

size_t padAudioData(vector<double> *audioData, size_t frameSamples, size_t strideSamples) {
    if (audioData->size() < frameSamples) {
        throw new logic_error("Audio file is smaller than frame.");
    }

    size_t restSamples = audioData->size() - frameSamples;
    size_t oddSamples = restSamples % strideSamples;

    if (oddSamples != 0) {
        audioData->resize(audioData->size() + strideSamples - oddSamples);
    }

    restSamples = audioData->size() - frameSamples;
    return 1 + (restSamples / strideSamples);
}

inline
double hann(size_t index, size_t length) {
    double x = M_PI * index / (double) (length - 1);
    x = sin(x);
    return x * x;
}

double freqToMels(double freq) {
    return 1125 * log(1 + freq / 700);
}

double melsToFreq(double mels) {
    return 700 * (exp(mels / 1125) - 1);
}

void genFilterBanks(const vector<double> &frame, unsigned sampleRate, size_t filterCount, double cutoffLow,
                    double cutoffHigh, vector<double> *result) {
    double *pointsMels;
    double *pointsFreq;
    size_t *pointsSample;
    size_t pointsCount;
    double cutoffLowMels = freqToMels(cutoffLow);
    double cutoffHighMels = freqToMels(cutoffHigh);
    double filterWidthMels = (cutoffHighMels - cutoffLowMels) / (filterCount + 1);

    // TODO: Handle `cuttofHigh` over max frequency

    pointsCount = filterCount + 2; // ?

    pointsMels = new double[pointsCount];
    pointsMels[0] = cutoffLowMels;
    for (int i = 1; i < pointsCount; ++i)
        pointsMels[i] = pointsMels[i - 1] + filterWidthMels;

    pointsFreq = new double[pointsCount];
    for (int i = 0; i < pointsCount; ++i)
        pointsFreq[i] = melsToFreq(pointsMels[i]);
    delete[] pointsMels;

    pointsSample = new size_t[pointsCount];
    for (int i = 0; i < pointsCount; ++i)
        pointsSample[i] = pointsFreq[i] * frame.size() / sampleRate;
    delete[] pointsFreq;

    for (int i = 0; i < filterCount; ++i) {
        size_t begin = pointsSample[i + 0];
        size_t middle = pointsSample[i + 1];
        size_t end = pointsSample[i + 2];
        (*result)[i] = 0.0;
        for (int j = begin; j < middle; ++j) {
            double factor = (j - begin) / (double) (middle - begin);
            (*result)[i] += frame[j] * factor;
        }
        for (int j = middle; j < end; ++j) {
            double factor = (end - j) / (double) (end - middle);
            (*result)[i] += frame[j] * factor;
        }
        (*result)[i] = log((*result)[i]);
    }
    delete[] pointsSample;
}

PreProcessing::PreProcessing(const handler_t &handler) : handler{handler} {
    // === Logger Init ===
    spdlog::set_async_mode(8192);
    console = spdlog::stdout_logger_mt(typeid(*this).name(), true);
    console->set_level(spdlog::level::debug);
    spdlog::drop_all();
}

void PreProcessing::loadAudioFile(const string &audioPath) {
    int counter;
    SndfileHandle audioInfo = SndfileHandle(audioPath);
    fftw_complex *fftResult;
    fftw_plan fftPlan, dctPlan;
    vector<double> audioData, frame, filterBanks;
    size_t binSize, windowSize, windowNum, windowStep;

    if (audioInfo.error()) {
        console->error("Failed to open audio file. Reason: {0}", audioInfo.strError());
        throw new logic_error(string("LibSndFile: ") + audioInfo.strError());
    }

    console->debug("{0} frames, {1} sample rate, {2} channels.", audioInfo.frames(), audioInfo.samplerate(),
                   audioInfo.channels());

    audioData = getAudioData(audioInfo);

    if (audioData.size() == 0) {
        console->error("Failed to read audio file. Malformed file.");
        throw new logic_error("Failed to read audio file. Malformed file.");
    }

    binSize = getBinSize(audioInfo.samplerate(), expectedResolutionDuration/*, binMinimumSize*/);
    windowSize = binSize * 2;
    windowStep = windowSize / oversamplingFactor;
    windowNum = padAudioData(&audioData, windowSize, windowStep);

    frame = vector<double>(windowSize);
    filterBanks = vector<double>(filterCount);
    fftResult = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * windowSize);
    fftPlan = fftw_plan_dft_r2c_1d(windowSize, frame.data(), fftResult, FFTW_MEASURE);
    dctPlan = fftw_plan_r2r_1d(filterCount, filterBanks.data(), filterBanks.data(), FFTW_REDFT10, FFTW_MEASURE);

    for (counter = 0; counter < windowNum; counter++) {
        size_t j;
        size_t frameInit = windowStep * counter;
        double real, imag;

        // Copy frame data and apply window function
        for (j = 0; j < windowSize; j++) {
            frame[j] = audioData[frameInit + j] * hann(j, windowSize);
        }

        fftw_execute(fftPlan); // Execute FFT

        for (j = 0; j < windowSize; j++) { // Copy FFT results to frame
            real = fftResult[j][0];
            imag = fftResult[j][1];
            frame[j] = (real * real) + (imag * imag);
        }

        // Execute MFCC
        genFilterBanks(frame, audioInfo.samplerate(), filterCount, cutoffLow, cutoffHigh, &filterBanks);

        fftw_execute(dctPlan); // Execute DCT

        handler(filterBanks);
    }

    fftw_free(fftResult);
    fftw_destroy_plan(fftPlan);
    fftw_destroy_plan(dctPlan);
}