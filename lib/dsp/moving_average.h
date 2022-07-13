// Copyright 2018 Humble Audio LLC
//
// Simple moving average

#ifndef LIB_DSP_MOVING_AVERAGE_H_
#define LIB_DSP_MOVING_AVERAGE_H_

#include <conf/arch_conf.h>

#include "stmlib/stmlib.h"

const uint8_t kMovingAverageBufSize = 8;
const pfloat_t kMovingAverageInvBufSize = 1.0f / kMovingAverageBufSize;

namespace dsp {

class MovingAverage {
 public:
  MovingAverage() { }
  ~MovingAverage() { }

  void Init();
  pfloat_t Update(const pfloat_t datum);
  pfloat_t Output();

 private:
  pfloat_t sum_;
  uint8_t i_;
  float buf_[kMovingAverageBufSize];

  DISALLOW_COPY_AND_ASSIGN(MovingAverage);
};

}  // namespace dsp

#endif  // LIB_DSP_MOVING_AVERAGE_H_
