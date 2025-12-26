#ifndef MAIN_H
#define MAIN_H

#include "globals.h"
#include "portaudio.h"

int audioCallback(const void *, void *, unsigned long,
                  const PaStreamCallbackTimeInfo *,
                  PaStreamCallbackFlags, void *);

void checkError(PaError err);

#endif
