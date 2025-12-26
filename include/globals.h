#ifndef GLOBALS_H
#define GLOBALS_H

#include <atomic>
#include <cstdint>

#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 128
#define PROGRAM_DURATION 200
#define pi 3.14159

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
    static const int filterLength = 21;
    static float ha[filterLength];
    static float hb[filterLength];
    static float wn[filterLength];

    std::atomic<float> cutofffreq;
    std::atomic<float *> h_n;
    std::atomic<bool> computehn;
    std::atomic<bool> islpfActive;
};

struct hpfParameters{
    static const int hpfilterLength = 21;
    static float ha[hpfilterLength];
    static float hb[hpfilterLength];
    static float wn[hpfilterLength];

    std::atomic<float> cutofffreq;
    std::atomic<float *> h_n;
    std::atomic<bool> computehn;
    std::atomic<bool> ishpfActive;
};

struct callBackUserData
{
    CoreParameters cp;
    amplifyParameters aP;
    lpfParamters lpf;
    hpfParameters hpf;
};

#endif
