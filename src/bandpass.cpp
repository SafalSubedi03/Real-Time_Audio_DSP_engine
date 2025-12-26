#include"../include/bandpass.h"
#include<cmath>

using namespace std;

void computebphamming()
{
    
    for (int n = 0; n < filterlength; n++)
    {
        bpfParameters::wn[n] = 0.54f - 0.46f * cos((2 * pi * n) / (filterlength - 1));
    }
}

void computebpfimpulse(callBackUserData& cd){
    
    int centeridx = (filterlength - 1 ) / 2;

    while(cd.cp.is_running.load()){
        float* inactive = (cd.bpf.h_n.load() == bpfParameters::ha) ? bpfParameters::ha : bpfParameters::hb;
        float wcL =(2 * pi * cd.bpf.cutofffreqL.load()) / SAMPLE_RATE;
        float wcH = (2 * pi * cd.bpf.cutofffreqH.load()) / SAMPLE_RATE;
        float wo = (wcL + wcH) / 2;
        computebphamming();
        

        if (!cd.bpf.computehn.load()) 
            continue;

        //Get a low pass filter response 
        cd.lpf.cutofffreq.store((wcH-wcL)/2);
        
        cd.lpf.computehn.store(true);
        //give some time to the low impulse thread to calculate the impulse response;
        _sleep(100);
        cd.lpf.computehn.store(false);
        inactive = cd.lpf.h_n.load();
        

        for(int n = 0; n < filterlength; n ++){
            int k = n-centeridx;
            inactive[n] *= 2*cos(wo*k)* bpfParameters::wn[n];
        }
        
        cd.bpf.h_n.store(inactive);
        cd.bpf.computehn.store(false);     

        }
        

    }

