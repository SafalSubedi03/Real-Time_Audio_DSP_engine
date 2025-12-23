#include <iostream>
#include "portaudio.h"
#include<atomic>
#include<thread>
#include<cmath>
using namespace std;


//Preprocessor macros
#define SAMPLE_RATE  44100
#define FRAMES_PER_BUFFER 128
#define PROGRAM_DURATION 200



using namespace std;

//Global Configurables 
bool isdisplayactive = false;








//Structure to hold Callback parameters
struct CoreParameters{
    unsigned int short inputChannelCount;
    unsigned int short outputChannelCount;
    uint64_t processedFrames;
    uint64_t targetFrames = (PROGRAM_DURATION * SAMPLE_RATE);
    atomic <bool> is_running;
    

};

struct amplifyParameters{
    atomic <int> Amplify_HEADPHONE_R;
    atomic <int> Amplify_HEADPHONE_L;

};

struct lpfParamters{
    static const int filterLength = 20;
    static float ha[filterLength]; // h_n after computation
    static float hb[filterLength]; // h_n before computation
    
    atomic <float> cutofffreq;
    atomic <float*> h_n;
    atomic <bool> computehn;
    atomic <bool> islpfActive;
};

float lpfParamters::ha[lpfParamters::filterLength] = {};
float lpfParamters::hb[lpfParamters::filterLength] = {};
//UserData for callBackFunction
struct callBackUserData{
    CoreParameters cp;   
    amplifyParameters aP;
    lpfParamters lpf;
};


void computelpfImpuseResponse(callBackUserData& cd){
    float tempStorage = 0.0f;
    int shiftfilterBy =  cd.lpf.filterLength / 2;
    while(cd.cp.is_running.load() && cd.lpf.islpfActive.load()){
        float wc = 2.0f * 3.14159f * cd.lpf.cutofffreq.load() / SAMPLE_RATE;
        float* current = cd.lpf.h_n.load();
        float* inactive = (current == cd.lpf.ha) ? cd.lpf.hb : cd.lpf.ha;

        //check if h_n needs to be computed
        if(cd.lpf.computehn.load()){
            // Compute hn into inactive buffer                
           for(int i = 0; i < cd.lpf.filterLength; i++){
                int n = i - shiftfilterBy;
                if(n == 0)
                    inactive[i] = wc / 3.14159f;   // sinc(0) = wc/pi
                else
                    inactive[i] = sin(wc * n) / (3.14159f * n);
            }
 
            cd.lpf.h_n = inactive;
            cd.lpf.computehn.store(false);
        } 
        //small sleep to avoid busy-wait;
        this_thread::sleep_for(chrono::microseconds(10));  
    }
}




