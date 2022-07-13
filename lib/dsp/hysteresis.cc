// Copyright 2017 Humble Audio LLC

#include "lib/dsp/hysteresis.h"

#include "lib/math/math.h"

namespace dsp {

void Hysteresis::Init(pfloat_t unitChangeThreshold) {
  unitChangeThres_ = unitChangeThreshold;
  unit_ = -1.0f;
}

pfloat_t Hysteresis::Update(const pfloat_t unit, const pfloat_t val) {
  if (wabsf(unit - unit_) > unitChangeThres_ || unit_ == -1.0f) {
    unit_ = unit;
    val_ = val;
  }

  return val_;
}

pfloat_t Hysteresis::Value() {
  return val_;
}

}  // namespace dsp
