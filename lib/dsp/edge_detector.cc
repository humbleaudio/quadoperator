// Copyright 2017 Humble Audio LLC

#include "lib/dsp/edge_detector.h"

#include "lib/math/math.h"

namespace dsp {

void EdgeDetector::Init(pfloat_t highThreshold) {
  highThreshold_ = highThreshold;
  i_ = 0;
  sum_ = 0.0f;
  lastSum_ = 0.0f;

  for (uint8_t i = 0; i < kEdgeDetectorBufferSize; i++) {
    buffer_[i] = 0.0f;
  }
}

bool EdgeDetector::Update(const pfloat_t datum) {
  lastSum_ = sum_;

  sum_ -= buffer_[i_];
  buffer_[i_] = datum;
  sum_ += datum;
  i_ = (i_ + 1) % kEdgeDetectorBufferSize;

  return DidTransitionHigh();
}

bool EdgeDetector::DidTransitionHigh() {
  return IsHigh() && !LastWasHigh();
}

bool EdgeDetector::IsHigh() {
  return sum_ >= highThreshold_ * kEdgeDetectorBufferSize;
}

bool EdgeDetector::LastWasHigh() {
  return lastSum_ >= highThreshold_ * kEdgeDetectorBufferSize;
}

}  // namespace dsp