//Control Thread - Takes the input from console and makes changes to the callback parameters on real time
void controlT(callBackUserData& cD){
    char keyPressed = '*';
    int gainL;
    int gainR;
    int changeBy = 10;
    int wChangeBy = 10;
    
    while(cD.cp.is_running.load()){
        cin>>keyPressed;
        switch(keyPressed){
            case 'q' : {
                gainL = cD.aP.Amplify_HEADPHONE_L.load();
                
                cD.aP.Amplify_HEADPHONE_L.store(gainL + changeBy);
                keyPressed = '*'; //A dummy charactor to the keyboard buffer to avoid any infinite update case
                break;
            }
            
            case 'a' : {
                gainL = cD.aP.Amplify_HEADPHONE_L.load();
                if((gainL-changeBy) > 0)
                    cD.aP.Amplify_HEADPHONE_L.store(gainL - changeBy);
                keyPressed = '*'; //A dummy charactor to the keyboard buffer to avoid any infinite update case
                break;
            }

            case 'w' : {
                gainR = cD.aP.Amplify_HEADPHONE_R.load();
                cD.aP.Amplify_HEADPHONE_R.store(gainR + changeBy);
                keyPressed = '*'; //A dummy charactor to the keyboard buffer to avoid any infinite update case
                break;
            }
            
            case 's' : {
                gainR = cD.aP.Amplify_HEADPHONE_R.load();
                if((gainR - changeBy) > 0)
                    cD.aP.Amplify_HEADPHONE_R.store(gainR - changeBy);
                keyPressed = '*'; //A dummy charactor to the keyboard buffer to avoid any infinite update case
                break;
            }

            case 'e' : {
                float wc = cD.lpf.cutofffreq.load();
                if((wc + changeBy) < SAMPLE_RATE/2){
                    cD.lpf.cutofffreq.store(cD.lpf.cutofffreq.load() + wChangeBy);
                    cD.lpf.computehn.store(true);
                }
            }

            case 'd' : {
                float wc = cD.lpf.cutofffreq.load();
                if((wc - wChangeBy) > 0){
                    cD.lpf.cutofffreq.store(wc - wChangeBy);
                    cD.lpf.computehn.store(true);
                }
                break;
            }
            case 'o' : {
                cD.lpf.islpfActive.store(false);
            }

            default : {
                keyPressed = '*'; //A dummy charactor to the keyboard buffer to avoid any infinite update case
                break;
            }
            

        }

    }

}

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

    //Reading the atomic Parameters once per buffer
    int gainR = userData->aP.Amplify_HEADPHONE_R.load();
    int gainL = userData->aP.Amplify_HEADPHONE_L.load();
    bool islpfActive = userData->lpf.islpfActive.load();
    

    for(int i = 0; i<framesPerBuffer * userData->cp.inputChannelCount;i+=2){
        if(in != nullptr){
            if(islpfActive){
                float* h_n = userData->lpf.h_n.load();

                    for(int n = 0; n < framesPerBuffer; n++){
                        float sumL = 0.0f;
                        float sumR = 0.0f;

                        for(int k = 0; k < userData->lpf.filterLength; k++){
                            int idx = n - k;
                            if(idx >= 0){ // only use past samples
                                sumL += in[2*idx] * h_n[k];
                                sumR += in[2*idx+1] * h_n[k];
                            }
                        }

                            out[2*n] = gainL * sumL; 
                            out[2*n+1] = gainR * sumR;
                    }
                }
            else {
            out[i] = gainL * in[i];
            out[i+1] = gainL * in[i+1];
            }
        }
       
        else {
            out[i] = 0.0f;
            out[i+1] = 0.0f;
    }
    
    
}

    userData->cp.processedFrames += framesPerBuffer;
    if(userData->cp.processedFrames > userData->cp.targetFrames)
    {
        userData->cp.is_running.store(false);
        return paComplete;
    }
         
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

    
    const PaDeviceInfo* device_info;
    string devicetype;
    int channels;

    if(isdisplayactive){
        cout<<"ID"<<"\tDeviceType"<<"\tSampleRate"<<"\tNumberOfChannels"<<"\tAPI"<<"\tName"<<endl;
        for(int i = 0;i < Pa_GetDeviceCount();i++){
            device_info = Pa_GetDeviceInfo(i);
            devicetype = device_info->maxInputChannels ? "Input" : (device_info->maxOutputChannels ? "Output" : "Null");
            if(devicetype == "Input")
                channels = device_info->maxInputChannels;
            else if(devicetype == "Output")
                channels = device_info->maxOutputChannels;
            if(device_info->maxInputChannels != 0 || device_info->maxOutputChannels !=0){
                cout<<i<<"\t"<<devicetype<<"\t\t"<<device_info->defaultSampleRate<<"\t\t\t"<<channels<<"\t\t"<<device_info->hostApi<<"\t"<<device_info->name<<endl;
            }
        }

        //Display and terminate the program. The main purpose to display values is to set the valid input and output device
        return 0;

    }
    

    //Chose the devices such that api number,number of channels and sample rate are same for input and output device
    int outputSpeaker = 4; 
    int inputMic      = 0;
    
    
    //Set inputDevice parameters
    PaStreamParameters inputParameter;
    device_info = Pa_GetDeviceInfo(inputMic);

    inputParameter.device       = inputMic;
    inputParameter.channelCount = 2;
    inputParameter.sampleFormat = paFloat32;
    inputParameter.suggestedLatency = device_info->defaultLowInputLatency;
    inputParameter.hostApiSpecificStreamInfo = NULL;

    //Set ouputdevice Parameters
    PaStreamParameters outputParameter;
    device_info = Pa_GetDeviceInfo(outputSpeaker);
    outputParameter.device       = outputSpeaker;
    outputParameter.channelCount = 2;
    outputParameter.sampleFormat = paFloat32;
    outputParameter.suggestedLatency = device_info->defaultLowOutputLatency;
    outputParameter.hostApiSpecificStreamInfo = NULL;

    //Setup Before generating PaStream
        //userData setup
        callBackUserData userD;
        userD.cp.inputChannelCount = 2;
        userD.cp.outputChannelCount = 2;
        userD.cp.processedFrames = 0;   
        userD.cp.targetFrames -= userD.cp.targetFrames % FRAMES_PER_BUFFER; 

        userD.aP.Amplify_HEADPHONE_L.store(20);
        userD.aP.Amplify_HEADPHONE_R.store(20);

        userD.cp.is_running.store(true);
        userD.lpf.computehn.store(true);
        userD.lpf.cutofffreq.store(1000); // in hz
        userD.lpf.islpfActive.store(true);
        userD.lpf.h_n.store(userD.lpf.ha);
        
        


        //Create  thread 
        thread ControlThread(controlT,ref(userD));
        thread computeThread(computelpfImpuseResponse,ref(userD));
        

    // Generate PaStream
    PaStream* stream;
    err = Pa_OpenStream(&stream, &inputParameter, &outputParameter, SAMPLE_RATE, FRAMES_PER_BUFFER, paClipOff, audioCallback, &userD);
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
