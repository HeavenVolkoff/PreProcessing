#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

#include "portaudio.h"

#define SAMPLE_RATE (44100)

typedef struct{
    int frameIndex;
    int maxFrameIndex;
    char* recordedSamples;
}
        testData;

PaStream* stream;

static int recordCallback(const void* inputBuffer, void* outputBuffer, unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData){
    testData* data = (testData*)userData;
    const char* buffer_ptr = (const char*)inputBuffer;
    char* index_ptr = &data->recordedSamples[data->frameIndex];

    long framesToCalc;
    long i;
    int finished;
    unsigned long framesLeft = data->maxFrameIndex - data->frameIndex;

    if(framesLeft < frameCount){
        framesToCalc = framesLeft;
        finished = paComplete;
    }else{
        framesToCalc = frameCount;
        finished = paContinue;
    }

    if(inputBuffer == NULL){
        for(i = 0; i < framesToCalc; i++){
            *index_ptr++ = 0;
        }
    }else{
        for(i = 0; i < framesToCalc; i++){
            *index_ptr++ = *buffer_ptr++;
        }
    }

    data->frameIndex += framesToCalc;
    return finished;
}

int setup(testData streamData){
    PaError err;

    err = Pa_Initialize();
    if(err != paNoError){
        fprintf(stderr, "Pa_Initialize error: %s\n", Pa_GetErrorText(err));
        return 1;
    }

    PaStreamParameters inputParameters;
    inputParameters.device = Pa_GetDefaultInputDevice();
    if (inputParameters.device == paNoDevice) {
        fprintf(stderr, "Error: No default input device.\n");
        return 1;
    }

    inputParameters.channelCount = 1;
    inputParameters.sampleFormat = paInt8;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    err = Pa_OpenStream(&stream, &inputParameters, NULL, SAMPLE_RATE, 256, paClipOff, recordCallback, &streamData);
    if(err != paNoError){
        fprintf(stderr, "Pa_OpenDefaultStream error: %s\n", Pa_GetErrorText(err));
        return 1;
    }

    err = Pa_StartStream(stream);
    if(err != paNoError){
        fprintf(stderr, "Pa_StartStream error: %s\n", Pa_GetErrorText(err));
        return 1;
    }

    return 0;
}

void quit(testData streamData){
    PaError err;
    err = Pa_Terminate();
    if(err != paNoError){
        fprintf(stderr, "Pa_Terminate error: %s\n", Pa_GetErrorText(err));
    }

    if(streamData.recordedSamples)
        free(streamData.recordedSamples);
}

int main(){
    int i;
    PaError err;
    testData streamData = {0};

    streamData.frameIndex = 0;
    streamData.maxFrameIndex = SAMPLE_RATE;
    streamData.recordedSamples = (char*)malloc(SAMPLE_RATE * sizeof(char));
    if(streamData.recordedSamples == NULL)
        printf("Could not allocate record array.\n");

    for(i=0; i<SAMPLE_RATE; i++)
        streamData.recordedSamples[i] = 0;

    //int totalFrames = SAMPLE_RATE;

    if(!setup(streamData)){
        printf("Opened\n");

        int i = 0;

        while(i++ < 500){

            if((err = Pa_GetStreamReadAvailable(stream)) != paNoError)
                break;

            while((err = Pa_IsStreamActive(stream)) == 1){
                Pa_Sleep(1000);
            }

            err = Pa_CloseStream(stream);
            if(err != paNoError)
                break;

            streamData.frameIndex = 0;
            for(i=0; i<SAMPLE_RATE; i++)
                streamData.recordedSamples[i] = 0;
        }

        if(err != paNoError){
            fprintf(stderr, "Active stream error: %s\n", Pa_GetErrorText(err));
        }

        quit(streamData);
    }else{
        puts("Couldn't open\n");
    }
    return 0;
}