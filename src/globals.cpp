#include "../include/globals.h"
#include<cmath>

bool isdisplayactive = false;

float lpfParamters::ha[filterlength] = {};
float lpfParamters::hb[filterlength] = {};
float lpfParamters::wn[filterlength] = {};

float hpfParameters::ha[filterlength] = {};
float hpfParameters::hb[filterlength] = {};
float hpfParameters::wn[filterlength] = {};

float bpfParameters::ha[filterlength] = {};
float bpfParameters::wn[filterlength] = {};
float bpfParameters::hb[filterlength] = {};

float limiter::CT = 20 * log10(compressionThreshold);
float limiter::LT = 20 * log10(limitThreshold);