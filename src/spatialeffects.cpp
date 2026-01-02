//This file will contain the code for implementation of localization of sound in 3D space by just using amplitude panning
// and interavral time difference ItD

#include<cmath>
#include "..\include\globals.h"
#include "..\include\spatialeffects.h"




void computeITD(callBackUserData &cd){
    float delay_in_frames;


    while(cd.sl.isSpatialActive.load()){

        float angleRad = abs(cd.sl.azimuthalAngle.load() * (pi / 180.0f));

        
        cd.sl.delayinFrames.store(SAMPLE_RATE * headsize* angleRad / 343.0f);
    

    }
    //44100 samples in one second
    
}