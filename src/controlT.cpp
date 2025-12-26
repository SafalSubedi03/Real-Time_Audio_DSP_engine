#include "../include/controlT.h"
#include <iostream>

using namespace std;

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
            cD.lpf.cutofffreq.store(
                cD.lpf.cutofffreq.load() + wChangeBy);
            cD.lpf.computehn.store(true);
            break;

        case 'd':
            cD.lpf.cutofffreq.store(
                cD.lpf.cutofffreq.load() - wChangeBy);
            cD.lpf.computehn.store(true);
            break;

        case 'c': cD.lpf.islpfActive.store(false); break;
        case '3': cD.lpf.islpfActive.store(true);  break;
        default: break;
        }
    }
}
