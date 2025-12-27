#include "../include/controlT.h"
#include <iostream>

using namespace std;

void controlT(callBackUserData &cD)
{
    char keyPressed = '*';
    int changeBy = 10;
    int fchangeby = 100;
    float newfc = 0;

    while (cD.cp.is_running.load())
    {
        cin >> keyPressed;
        switch (keyPressed)

        {
            //amplification
        case 'q': cD.aP.Amplify_HEADPHONE_L += changeBy; break;
        case 'a': cD.aP.Amplify_HEADPHONE_L -= changeBy; break;
        case 'w': cD.aP.Amplify_HEADPHONE_R += changeBy; break;
        case 's': cD.aP.Amplify_HEADPHONE_R -= changeBy; break;
        

        case 'e':
            newfc = cD.lpf.cutofffreq.load() + fchangeby;
            cD.lpf.cutofffreq.store(newfc);
            cD.lpf.computehn.store(true);
            cout<<"Cutoff - "<<newfc<<endl;
            break;

        case 'd':
            newfc = cD.lpf.cutofffreq.load() - fchangeby;
            if(newfc > 0){
                cD.lpf.cutofffreq.store(newfc);
                cD.lpf.computehn.store(true);
            }
            cout<<"Cutoff - "<<newfc<<endl;
            

            break;

         case 'r':
            newfc = cD.hpf.cutofffreq.load() + fchangeby;
            cD.hpf.cutofffreq.store(newfc);
            cD.hpf.computehn.store(true);
            cout<<"Cutoff - "<<newfc<<endl;
            break;

        case 'f':
            newfc = cD.hpf.cutofffreq.load() - fchangeby;
            if(newfc > 0){
                cD.hpf.cutofffreq.store(newfc);
                cD.hpf.computehn.store(true);
            }
            cout<<"Cutoff - "<<newfc<<endl;
            break;
        case 't':
            newfc = cD.bpf.cutofffreqL.load() + fchangeby;
            cD.bpf.cutofffreqL.store(newfc);
            cD.bpf.computehn.store(true);
            cout<<"BandPass fL - "<<newfc<<endl;
            break;

        case 'g':
            newfc = cD.bpf.cutofffreqL.load() - fchangeby;
            if(newfc > 0){
                cD.bpf.cutofffreqL.store(newfc);
                cD.bpf.computehn.store(true);
            }
            cout<<"BandPass fL - "<<newfc<<endl;
            break;

        case 'y':
            newfc = cD.bpf.cutofffreqH.load() + fchangeby;
            cD.bpf.cutofffreqH.store(newfc);
            cD.bpf.computehn.store(true);
            cout<<"BandPass fH - "<<newfc<<endl;
            break;

        case 'h':
            newfc = cD.bpf.cutofffreqH.load() - fchangeby;
            if(newfc > 0){
                cD.bpf.cutofffreqH.store(newfc);
                cD.bpf.computehn.store(true);
            }
            cout<<"BandPass fH - "<<newfc<<endl;
            break;

         case 'b': 
            cD.bpf.isbpfActive.store(false); 
            cout<<"BPF OFF"<<endl;
            break;
        case '5':
            cD.lpf.islpfActive.store(false);
            cD.hpf.ishpfActive.store(false);
            cD.bpf.isbpfActive.store(true);
            cout<<"BPF ON"<<endl;  
            break;
        

        case 'c': 
            cD.lpf.islpfActive.store(false); 
            cout<<"LPF OFF"<<endl;
            break;
        case '3':
            cD.lpf.islpfActive.store(true);
            cD.hpf.ishpfActive.store(false);
            cout<<"LPF ON"<<endl;  
            break;
        case 'v': 
            cD.hpf.ishpfActive.store(false); 
            cout<<"HPF OFF"<<endl;
            break;
        case '4': 
            cD.hpf.ishpfActive.store(true);
            cD.lpf.islpfActive.store(false);   
            cout<<"HPF ON"<<endl;   
            
            break;
        default: 
        break;
        }
    }
}
