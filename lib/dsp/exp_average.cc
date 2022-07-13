// Copyright 2017 Humble Audio LLC

#include "lib/dsp/exp_average.h"

namespace dsp {

void ExpAverage::Init(pfloat_t alpha) {
  alpha_ = alpha;
  oneMinusAlpha_ = 1.0f - alpha_;
  avg_ = 0.0f;
}

pfloat_t ExpAverage::Update(const pfloat_t datum) {
  avg_ = oneMinusAlpha_ * avg_ + alpha_ * datum;
  return avg_;
}

pfloat_t ExpAverage::Average() {
  return avg_;
}

}  // namespace dsp
