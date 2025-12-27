#ifndef GLOBALS_H
#define GLOBALS_H

#include <atomic>
#include <cstdint>

#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 128
#define PROGRAM_DURATION 200
#define pi 3.141592
#define filterlength 51

extern bool isdisplayactive;

struct CoreParameters
{
    unsigned int short inputChannelCount;
    unsigned int short outputChannelCount;
    uint64_t processedFrames;
    uint64_t targetFrames = (PROGRAM_DURATION * SAMPLE_RATE);
    std::atomic<bool> is_running;
};

struct amplifyParameters
{
    std::atomic<int> Amplify_HEADPHONE_R;
    std::atomic<int> Amplify_HEADPHONE_L;
};

struct lpfParamters
{
    
    static float ha[filterlength];
    static float hb[filterlength];
    static float wn[filterlength];

    std::atomic<float> cutofffreq;
    std::atomic<float *> h_n;
    std::atomic<bool> computehn;
    std::atomic<bool> islpfActive;
};

struct hpfParameters{
    
    static float ha[filterlength];
    static float hb[filterlength];
    static float wn[filterlength];

    std::atomic<float> cutofffreq;
    
    std::atomic<float *> h_n;
    std::atomic<bool> computehn;
    std::atomic<bool> ishpfActive;
};

struct bpfParameters{
    
    static float ha[filterlength];
    static float hb[filterlength];
    static float wn[filterlength];

    std::atomic<float> cutofffreqL;
    std::atomic<float> cutofffreqH;
    std::atomic<float*> h_n;
    std::atomic<bool> computehn;
    std::atomic<bool> isbpfActive;
};
struct callBackUserData
{
    CoreParameters cp;
    amplifyParameters aP;
    lpfParamters lpf;
    hpfParameters hpf;
    bpfParameters bpf;
};
#endif
