#include "..\include\globals.h"
#include "..\include\limiter.h"
#include<cmath>
#include<thread>

using namespace std;

void computeFL(callBackUserData& cd){
    while(cd.cp.is_running.load()){
        if(!cd.lm.islimiterActive.load()){
            std::this_thread::sleep_for(std::chrono::microseconds(5));
            continue;
        }


        float cf = cd.lm.compressionfactor.load();
        float xpL = cd.lm.xpeakL.load();
        float xpR = cd.lm.xpeakR.load();
        
        if(cf <= 0.0f || xpL <= 0.0f || xpR <= 0.0f) continue; // safety check
        

        float LS = 20 * log10(1.0f - 1.0f/(cf + 1e-10f));
        float CS = 20 * log10(cf);
        float XSL = 20 * log10(xpL);
        float XSR = 20 * log10(xpR);
        float LT = limiter::LT;
        float CT = limiter::CT;
        float FNL = 0.0f;
        float FNR = 0.0f;


        if(XSL > LT)
            FNL = cf * (LT - XSL); //Expression for static functional value of limiter in DB/ logarithmic scale
        else
            FNL = 0.0f;       
       
        if(XSR > LT)
           FNR = cf * (LT - XSR);
        else 
            FNL = 0.0F;

        float attackCoeff = 1 - exp(-2.2/ (SAMPLE_RATE * cd.lm.tat.load()));
        float releaseCoeff = 1 - exp(-2.2/ (SAMPLE_RATE * cd.lm.trt.load()));
        
        cd.lm.targetGainL.store(pow(10.0f,FNL/20), std::memory_order_relaxed);
        cd.lm.targetGainR.store(pow(10.0f,FNR/20), std::memory_order_relaxed);
        cd.lm.attackCoeff.store(attackCoeff);
        cd.lm.releaseCoeff.store(releaseCoeff);
    }
}
