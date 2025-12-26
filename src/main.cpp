#include "../include/main.h"
#include "../include/lpfResponse.h"
#include "../include/controlT.h"
#include "../include/hpfResponse.h"

#include <iostream>
#include <thread>

using namespace std;

static int audioCallback(const void *inputBuffer,
                         void *outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo *,
                         PaStreamCallbackFlags,
                         void *u)
{
    float *in = (float *)inputBuffer;
    float *out = (float *)outputBuffer;
    callBackUserData *userData = (callBackUserData *)u;

    int gainR = userData->aP.Amplify_HEADPHONE_R.load();
    int gainL = userData->aP.Amplify_HEADPHONE_L.load();
    bool islpfActive = userData->lpf.islpfActive.load();
    bool ishpfActive = userData->hpf.ishpfActive.load();
    float *h = ishpfActive ?  userData->hpf.h_n.load(): userData->lpf.h_n.load();
    int M = ishpfActive ?  hpfParameters::hpfilterLength :lpfParamters::filterLength;
        
        static float delayL[lpfParamters::filterLength] = {};
        static float delayR[lpfParamters::filterLength] = {};

        for (unsigned long i = 0; i < framesPerBuffer; i++)
        {
            float xL = in ? in[2*i]   : 0.0f;
            float xR = in ? in[2*i+1] : 0.0f;

            for (int k = M - 1; k > 0; k--)
            {
                delayL[k] = delayL[k - 1];
                delayR[k] = delayR[k - 1];
            }

            delayL[0] = xL;
            delayR[0] = xR;

            float yL = xL, yR = xR;
           
                yL = 0.0f;
                yR = 0.0f;
                for (int k = 0; k < M; k++)
                {
                    yL += h[k] * delayL[k];
                    yR += h[k] * delayR[k];
                }
            
            
     
                out[2*i]   = 0.1f * gainL * yL;
                out[2*i+1] = 0.1f * gainR * yR;
        
    }



    userData->cp.processedFrames += framesPerBuffer;
    if (userData->cp.processedFrames >
        userData->cp.targetFrames)
    {
        userData->cp.is_running.store(false);
        return paComplete;
    }

    return paContinue;
}

static void checkError(PaError err)
{
    if (err != paNoError)
    {
        cout << "Error:- " << Pa_GetErrorText(err) << endl;
        exit(EXIT_FAILURE);
    }
}

int main()
{
    cout << "=== PortAudio Test ===" << endl;

    PaError err = Pa_Initialize();
    checkError(err);

    int outputSpeaker = Pa_GetDefaultOutputDevice();
    int inputMic = Pa_GetDefaultInputDevice();

    PaStreamParameters inputParameter;
    PaStreamParameters outputParameter;

    inputParameter.device = inputMic;
    inputParameter.channelCount = 2;
    inputParameter.sampleFormat = paFloat32;
    inputParameter.suggestedLatency =
        Pa_GetDeviceInfo(inputMic)->defaultLowInputLatency;
    inputParameter.hostApiSpecificStreamInfo = NULL;

    outputParameter.device = outputSpeaker;
    outputParameter.channelCount = 2;
    outputParameter.sampleFormat = paFloat32;
    outputParameter.suggestedLatency =
        Pa_GetDeviceInfo(outputSpeaker)->defaultLowOutputLatency;
    outputParameter.hostApiSpecificStreamInfo = NULL;

    callBackUserData userD;
    userD.cp.inputChannelCount = 2;
    userD.cp.outputChannelCount = 2;
    userD.cp.processedFrames = 0;
    userD.cp.is_running.store(true);

    userD.aP.Amplify_HEADPHONE_L.store(10);
    userD.aP.Amplify_HEADPHONE_R.store(10);

    userD.lpf.cutofffreq.store(1000);
    userD.lpf.computehn.store(true);
    userD.lpf.islpfActive.store(true);
    userD.lpf.h_n.store(lpfParamters::ha);

    userD.hpf.cutofffreq.store(3000);
    userD.hpf.computehn.store(true);
    userD.hpf.ishpfActive.store(false);
    userD.hpf.h_n.store(hpfParameters::ha);


    thread ControlThread(controlT, ref(userD));
    thread computeThread(computelpfImpuseResponse, ref(userD));
    thread hpfThread(computehpfimpulse, ref(userD));

    PaStream *stream;
    err = Pa_OpenStream(&stream,
                        &inputParameter,
                        &outputParameter,
                        SAMPLE_RATE,
                        FRAMES_PER_BUFFER,
                        paClipOff,
                        audioCallback,
                        &userD);
    checkError(err);

    Pa_StartStream(stream);
    while (Pa_IsStreamActive(stream)) Pa_Sleep(100);

    Pa_CloseStream(stream);
    Pa_Terminate();
    return 0;
}
