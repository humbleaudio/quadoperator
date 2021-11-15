// Copyright 2017 Will Stockwell

#include "quadop/dsp/clip_indicator.h"

#include <string.h>

#include "lib/math/math.h"

namespace quadop {

void ClipIndicator::Init() {
  bufferIndex_ = 0;
  memset(buffer_, 0, kClipIndicatorBufSize);
}

void ClipIndicator::AddSample(const pfloat_t sample) {
  // Fix sum
  bufferAbsSum_ -= buffer_[bufferIndex_];
  bufferAbsSum_ += wabsf(sample);

  // Update buffer state & index
  buffer_[bufferIndex_] = wabsf(sample);
  bufferIndex_ = (bufferIndex_ + 1) % kClipIndicatorBufSize;
}

pfloat_t ClipIndicator::IndicatorLevel() {
  pfloat_t lvlAvg = bufferAbsSum_ * kClipIndicatorBufSizeInv;

  if (lvlAvg > kClipIndicatorThreshold) {
    return (lvlAvg - kClipIndicatorThreshold) * kClipIndicatorScale;
  } else {
    return 0.0f;
  }
}

}  // namespace quadop
