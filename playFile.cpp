#include <iostream>

#include <portaudio.h>
#include <sndfile.h>

#define error_check(err) \
    do {\
        if (err) { \
            fprintf(stderr, "line %d ", __LINE__); \
            fprintf(stderr, "error number: %d\n", err); \
            fprintf(stderr, "\n\t%s\n\n", Pa_GetErrorText(err)); \
            return err; \
        } \
    } while (0)

static int output_cb(const void * input, void * output, unsigned long frames_per_buffer, const PaStreamCallbackTimeInfo *time_info, PaStreamCallbackFlags flags, void * data) {
    SNDFILE *file = (SNDFILE *) data;

    /* this should not actually be done inside of the stream callback
     * but in an own working thread
     *
     * Note although I haven't tested it for stereo I think you have
     * to multiply frames_per_buffer with the channel count i.e. 2 for
     * stereo */
    sf_read_short(file, (short *) output, (sf_count_t) frames_per_buffer * 2);
    return paContinue;
}

static void end_cb(void * data) {
    printf("end!\n");
}

using namespace std;

int main(int argc, char ** argv) {
    PaStreamParameters outputDeviceParam;
    PaStream *stream;
    PaError err;
    SNDFILE *audioFile;
    SF_INFO soundFileInfo;
    double audioFileLength;

    if (argc < 2) {
        cerr << "Usage " << argv[0] << ": <WAV file>" << endl;
        return -1;
    }

    audioFile = sf_open(argv[1], SFM_READ, &soundFileInfo);
    audioFileLength = soundFileInfo.frames / (double) soundFileInfo.samplerate;

    cout << (int) soundFileInfo.frames << " frames, " << soundFileInfo.samplerate << " samplerate, " <<   soundFileInfo.channels << " channels, " << endl;

    /* init portAudio */
    err = Pa_Initialize();
    error_check(err);

    /* we are using the default device */
    outputDeviceParam.device = Pa_GetDefaultOutputDevice();
    if (outputDeviceParam.device == paNoDevice) {
        cerr << "Haven't found an audio device!" << endl;
        return -1;
    }

    /* stereo or mono */
    outputDeviceParam.channelCount = soundFileInfo.channels;
    outputDeviceParam.sampleFormat = paInt16;
    outputDeviceParam.suggestedLatency = Pa_GetDeviceInfo(outputDeviceParam.device)->defaultLowOutputLatency;
    outputDeviceParam.hostApiSpecificStreamInfo = NULL;

    err = Pa_OpenStream(&stream, NULL, &outputDeviceParam, soundFileInfo.samplerate, paFramesPerBufferUnspecified, paClipOff, output_cb, audioFile);
    error_check(err);

    err = Pa_SetStreamFinishedCallback(stream, &end_cb);
    error_check(err);

    err = Pa_StartStream(stream);
    error_check(err);

    cout << "Playing audio file, length: " << audioFileLength << endl;
    Pa_Sleep((long) (audioFileLength + 0.5) * 1000);

    err = Pa_StopStream(stream);
    error_check(err);

    err = Pa_CloseStream(stream);
    error_check(err);

    sf_close(audioFile);

    Pa_Terminate();

    return 0;
}