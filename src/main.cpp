#include <iostream>
#include "portaudio.h"
#include <atomic>
#include <thread>
#include <cmath>
using namespace std;

// Preprocessor macros
#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 128
#define PROGRAM_DURATION 200
#define pi 3.14159

using namespace std;

// Global Configurables
bool isdisplayactive = false;

// Structure to hold Callback parameters
struct CoreParameters
{
    unsigned int short inputChannelCount;
    unsigned int short outputChannelCount;
    uint64_t processedFrames;
    uint64_t targetFrames = (PROGRAM_DURATION * SAMPLE_RATE);
    atomic<bool> is_running;
};

struct amplifyParameters
{
    atomic<int> Amplify_HEADPHONE_R;
    atomic<int> Amplify_HEADPHONE_L;
};

struct lpfParamters
{
    static const int filterLength = 21;
    static float ha[filterLength]; // h_n after computation
    static float hb[filterLength]; // h_n before computation
    static float wn[filterLength];

    atomic<float> cutofffreq;
    atomic<float *> h_n;
    atomic<bool> computehn;
    atomic<bool> islpfActive;
};

float lpfParamters::ha[lpfParamters::filterLength] = {};
float lpfParamters::hb[lpfParamters::filterLength] = {};
float lpfParamters::wn[lpfParamters::filterLength] = {};

// UserData for callBackFunction
struct callBackUserData
{
    CoreParameters cp;
    amplifyParameters aP;
    lpfParamters lpf;
};

void computehamming(){
    int M = lpfParamters::filterLength;
    for (int n = 0; n < M; n++){
        lpfParamters::wn[n] = 0.54f - 0.46f * cos((2 * pi * n) / (M - 1));
    }
}

void computelpfImpuseResponse(callBackUserData &cd)
{
    int M = lpfParamters::filterLength;
    int centeridx = (M - 1) / 2;

    computehamming();

    while (cd.cp.is_running.load())
    {
        if (!cd.lpf.computehn.load()) continue;

        float* inactive = (cd.lpf.h_n.load() == lpfParamters::ha)
                            ? lpfParamters::hb
                            : lpfParamters::ha;

        float fc = cd.lpf.cutofffreq.load();
        float wc = 2.0f * pi * fc / SAMPLE_RATE;

        for(int n = 0; n < M; n++){
            int k = n - centeridx;
            if (k == 0)
                inactive[n] = wc / pi;
            else
                inactive[n] = sin(wc * k) / (pi * k);

            inactive[n] *= lpfParamters::wn[n];
        }

        // normalize
        float sum = 0.0f;
        for(int n = 0; n < M; n++) sum += inactive[n];
        for(int n = 0; n < M; n++) inactive[n] /= sum;

        cd.lpf.h_n.store(inactive);
        cd.lpf.computehn.store(false);
    }
}

// Control Thread - Takes the input from console and makes changes to the callback parameters on real time
void controlT(callBackUserData &cD)
{
    char keyPressed = '*';
    int changeBy = 3;
    int wChangeBy = 100;

    while (cD.cp.is_running.load())
    {
        cin >> keyPressed;
        switch (keyPressed)
        {
        case 'q': cD.aP.Amplify_HEADPHONE_L += changeBy; break;
        case 'a': cD.aP.Amplify_HEADPHONE_L -= changeBy; break;
        case 'w': cD.aP.Amplify_HEADPHONE_R += changeBy; break;
        case 's': cD.aP.Amplify_HEADPHONE_R -= changeBy; break;

        case 'e':
            cD.lpf.cutofffreq.store(cD.lpf.cutofffreq.load() + wChangeBy);
            cD.lpf.computehn.store(true);
            break;

        case 'd':
            cD.lpf.cutofffreq.store(cD.lpf.cutofffreq.load() - wChangeBy);
            cD.lpf.computehn.store(true);
            break;

        case 'c': cD.lpf.islpfActive.store(false); break;
        case '3': cD.lpf.islpfActive.store(true);  break;
        default: break;
        }
    }
}

// Pa callback function
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
    float* h = userData->lpf.h_n.load();
    int M = lpfParamters::filterLength;

    static float delayL[lpfParamters::filterLength] = {};
    static float delayR[lpfParamters::filterLength] = {};

    for (unsigned long i = 0; i < framesPerBuffer; i++)
    {
        float xL = in ? in[2*i]   : 0.0f;
        float xR = in ? in[2*i+1] : 0.0f;

        for (int k = M-1; k > 0; k--) {
            delayL[k] = delayL[k-1];
            delayR[k] = delayR[k-1];
        }
        delayL[0] = xL;
        delayR[0] = xR;

        float yL = xL, yR = xR;

        if (islpfActive) {
            yL = 0.0f;
            yR = 0.0f;
            for (int k = 0; k < M; k++) {
                yL += h[k] * delayL[k];
                yR += h[k] * delayR[k];
            }
        }

        out[2*i]   = 0.1f * gainL * yL;
        out[2*i+1] = 0.1f * gainR * yR;
    }

    userData->cp.processedFrames += framesPerBuffer;
    if (userData->cp.processedFrames > userData->cp.targetFrames)
    {
        userData->cp.is_running.store(false);
        return paComplete;
    }
    return paContinue;
}

// helper function to check error
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
    inputParameter.suggestedLatency = Pa_GetDeviceInfo(inputMic)->defaultLowInputLatency;
    inputParameter.hostApiSpecificStreamInfo = NULL;

    outputParameter.device = outputSpeaker;
    outputParameter.channelCount = 2;
    outputParameter.sampleFormat = paFloat32;
    outputParameter.suggestedLatency = Pa_GetDeviceInfo(outputSpeaker)->defaultLowOutputLatency;
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

    thread ControlThread(controlT, ref(userD));
    thread computeThread(computelpfImpuseResponse, ref(userD));

    PaStream *stream;
    err = Pa_OpenStream(&stream, &inputParameter, &outputParameter,
                        SAMPLE_RATE, FRAMES_PER_BUFFER,
                        paClipOff, audioCallback, &userD);
    checkError(err);

    Pa_StartStream(stream);
    while (Pa_IsStreamActive(stream)) Pa_Sleep(100);

    Pa_CloseStream(stream);
    Pa_Terminate();
    return 0;
}
