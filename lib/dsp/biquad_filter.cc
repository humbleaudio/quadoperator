// Copyright 2015 Will Stockwell

#include "lib/dsp/biquad_filter.h"

namespace dsp {

void BiquadFilter::Init(float a0, float a1, float a2, float b0, float b1, float b2) {  // NOLINT(whitespace/line_length)
  a0_ = a0;
  a1_ = a1;
  a2_ = a2;
  b0_ = b0;
  b1_ = b1;
  b2_ = b2;
}

float BiquadFilter::Process(float sample) {
#ifdef DEBUG  // Save some time in debug mode by not computing the filter
  return sample;
#else
  float y;

  for (uint8_t i = 0; i < 2; i++) {
    y = b0_ * sample + b1_ * x_[i][0] + b2_ * x_[i][1] - a1_ * y_[i][0] - a2_ * y_[i][1];  // NOLINT(whitespace/line_length)

    x_[i][1] = x_[i][0];
    x_[i][0] = sample;
    y_[i][1] = y_[i][0];
    y_[i][0] = y;
    sample = y;
  }

  return y;
#endif  // DEBUG
}

}  // namespace dsp
