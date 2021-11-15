// Copyright 2017 Will Stockwell

#ifndef QUADOP_CALIBRATION_H_
#define QUADOP_CALIBRATION_H_

#include "stmlib/stmlib.h"

#include "quadop/drivers/dac.h"
#include "quadop/drivers/digital_pins.h"
#include "quadop/drivers/external_adc.h"
#include "quadop/drivers/internal_adc.h"
#include "quadop/dsp/operator_bank.h"
#include "quadop/settings.h"

namespace quadop {

const uint8_t kSwitchCount = 5;  // 5 flips
const uint8_t kSwitchTimeWindow = 5;  // 5 seconds
const uint8_t kSwitchSequenceLength = 9;

typedef enum {
  CalibrationStateIdle = 0,
  CalibrationStateZeroVoltsPerO,
  CalibrationStateFiveVoltsPerO
} CalibrationState;

// The switch flipping sequence that triggers calibration is
// each of the operator state lock switches in order,
// then vco/lfo 5 times fast
const DigitalPin triggerSequence[kSwitchSequenceLength] = {
  DigitalPinLockSwitch1,
  DigitalPinLockSwitch2,
  DigitalPinLockSwitch3,
  DigitalPinLockSwitch4,
  DigitalPinVcoLfoSwitch,
  DigitalPinVcoLfoSwitch,
  DigitalPinVcoLfoSwitch,
  DigitalPinVcoLfoSwitch,
  DigitalPinVcoLfoSwitch
};

class Calibration {
 public:
  Calibration() { }
  ~Calibration() { }

  void Init(uint32_t sampleRate,
            InternalAdc * intAdc,
            ExternalAdc * extAdc,
            Dac * dac,
            Settings * settings);
  void HandleSwitchChange(DigitalPin pin, uint32_t sampleCount);
  void AdvanceState(uint32_t sampleCount);
  CalibrationState State();

 private:
  void ResetTimestampBuffer();
  void AddSwitchTimestamp(uint32_t sampleCount);
  void AddLastFlip(DigitalPin pin);
  bool ShouldCalibrate(uint32_t sampleCount);
  void Calibrate();
  void ReadVPerOValues(uint8_t rnd);
  void ReadOffsetValues();
  void SetDacChannels();
  pfloat_t CalcVPerOScale(pfloat_t * adc);
  pfloat_t CalcVPerOOffset(pfloat_t * adc);
  pfloat_t CalcOffset(pfloat_t read, pfloat_t expected);

  InternalAdc * intAdc_;
  ExternalAdc * extAdc_;
  Dac * dac_;
  Settings * set_;
  CalibrationState state_;

  uint32_t sampleRate_;
  uint32_t switchTs_[kSwitchCount];
  uint8_t tsIndex_;
  uint32_t lastStateChangeTimestamp_;

  DigitalPin lastFlipped_[kSwitchSequenceLength];
  uint8_t lastFlipIndex_;
  uint32_t flipCount_;

  pfloat_t adcVpero_[2];  // first is 1v, second is 3v
  pfloat_t adcRatioCvVpero_[kNumOps][2];  // first is 1v, second is 3v
  pfloat_t adcArFm_[2];

  DISALLOW_COPY_AND_ASSIGN(Calibration);
};

}  // namespace quadop

#endif  // QUADOP_CALIBRATION_H_
