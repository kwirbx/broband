#ifndef PRESETS_H
#define PRESETS_H

#include "FastLED.h"

typedef int (*PresetTick)(CRGB * leds, int numLEDs, void * context);

typedef struct Preset {
    PresetTick tick;
    void * context;
} Preset;

#endif
