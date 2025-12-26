#include "../include/lpfResponse.h"
#include <cmath>

void computehamming()
{
    int M = lpfParamters::filterLength;
    for (int n = 0; n < M; n++)
    {
        lpfParamters::wn[n] =
            0.54f - 0.46f * cos((2 * pi * n) / (M - 1));
    }
}

void computelpfImpuseResponse(callBackUserData &cd)
{
    int M = lpfParamters::filterLength;
    int centeridx = (M - 1) / 2;

    computehamming();

    while (cd.cp.is_running.load())
    {
        if (!cd.lpf.computehn.load()) continue;

        float *inactive =
            (cd.lpf.h_n.load() == lpfParamters::ha)
            ? lpfParamters::hb
            : lpfParamters::ha;

        float fc = cd.lpf.cutofffreq.load();
        float wc = 2.0f * pi * fc / SAMPLE_RATE;

        for (int n = 0; n < M; n++)
        {
            int k = n - centeridx;
            if (k == 0)
                inactive[n] = wc / pi;
            else
                inactive[n] = sin(wc * k) / (pi * k);

            inactive[n] *= lpfParamters::wn[n];
        }

        float sum = 0.0f;
        for (int n = 0; n < M; n++) sum += inactive[n];
        for (int n = 0; n < M; n++) inactive[n] /= sum;

        cd.lpf.h_n.store(inactive);
        cd.lpf.computehn.store(false);
    }
}
