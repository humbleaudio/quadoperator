// Copyright 2017 Humble Audio LLC
//
// Up to 8 pole IIR low pass filter

#ifndef LIB_DSP_LOW_PASS_FILTER_H_
#define LIB_DSP_LOW_PASS_FILTER_H_

#include <conf/arch_conf.h>

#include "stmlib/stmlib.h"

namespace dsp {

class LowPassFilter {
 public:
  LowPassFilter() { }
  ~LowPassFilter() { }

  void Init(pfloat_t sampleRate, pfloat_t fc, uint8_t numPoles);
  pfloat_t Update(const pfloat_t datum);
  pfloat_t Output();

 private:
  uint8_t numPoles_;
  pfloat_t alpha_;
  pfloat_t oneMinusAlpha_;
  float buf_[9];  // Up to 8 poles plus a slot for the input sample

  DISALLOW_COPY_AND_ASSIGN(LowPassFilter);
};

}  // namespace dsp

#endif  // LIB_DSP_LOW_PASS_FILTER_H_
