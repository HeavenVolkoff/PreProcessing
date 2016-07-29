#include <cmath>
#include <vector>
#include <complex>
#include <iostream>

#include <fftw3.h>
#include <sndfile.hh>

using namespace std;

inline
double hann(double value) {
    double x = cos(value * M_PI / 2.0);
    return x*x;
}

vector<double> windowing (vector<double> &audioData) {
    size_t counter = 0;

    for (const double &data : audioData) {
        audioData[counter++] = hann(data);
    }

    return audioData;
}

vector<complex> fft (const vector<double> &data) {
    // TODO: http://www.jimmo.org/example-of-one-dimensional-dft-of-real-data-with-fftw/
}

vector<double> getAudioDataFromWavFile (const std::string &filePath) {
    double* audioData;
    size_t audioDataSize;
    vector<double> output;
    sf_count_t readFrames;
    SndfileHandle audioFile;

    // Open audio file in read-only
    audioFile = SndfileHandle(filePath);

    if (audioFile.error()) {
        throw string("Failed to open audio file. Reason: ") + audioFile.strError();
    }

    cout << audioFile.frames() << " frames, " << audioFile.samplerate() << " samplerate, " << audioFile.channels() << " channels, " << endl;

    audioDataSize = (size_t) audioFile.frames() * audioFile.channels();
    audioData = (double*) malloc(audioDataSize * sizeof(double));
    readFrames = audioFile.read(audioData, audioFile.frames());

    if (readFrames != audioFile.frames()) {
        throw "Number of read frames don't match all frames in audio file.";
    }

    output = vector<double>(audioDataSize);
    output.assign(audioData, audioData + audioDataSize);

    free(audioData);
    return output;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        cerr << "Usage " << argv[0] << ": <WAV file>" << endl;
        return -1;
    }

    vector<double> audioData = getAudioDataFromWavFile(argv[1]);
    audioData = windowing(audioData);

    vector<complex> transformedData = fft(audioData);

    return 0;
}