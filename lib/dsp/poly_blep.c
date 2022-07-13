// Copyright 2016 Will Stockwell
//
// PolyBLEP function for generating bandlimited waveshapes. Algorithm below
// derived from:
// http://www.martin-finke.de/blog/articles/audio-plugins-018-polyblep-oscillator/

#include <conf/arch_conf.h>

#include "lib/dsp/poly_blep.h"

pfloat_t poly_blep(pfloat_t phase, pfloat_t phaseInc, pfloat_t invPhaseInc) {
  if (phase + 1.0f < phaseInc) {
    pfloat_t x = (phase + 1.0f) * invPhaseInc;
    return x + x - x*x - 1.0f;
  } else if (phase > 1.0f - phaseInc) {
    pfloat_t x = (phase - 1.0f) * invPhaseInc;
    return x*x + x + x + 1.0f;
  } else {
    return 0.0f;
  }
}
