#include "../include/globals.h"

bool isdisplayactive = false;

float lpfParamters::ha[lpfParamters::filterLength] = {};
float lpfParamters::hb[lpfParamters::filterLength] = {};
float lpfParamters::wn[lpfParamters::filterLength] = {};
