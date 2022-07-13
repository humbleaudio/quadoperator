// Copyright 2015 Will Stockwell

#ifndef LIB_DSP_BIQUAD_FILTER_H_
#define LIB_DSP_BIQUAD_FILTER_H_

#include "stmlib/stmlib.h"

namespace dsp {

class BiquadFilter {
 public:
  BiquadFilter() { }
  ~BiquadFilter() { }

  void Init(float a0, float a1, float a2, float b0, float b1, float b2);
  float Process(float samples);

 private:
  float a0_, a1_, a2_, b0_, b1_, b2_;
  float x_[2][2];
  float y_[2][2];

  DISALLOW_COPY_AND_ASSIGN(BiquadFilter);
};

}  // namespace dsp

#endif  // LIB_DSP_BIQUAD_FILTER_H_
