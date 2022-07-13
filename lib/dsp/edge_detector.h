// Copyright 2017 Humble Audio LLC

#ifndef LIB_DSP_EDGE_DETECTOR_H_
#define LIB_DSP_EDGE_DETECTOR_H_

#include <conf/arch_conf.h>

#include "stmlib/stmlib.h"

const uint8_t kEdgeDetectorBufferSize = 3;

namespace dsp {

class EdgeDetector {
 public:
  EdgeDetector() { }
  ~EdgeDetector() { }

  void Init(pfloat_t highThreshold);
  bool Update(const pfloat_t datum);  // returns latest DidTransition() result
  bool DidTransitionHigh();

 private:
  bool IsHigh();
  bool LastWasHigh();

  pfloat_t highThreshold_;
  pfloat_t buffer_[kEdgeDetectorBufferSize];
  pfloat_t sum_;
  pfloat_t lastSum_;
  uint8_t i_;

  DISALLOW_COPY_AND_ASSIGN(EdgeDetector);
};

}  // namespace dsp

#endif  // LIB_DSP_EDGE_DETECTOR_H_
