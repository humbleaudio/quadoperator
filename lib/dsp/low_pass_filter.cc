// Copyright 2017 Humble Audio LLC

#include "lib/dsp/low_pass_filter.h"

#include "lib/math/math.h"

namespace dsp {

void LowPassFilter::Init(pfloat_t sampleRate, pfloat_t fc, uint8_t numPoles) {
  assert_param(1 <= numPoles && numPoles <= 8);
  numPoles_ = numPoles;

  const pfloat_t dt = 1.0f / sampleRate;
  // See this article for details of this alpha calculation:
  // https://en.wikipedia.org/wiki/Low-pass_filter#Discrete-time_realization
  alpha_ = fc * 2.0f * kPi * dt / (1.0f + fc * 2.0f * kPi * dt);
  oneMinusAlpha_ = 1.0f - alpha_;

  for (uint8_t i = 0; i < numPoles_ + 1; i++) {
    buf_[i] = 0.0f;
  }
}

pfloat_t LowPassFilter::Update(const pfloat_t datum) {
  buf_[0] = datum;
  for (uint8_t i = 1; i < numPoles_ + 1; i++) {
    buf_[i] = oneMinusAlpha_ * buf_[i] + alpha_ * buf_[i-1];
  }
  return Output();
}

pfloat_t LowPassFilter::Output() {
  return buf_[numPoles_];
}

}  // namespace dsp
