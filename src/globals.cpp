#include "../include/globals.h"

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