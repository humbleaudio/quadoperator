// Copyright 2018 Humble Audio LLC

#include "quadop/cv_test.h"

#include "quadop/state.h"

typedef quadop::State s;

namespace quadop {

void CvTest::Init() {
  cvSetIndex_ = 0;
}

void CvTest::HandleSwitchFlip() {
  cvSetIndex_ = (cvSetIndex_ + 1) % kNumCvTestSets;
}

uint8_t CvTest::CurrentSetIndex() {
  return cvSetIndex_;
}

pfloat_t CvTest::Output(uint8_t i) {
  if (i == 0) {
    return s::bank.OperatorLevel(0);
  } else {  // CV passthrough
    uint32_t cvChannel = CvTestSets[cvSetIndex_][i-1];

    if (cvChannel == kCvTestExternalFm) {
      return s::extAdc.Value();
    } else {
      return s::intAdc.Values(AdcChannelCoarse);
    }
  }
}

}  // namespace quadop