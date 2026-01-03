// This file will contain the code for implementation of localization of sound in 3D space by just using amplitude panning
//  and interavral time difference ItD

#include <cmath>
#include "..\include\globals.h"
#include <thread>
#include "..\include\spatialeffects.h"

void computeITD(callBackUserData &cd)
{
    float angle = 0.0f; // start at center
    float step = 2.0f;  // degrees per update
    int direction = +1; // +1 or -1

    while (cd.sl.isSpatialActive.load())
    {
        // update angle
        if (cd.sl.autoRotate.load())
        {
            angle += direction * step;

            // boundary handling
            if (angle >= 90.0f)
            {
                angle = 90.0f;
                direction = -1;
            }
            else if (angle <= -90.0f)
            {
                angle = -90.0f;
                direction = +1;
            }

            // store angle (used by audio thread)
            cd.sl.azimuthalAngle.store(angle);
        } else 
            angle = cd.sl.azimuthalAngle.load();

        // Basic ITD calculation 
        float angleRad = fabs(angle) * (pi / 180.0f);
        
         
        float delay = SAMPLE_RATE * headsize * sin(angleRad) / 343.0f;
               

        cd.sl.delayinFrames.store(static_cast<int>(delay));
        cd.sl.FarEarGain.store(pow(10.0f, 0.3f * sin(angleRad)));

        // control update rate
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
