// Copyright 2017 Will Stockwell

#ifndef QUADOP_DSP_CLIP_INDICATOR_H_
#define QUADOP_DSP_CLIP_INDICATOR_H_

#include "conf/arch_conf.h"
#include "stmlib/stmlib.h"

namespace quadop {

const uint8_t kClipIndicatorBufSize = 24;
const pfloat_t kClipIndicatorBufSizeInv
  = 1.0f / static_cast<pfloat_t>(kClipIndicatorBufSize);
const pfloat_t kClipIndicatorThreshold = 0.95f;  // adjustable, from 0.0 to 1.0
const pfloat_t kClipIndicatorScale = 1.0f / (1.0f - kClipIndicatorThreshold);

class ClipIndicator {
 public:
  ClipIndicator() { }
  ~ClipIndicator() { }

  void Init();
  void AddSample(const pfloat_t sample);
  pfloat_t IndicatorLevel();

 private:
  pfloat_t buffer_[kClipIndicatorBufSize];
  pfloat_t bufferAbsSum_;
  uint8_t bufferIndex_;

  DISALLOW_COPY_AND_ASSIGN(ClipIndicator);
};

}  // namespace quadop

#endif  // QUADOP_DSP_CLIP_INDICATOR_H_
