#include "../include/main.h"
#include "../include/lpfResponse.h"
#include "../include/controlT.h"
#include "../include/hpfResponse.h"
#include "../include/bandpass.h"
#include "../include/limiter.h"

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

    

    bool islpfActive = userData->lpf.islpfActive.load();
    bool ishpfActive = userData->hpf.ishpfActive.load();
    bool isbpfActive = userData->bpf.isbpfActive.load();
    float *h = ishpfActive ?  userData->hpf.h_n.load(): 
               isbpfActive ?  userData->bpf.h_n.load(): userData->lpf.h_n.load();
    
        
        static float delayL[filterlength] = {};
        static float delayR[filterlength] = {};
        float targetGL = 0.0f;
        float targetGR = 0.0f;

        float xLpeak = 0.0f;
        float xRpeak = 0.0f;

        for (unsigned long i = 0; i < framesPerBuffer; i++)
        {

            float gainR = userData->aP.Amplify_HEADPHONE_R.load();
            float gainL = userData->aP.Amplify_HEADPHONE_L.load();

            xLpeak = in[2*i] > xLpeak ? in[2*i] : xLpeak;
            xRpeak = in[2*i+1] > xRpeak ? in[2*i+1] : xRpeak;

            float attackCoeff = 0.0f;
            float releaseCoeff = 0.0f;

            float xL = in ? in[2*i]   : 0.0f;
            float xR = in ? in[2*i+1] : 0.0f;

            if(userData->lm.islimiterActive.load()){
                    
                    userData->lm.xpeakL.store(xLpeak);
                    userData->lm.xpeakR.store(xRpeak);

                    attackCoeff = userData->lm.attackCoeff.load();
                    releaseCoeff= userData->lm.releaseCoeff.load();
                    //sepearting the generate gain to provide enough time for computation 
                }

            for (int k = filterlength - 1; k > 0; k--)
            {
                delayL[k] = delayL[k - 1];
                delayR[k] = delayR[k - 1];
            }

            delayL[0] = xL;
            delayR[0] = xR;

            float yL = xL, yR = xR;
           
                yL = 0.0f;
                yR = 0.0f;
                for (int k = 0; k < filterlength; k++)
                {
                    yL += h[k] * delayL[k];
                    yR += h[k] * delayR[k];
                }
           
                if(userData->lm.islimiterActive.load()){
                    targetGL = userData->lm.targetGainL.load();
                    targetGR = userData->lm.targetGainR.load(); 
                    
                    if(targetGL < gainL)
                        gainL += (targetGL - gainL) * attackCoeff;
                    else 
                        gainL += (targetGL - gainL) * releaseCoeff;

                    if(targetGR < gainR)
                        gainR += (targetGR - gainR) * attackCoeff;
                    else 
                        gainR += (targetGR - gainR) * releaseCoeff;

                    

                    
                    
                }
            
            
     
                out[2*i]   = gainL * yL;
                out[2*i+1] = gainR * yR;
            
        
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

    userD.aP.Amplify_HEADPHONE_L.store(0.3f);
    userD.aP.Amplify_HEADPHONE_R.store(0.3f);

    userD.lpf.cutofffreq.store(1000);
    userD.lpf.computehn.store(true);
    userD.lpf.islpfActive.store(true);
    userD.lpf.h_n.store(lpfParamters::ha);

    userD.hpf.cutofffreq.store(3000);
    userD.hpf.computehn.store(true);
    userD.hpf.ishpfActive.store(false);
    userD.hpf.h_n.store(hpfParameters::ha);

    userD.bpf.computehn.store(true);
    userD.bpf.cutofffreqL.store(1000);
    userD.bpf.cutofffreqH.store(5000);
    userD.bpf.isbpfActive.store(false);
    userD.bpf.h_n.store(bpfParameters::ha);

    userD.lm.tat.store(0.02e-3);
    userD.lm.trt.store(1e-3);
    userD.lm.compressionfactor.store(2); 
    userD.lm.islimiterActive.store(false);  


    thread ControlThread(controlT, ref(userD));
    thread computeThread(computelpfImpuseResponse, ref(userD));
    thread hpfThread(computehpfimpulse, ref(userD));
    thread bpfThread(computebpfimpulse, ref(userD));
    thread lmThread(computeFL, ref(userD));

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
