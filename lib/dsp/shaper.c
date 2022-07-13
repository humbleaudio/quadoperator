// Copyright 2016 Will Stockwell

#include <conf/arch_conf.h>

#include "lib/dsp/shaper.h"
#include "lib/math/math.h"

// restrict to -1 to 1 range
float clip(float x) {
  return wminf(1.0f, wmaxf(-1.0f, x));
}

// restrict to -1 to 1 range, smoothly
float soft_clip(float x) {
  if (x > 1.0f) {
    return 1.0f;
  } else if (x < -1.0f) {
    return -1.0f;
  } else {
    return 1.5f * x - 0.5f * x*x*x;
  }
}

float cubic_shape(float x) {
  return wminf(1.0f, wmaxf(-1.0f, 1.5f * x - 0.5f * x*x*x));
}
