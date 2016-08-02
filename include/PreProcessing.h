#ifndef PREPROCESSING_PREPROCESSING_H
#define PREPROCESSING_PREPROCESSING_H

#include <iostream>

using namespace std;


/**
 * @link: http://support.ircam.fr/docs/AudioSculpt/3.0/co/FFT%20Size.html
 * @link: http://www.fftw.org/doc/Real_002dto_002dReal-Transforms.html#Real_002dto_002dReal-Transforms
 *
 * Propriedades:
 * frameLength
 * frameStep
 * OversamplingFactor
 * fftSize = primeira potencia de (2 * OversamplingFactor) igual ou maior ao frameLength
 * binSize = fftSize/2
 */

template <size_t frameLength, size_t frameStep>
class PreProcessing {
public:
    PreProcessing();
    ~PreProcessing();

    /**
     * TODO # This is the PreProcessing class, it should be created only once.
     * TODO # It should have methods for loading audio both from WAV files (libsnd) and input devices (portaudio).
     * TODO # It will apply to the audio data framming¹ (cut and stride) as well as windowing (border easing).
     * TODO # It will apply FFT (fftw) and MFCC to each audio data fragment
     */
};


#endif //PREPROCESSING_PREPROCESSING_H
