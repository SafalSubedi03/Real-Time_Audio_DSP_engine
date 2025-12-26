#include "../include/globals.h"

bool isdisplayactive = false;

float lpfParamters::ha[lpfParamters::filterLength] = {};
float lpfParamters::hb[lpfParamters::filterLength] = {};
float lpfParamters::wn[lpfParamters::filterLength] = {};

float hpfParameters::ha[hpfParameters::hpfilterLength] = {};
float hpfParameters::hb[hpfParameters::hpfilterLength] = {};
float hpfParameters::wn[hpfParameters::hpfilterLength] = {};