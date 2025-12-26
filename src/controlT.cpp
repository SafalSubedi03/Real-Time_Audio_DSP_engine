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
