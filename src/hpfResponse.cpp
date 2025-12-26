#include "../include/hpfResponse.h"
#include "../include/globals.h"
#include <cmath>

using namespace std;

void computehphamming()
{
    
    for (int n = 0; n < filterlength; n++)
    {
        hpfParameters::wn[n] = 0.54f - 0.46f * cos((2 * pi * n) / (filterlength - 1));
    }
}

void computehpfimpulse(callBackUserData& cd){
    
    
    int centeridx = (filterlength-1) / 2;
    computehphamming();

    while(cd.cp.is_running.load()){
        if(!cd.hpf.computehn.load()) continue;

        float* inactive = (cd.hpf.h_n.load() == cd.hpf.ha) ? hpfParameters::hb : hpfParameters::ha;
        float fc = cd.hpf.cutofffreq.load()/ SAMPLE_RATE;
        float wc = 2*3.14159*fc;
        for(int n = 0;n<filterlength;n++){
            int k = n - centeridx;
            if(k == 0){
                inactive[n] = 1 - 2 * fc;
            }
            else{
                inactive[n] = -sin(wc*k) / (3.14159 * k);
            }
            inactive[n] *= hpfParameters::wn[n];
        }

        

        cd.hpf.h_n.store(inactive);
        cd.hpf.computehn.store(false);
    }
}