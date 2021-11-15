// Copyright 2018 Humble Audio LLC

#ifndef QUADOP_CV_TEST_H_
#define QUADOP_CV_TEST_H_

#include "stmlib/stmlib.h"

#include "quadop/drivers/internal_adc.h"

namespace quadop {

const uint8_t kNumCvTestSets = 6;
const uint32_t kCvTestExternalFm = 0xffffffff;
const uint32_t CvTestSets[kNumCvTestSets][3] = {
  {AdcChannelRatioCv1, AdcChannelShapeCv1, AdcChannelGainCv1},
  {AdcChannelRatioCv2, AdcChannelShapeCv2, AdcChannelGainCv2},
  {AdcChannelRatioCv3, AdcChannelShapeCv3, AdcChannelGainCv3},
  {AdcChannelRatioCv4, AdcChannelShapeCv4, AdcChannelGainCv4},
  {AdcChannelVPerO, AdcChannelLowFreqFm, AdcChannelReset},
  {kCvTestExternalFm, AdcChannelGainCvExt, AdcChannelAlgoCrossfadeCv},
};


class CvTest {
 public:
  CvTest() { }
  ~CvTest() { }

  void Init();
  void HandleSwitchFlip();
  uint8_t CurrentSetIndex();
  pfloat_t Output(uint8_t i);

 private:
  uint8_t cvSetIndex_;

  DISALLOW_COPY_AND_ASSIGN(CvTest);
};

}  // namespace quadop

#endif  // QUADOP_CV_TEST_H_
