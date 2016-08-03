#ifndef PREPROCESSING_PREPROCESSING_H
#define PREPROCESSING_PREPROCESSING_H

#include <cmath>
#include <vector>
#include <complex>
#include <iostream>

#include <fftw3.h>
#include <sndfile.hh>
#include <spdlog/logger.h>
#include <spdlog/spdlog.h>

using namespace std;

namespace voiceMagic {
    /**
     * @link: http://support.ircam.fr/docs/AudioSculpt/3.0/co/FFT%20Params.html
     * @link: http://www.fftw.org/doc/Real_002dto_002dReal-Transforms.html#Real_002dto_002dReal-Transforms
     *
     * About expectedResoutionDuration: (diferences between Temporal definition and frequency definition)
     * The longer the window, the less "images" we get of the signal evolution in time.
     * The more bins (the sorter the window), the more slices of frequency range we get, and the more precise these slices are.
     *
     * About oversamplingFactor:
     * The shorter the window step the better the detection of transients of temporal variations of the signal. (more processing thougth)
     *
     * Parameters:
     * expectedResolutionDuration (Default 20ms, this will not obligatorily be the resolutionDuration of the window, it also depends on sampleRate and approximation)
     * oversamplingFactor (>= 2, default to 8)
     * (audio file Path => string) OR (audio device => unknown type)
     *
     * Properties:
     * sampleRate => from audio file (libsnd)
     * binSize => getBinSize(sampleRate)
     * windowSize = fftSize => binSize * 2 (in samples)
     * windowingFunction => implement hammin / hann / blackman
     */
    class PreProcessing {
    public:
        typedef void (*handler_t)(const vector<double> &);

        PreProcessing(const handler_t &handler);
        ~PreProcessing();

        void loadAudioFile(const string &);

        const double cutoffLow = 300.0;
        const double cutoffHigh = 8000.0;
        const size_t filterCount = 26;
        const size_t oversamplingFactor = 8;
        const size_t expectedResolutionDuration = 20;

        const handler_t &handler;
    private:
        std::shared_ptr<spdlog::logger> console;
    };
}

#endif //PREPROCESSING_PREPROCESSING_H
