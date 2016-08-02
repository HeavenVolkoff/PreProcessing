#include <cmath>
#include <vector>
#include <iostream>

#include <fftw3.h>
#include <sndfile.hh>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>

using namespace std;

// Logger (will be initialized on main)
std::shared_ptr<spdlog::logger> console;

/**
 * Log received vector
 * @param v
 */
template<class T>
void printVector(const vector<T> &v, size_t limit = 100) {
    if (console->level() > spdlog::level::level_enum::debug) {
        return;
    }

    string arrStr = "[";

    for (int i = 0; i < v.size() && i <= limit; i++) {
        arrStr += to_string(v[i]) + (i == v.size() - 1 ? "" : i == limit ? "..." : ", ");
    }

    arrStr += "]";

    console->debug(arrStr);
}

/**
 * Apply the Hanning windowing function on received amplitude value
 *
 * @param index
 * @param length
 * @return
 */
inline
double hann(size_t index, size_t length) {
    double x = M_PI * index / (double) (length - 1);
    x = sin(x);
    return x * x;
}

/**
 * Apply Fast Fourrier Transform on the received amplitude vector and return the respective ernergy vector
 *
 * @param audioData
 * @return
 */
vector<double> fft(vector<double> &audioData) {
    int counter;
    double real, imag;

    vector<double> output(audioData.size());
    fftw_complex *fftResult = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * audioData.size());
    fftw_plan plan = fftw_plan_dft_r2c_1d((int) audioData.size(), audioData.data(), fftResult, FFTW_ESTIMATE);
    fftw_execute(plan);

    for (counter = 0; counter < audioData.size(); counter++) {
        real = fftResult[counter][0];
        imag = fftResult[counter][1];
        output[counter] = sqrt((real * real) + (imag * imag));
    }

    fftw_destroy_plan(plan);
    free(fftResult);

    return output;
}

/**
 * Get amplitude vector from WAV file
 *
 * @param filePath
 * @return
 */
vector<double> getAudioDataFromWavFile(const std::string &filePath) {
    vector<double> output;
    sf_count_t readFrames;
    SndfileHandle audioFile;
    sf_count_t audioDataSize;

    // Open audio file in read-only
    audioFile = SndfileHandle(filePath);

    if (audioFile.error()) {
        throw string("Failed to open audio file. Reason: ") + audioFile.strError();
    }

    console->debug("{0} frames, {1} sample rate, {2} channels.", audioFile.frames(), audioFile.samplerate(),
                   audioFile.channels());

    audioDataSize = audioFile.frames() * audioFile.channels();
    output = vector<double>((size_t) audioDataSize);
    readFrames = audioFile.read(output.data(), audioDataSize);

    if (readFrames != audioDataSize) {
        throw "Number of read frames don't match all frames in audio file.";
    }

    // Mix down multi-channel audio to mono
    if (audioFile.channels() > 1) {
        vector<double> mono((size_t) audioFile.frames());

        for (int i = 0; i < audioFile.frames(); i++) {
            mono[i] = 0;

            for (int j = 0; j < audioFile.channels(); j++) {
                mono[i] += output[i * audioFile.channels() + j];
            }

            mono[i] /= audioFile.channels();
        }

        return mono;
    }

    return output;
}

int main(int argc, char **argv) {
    // === Logger Init ===
    size_t q_size = 8192;
    spdlog::set_async_mode(q_size);
    console = spdlog::stdout_logger_mt("console", true);
    console->set_level(spdlog::level::debug);
    spdlog::drop_all();

    if (argc < 2) {
        console->warn("Usage {0}: <WAV file>", argv[0]);
        return -1;
    }

    vector<double> audioData = getAudioDataFromWavFile(argv[1]);

    printVector(audioData);

    //Apply windowing function to vector
    for (size_t i = 0; i < audioData.size(); ++i) {
        audioData[i] *= hann(i, audioData.size());
    }

    printVector(audioData);

    vector<double> transformedData = fft(audioData);

    printVector(transformedData);

    return 0;
}