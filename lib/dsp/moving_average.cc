// Copyright 2018 Humble Audio LLC

#include "lib/dsp/moving_average.h"

namespace dsp {

void MovingAverage::Init() {
  i_ = 0;

  for (uint8_t i = 0; i < kMovingAverageBufSize; i++) {
    buf_[i] = 0.0f;
  }
}

pfloat_t MovingAverage::Update(const pfloat_t datum) {
  sum_ -= buf_[i_];
  buf_[i_] = datum * kMovingAverageInvBufSize;
  sum_ += buf_[i_];
  i_ = (i_ + 1) % kMovingAverageBufSize;

  return Output();
}

pfloat_t MovingAverage::Output() {
  return sum_;
}

}  // namespace dsp
