// Copyright 2017 Humble Audio LLC

#ifndef LIB_DSP_EXP_AVERAGE_H_
#define LIB_DSP_EXP_AVERAGE_H_

#include <conf/arch_conf.h>

#include "stmlib/stmlib.h"

namespace dsp {

class ExpAverage {
 public:
  ExpAverage() { }
  ~ExpAverage() { }

  void Init(pfloat_t alpha);
  pfloat_t Update(const pfloat_t datum);
  pfloat_t Average();

 private:
  pfloat_t alpha_;
  pfloat_t oneMinusAlpha_;
  float avg_;

  DISALLOW_COPY_AND_ASSIGN(ExpAverage);
};

}  // namespace dsp

#endif  // LIB_DSP_EXP_AVERAGE_H_
