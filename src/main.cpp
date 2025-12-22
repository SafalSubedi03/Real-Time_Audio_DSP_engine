#include <iostream>
#include "portaudio.h"

using namespace std;


//Preprocessor macros
#define SAMPLE_RATE  44100
#define FRAMES_PER_BUFFER 512
#define PROGRAM_DURATION 5
#define SPEAKER_AMPLIFICATION_FACTOR 3


//Global Configurables 
static bool display_device_info = true;




//UserData for callBackFunction
struct callBackUserData{
    unsigned int short inputChannelCount;
    unsigned int short outputChannelCount;
    uint64_t processedFrames;
    uint64_t targetFrames = (PROGRAM_DURATION * SAMPLE_RATE);

};

//Pa callback function 

static int audioCallback(const void* inputBuffer,
                         void* outputBuffer, 
                         unsigned long framesPerBuffer, 
                         const PaStreamCallbackTimeInfo*, 
                         PaStreamCallbackFlags, 
                         void* u){


    float* in = (float*)inputBuffer;
    float* out = (float*)outputBuffer;
    callBackUserData* userData = (callBackUserData*)u;

    for(int i = 0; i<framesPerBuffer * userData->inputChannelCount;i++){
        if(in != nullptr)
            out[i] = SPEAKER_AMPLIFICATION_FACTOR*in[i];
        else 
            out[i] = 0.0f;
    
    }

    userData->processedFrames += framesPerBuffer;
    if(userData->processedFrames > userData->targetFrames)
        return paComplete;
         
    return paContinue;
    

}

//helper function to check error 
static void checkError(PaError err){
    if(err != paNoError){
        cout<<"Error:- "<<Pa_GetErrorText(err)<<endl;
        exit(EXIT_FAILURE);
    }
}

int main() {
    cout << "=== PortAudio Test ===" << endl;
    PaError err = Pa_Initialize();
    checkError(err);

    


    int outputSpeaker = Pa_GetDefaultOutputDevice();
    int inputMic      = Pa_GetDefaultInputDevice();
    
    const PaDeviceInfo* device_info;
    
    //Set inputDevice parameters
    PaStreamParameters inputParameter;
    device_info = Pa_GetDeviceInfo(inputMic);

    inputParameter.device       = inputMic;
    inputParameter.channelCount = 2;
    inputParameter.sampleFormat = paFloat32;
    inputParameter.suggestedLatency = device_info->defaultLowInputLatency;
    inputParameter.hostApiSpecificStreamInfo = NULL;

    if(display_device_info){
        cout<<"Name: "<<device_info->name<<endl;
        cout<<"MaxInputChannels: "<<device_info->maxInputChannels<<endl;
        cout<<"defaultLowInputLatencey: "<<device_info->defaultLowInputLatency<<endl;
        cout<<"Sample Rate: "<<device_info->defaultSampleRate<<endl;
        cout<<"HostApi: "<<device_info->hostApi<<endl;
    }

    //Set ouputdevice Parameters
    PaStreamParameters outputParameter;
    device_info = Pa_GetDeviceInfo(outputSpeaker);

    outputParameter.device       = outputSpeaker;
    outputParameter.channelCount = 2;
    outputParameter.sampleFormat = paFloat32;
    outputParameter.suggestedLatency = device_info->defaultLowOutputLatency;
    outputParameter.hostApiSpecificStreamInfo = NULL;

    if(display_device_info){
        cout<<"Name: "<<device_info->name<<endl;
        cout<<"MaxOutputChannels: "<<device_info->maxOutputChannels<<endl;
        cout<<"defaultLowOutputLatencey: "<<device_info->defaultLowOutputLatency<<endl;
        cout<<"Sample Rate: "<<device_info->defaultSampleRate<<endl;
        cout<<"HostApi: "<<device_info->hostApi<<endl;
    }

    //Setup Before generating PaStream
        //userData setup
        callBackUserData u;
        u.inputChannelCount = 2;
        u.outputChannelCount = 2;
        u.processedFrames = 0;   
        u.targetFrames -= u.targetFrames % FRAMES_PER_BUFFER; 

    // Generate PaStream
    PaStream* stream;
    err = Pa_OpenStream(&stream, &inputParameter, &outputParameter, SAMPLE_RATE, FRAMES_PER_BUFFER, paClipOff, audioCallback, &u);
    checkError(err);

    cout<<"----Starting Stream----"<<endl;

    err = Pa_StartStream(stream);
    checkError(err);

    while(Pa_IsStreamActive(stream))
        Pa_Sleep(100);

    err = Pa_CloseStream(stream);
    checkError(err);

    Pa_Terminate();
    return 0;
}
