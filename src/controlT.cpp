#include "../include/controlT.h"
#include <iostream>

using namespace std;

void controlT(callBackUserData &cD)
{
    char keyPressed = '*';
    float changeBy = 10.0f;
    int fchangeby = 100;
    float newfc = 0.0f;
    float newgL = 0.0f;
    float newgR = 0.0f;
    float newtat = 0.0f;
    float newtrt = 0.0f;
    float newcompressionRatio = 0.0f;
    float RchangeBy = 0.5;
    float AzimuthalAngleChangeBy = 5; // degress
    float currAngle;
    float newAngle;

    while (cD.cp.is_running.load())
    {
        cin >> keyPressed;
        switch (keyPressed)

        {
            // amplification
        case 'q':
            newgL = cD.aP.Amplify_HEADPHONE_L.load() + changeBy;

            cD.aP.Amplify_HEADPHONE_L.store(newgL);
            cout << "GL:- " << cD.aP.Amplify_HEADPHONE_L.load() << endl;
            break;

        case 'a':
            newgL = cD.aP.Amplify_HEADPHONE_L.load() - changeBy;
            if (newgL > 0)
                cD.aP.Amplify_HEADPHONE_L.store(newgL);
            cout << "GL:- " << cD.aP.Amplify_HEADPHONE_L.load() << endl;
            break;
        case 'w':
            newgR = cD.aP.Amplify_HEADPHONE_R.load() + changeBy;

            cD.aP.Amplify_HEADPHONE_R.store(newgR);
            cout << "GR:- " << cD.aP.Amplify_HEADPHONE_R.load() << endl;
            break;
        case 's':
            newgR = cD.aP.Amplify_HEADPHONE_R.load() - changeBy;
            if (newgR > 0)
                cD.aP.Amplify_HEADPHONE_R.store(newgR);
            cout << "GR:- " << cD.aP.Amplify_HEADPHONE_R.load() << endl;
            break;

        case 'e':
            newfc = cD.lpf.cutofffreq.load() + fchangeby;
            cD.lpf.cutofffreq.store(newfc);
            cD.lpf.computehn.store(true);
            cout << "Cutoff - " << newfc << endl;
            break;

        case 'd':
            newfc = cD.lpf.cutofffreq.load() - fchangeby;
            if (newfc > 0)
            {
                cD.lpf.cutofffreq.store(newfc);
                cD.lpf.computehn.store(true);
            }
            cout << "Cutoff - " << newfc << endl;

            break;

        case 'r':
            newfc = cD.hpf.cutofffreq.load() + fchangeby;
            cD.hpf.cutofffreq.store(newfc);
            cD.hpf.computehn.store(true);
            cout << "Cutoff - " << newfc << endl;
            break;

        case 'f':
            newfc = cD.hpf.cutofffreq.load() - fchangeby;
            if (newfc > 0)
            {
                cD.hpf.cutofffreq.store(newfc);
                cD.hpf.computehn.store(true);
            }
            cout << "Cutoff - " << newfc << endl;
            break;
        case 't':
            newfc = cD.bpf.cutofffreqL.load() + fchangeby;
            cD.bpf.cutofffreqL.store(newfc);
            cD.bpf.computehn.store(true);
            cout << "BandPass fL - " << newfc << endl;
            break;

        case 'g':
            newfc = cD.bpf.cutofffreqL.load() - fchangeby;
            if (newfc > 0)
            {
                cD.bpf.cutofffreqL.store(newfc);
                cD.bpf.computehn.store(true);
            }
            cout << "BandPass fL - " << newfc << endl;
            break;

        case 'y':
            newfc = cD.bpf.cutofffreqH.load() + fchangeby;
            cD.bpf.cutofffreqH.store(newfc);
            cD.bpf.computehn.store(true);
            cout << "BandPass fH - " << newfc << endl;
            break;

        case 'h':
            newfc = cD.bpf.cutofffreqH.load() - fchangeby;
            if (newfc > 0)
            {
                cD.bpf.cutofffreqH.store(newfc);
                cD.bpf.computehn.store(true);
            }
            cout << "BandPass fH - " << newfc << endl;
            break;

        case 'b':
            cD.bpf.isbpfActive.store(false);
            cout << "BPF OFF" << endl;
            break;
        case '5':
            cD.lpf.islpfActive.store(false);
            cD.hpf.ishpfActive.store(false);
            cD.bpf.isbpfActive.store(true);
            cout << "BPF ON" << endl;
            break;

        case 'c':
            cD.lpf.islpfActive.store(false);
            cout << "LPF OFF" << endl;
            break;
        case '3':
            cD.lpf.islpfActive.store(true);
            cD.hpf.ishpfActive.store(false);
            cout << "LPF ON" << endl;
            break;
        case 'v':
            cD.hpf.ishpfActive.store(false);
            cout << "HPF OFF" << endl;
            break;
        case '4':
            cD.hpf.ishpfActive.store(true);
            cD.lpf.islpfActive.store(false);
            cout << "HPF ON" << endl;

            break;

        // Limiter Controls
        case 'l':
            // implementing toggle control
            cD.lm.islimiterActive.store(!cD.lm.islimiterActive.load());
            cout << "Limiter active:- " << cD.lm.islimiterActive.load() << endl;
            break;

        case 'k':
            newcompressionRatio = cD.lm.compressionfactor.load() + RchangeBy;
            cD.lm.compressionfactor.store(newcompressionRatio);
            cout << "Compression Ratio R:- " << cD.lm.compressionfactor.load() << endl;
            break;

        case ';':
            newcompressionRatio = cD.lm.compressionfactor.load() - RchangeBy;
            if (newcompressionRatio > 0)
                cD.lm.compressionfactor.store(newcompressionRatio);
            cout << "Compression Ratio R:- " << cD.lm.compressionfactor.load() << endl;
            break;

            // Spatial Localization

        case ',':
            cD.sl.isSpatialActive.store(!cD.sl.isSpatialActive.load());
            cout << "Direction Control: " << cD.sl.isSpatialActive.load() << endl;
            break;

        case '>':
            
            
                currAngle = cD.sl.azimuthalAngle.load();
                newAngle = currAngle + AzimuthalAngleChangeBy;
                if (newAngle <= 90)
                    cD.sl.azimuthalAngle.store(newAngle);
                cout << "Angle" << cD.sl.azimuthalAngle.load() << endl;
            
            break;

        case '<':
           
            
                currAngle = cD.sl.azimuthalAngle.load();
                newAngle = currAngle - AzimuthalAngleChangeBy;
                if (newAngle >= -90)
                    cD.sl.azimuthalAngle.store(newAngle);
                cout << "Angle" << cD.sl.azimuthalAngle.load() << endl;
            
            break;
        case '.':
            if (cD.sl.isSpatialActive.load())
            {
                cD.sl.autoRotate.store(!cD.sl.autoRotate.load());
                cout << "Auto Rotate - " << cD.sl.autoRotate.load() << endl;
            }
            break;
  

        default:
            break;
        }
    }
}
