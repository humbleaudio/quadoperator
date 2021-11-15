// Copyright 2017 Humble Audio LLC
//
// Abstraction to process inputs from ADCs into calibrated, noise-reduced,
// meaningful representations

#ifndef QUADOP_PARAMETERS_H_
#define QUADOP_PARAMETERS_H_

#include <conf/arch_conf.h>

#include "lib/dsp/edge_detector.h"
#include "lib/dsp/moving_average.h"
#include "lib/dsp/hysteresis.h"
#include "quadop/drivers/internal_adc.h"
#include "quadop/drivers/external_adc.h"
#include "quadop/dsp/clip_indicator.h"
#include "quadop/settings.h"

typedef enum {
  OscStateLfo,
  OscStateVco
} OscState;

namespace quadop {

typedef struct {
  dsp::Hysteresis ratioHys;  // for use in stepped lock state
  dsp::MovingAverage ratioKnobAvg;  // for use in free state
  dsp::MovingAverage ratioCvAvg;  // for use in free state
  dsp::MovingAverage detuneAvg;
  dsp::MovingAverage shapeAvg;
  dsp::MovingAverage gainAvg;
  dsp::MovingAverage modAvg[kNumOps+1];  // +1 term for ext signal
} OperatorInputDspState;

class Parameters {
 public:
  Parameters() { }
  ~Parameters() { }

  void Init(InternalAdc * intAdc, ExternalAdc * extAdc, Settings * settings);

  void UpdateMasterFrequency(OscState oscState);
  pfloat_t UpdateExternalLevel();
  pfloat_t LowFreqFm();
  bool Reset();
  pfloat_t GainExt();
  pfloat_t Gain(uint8_t channel);
  pfloat_t Shape(uint8_t channel);
  pfloat_t Mod(uint8_t slot, uint8_t source, uint8_t dest);
  pfloat_t LockedFrequency(uint8_t channel);
  pfloat_t FreeFrequency(uint8_t channel);
  pfloat_t ClipIndicatorLevel();
  pfloat_t AlgoRightGain();

 private:

  OscState oscState_;
  volatile pfloat_t masterFreq_;
  quadop::InternalAdc * intAdc_;
  quadop::ExternalAdc * extAdc_;
  quadop::Settings * settings_;
  OperatorInputDspState op_[kNumOps];
  dsp::MovingAverage coarseAvg_;
  dsp::MovingAverage fineAvg_;
  dsp::MovingAverage gainExtAvg_;  // +1 term for ext signal
  dsp::MovingAverage crossFadeAvg_;
  dsp::MovingAverage lowFreqFmAvg_;
  dsp::EdgeDetector resetDetect_;
  quadop::ClipIndicator clipInd_;

  DISALLOW_COPY_AND_ASSIGN(Parameters);
};

}  // namespace quadop

#endif  // QUADOP_PARAMETERS_H_
