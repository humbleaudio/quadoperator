// Copyright 2017 Will Stockwell

#include "quadop/calibration.h"

const uint32_t kEmptyTimestamp = 0xffffffff;

namespace quadop {

void Calibration::Init(uint32_t sampleRate,
  InternalAdc * intAdc,
  ExternalAdc * extAdc,
  Dac * dac,
  Settings * settings) {
  sampleRate_ = sampleRate;
  intAdc_ = intAdc;
  extAdc_ = extAdc;
  dac_ = dac;
  set_ = settings;
  ResetTimestampBuffer();
}

void Calibration::HandleSwitchChange(DigitalPin pin, uint32_t sampleCount) {
  if (state_ == CalibrationStateIdle) {
    AddSwitchTimestamp(sampleCount);
    AddLastFlip(pin);

    if (ShouldCalibrate(sampleCount)) {
      ResetTimestampBuffer();
      state_ = CalibrationStateZeroVoltsPerO;
      SetDacChannels();
    }
  }
}

void Calibration::AdvanceState(uint32_t sampleCount) {
  // Leave Calibration in each state for a half second to allow DAC
  // output signals to flow through
  if (sampleCount - lastStateChangeTimestamp_ < sampleRate_ / 2) {
    return;
  }

  lastStateChangeTimestamp_ = sampleCount;

  switch (state_) {
  case CalibrationStateZeroVoltsPerO:
  {
    ReadVPerOValues(0);
    state_ = CalibrationStateFiveVoltsPerO;
    SetDacChannels();
    break;
  }

  case CalibrationStateFiveVoltsPerO:
  {
    ReadVPerOValues(1);
    state_ = CalibrationStateIdle;
    Calibrate();
    break;
  }
  }
}

CalibrationState Calibration::State() {
  return state_;
}

void Calibration::Calibrate() {
  set_->SetVPerOScale(CalcVPerOScale(adcVpero_));
  set_->SetVPerOOffset(CalcVPerOOffset(adcVpero_));

  for (uint8_t i = 0; i < kNumOps; i++) {
    set_->SetRatioCvVPerOScale(i, CalcVPerOScale(adcRatioCvVpero_[i]));
    set_->SetRatioCvVPerOOffset(i, CalcVPerOOffset(adcRatioCvVpero_[i]));
  }

  // We calculate the offset based on an average of two readings, because
  // it's easy and why not?
  set_->SetArFmOffset(CalcOffset((adcArFm_[0] + adcArFm_[1]) * 0.5f, 0.0f));

  set_->Save();
}

void Calibration::ResetTimestampBuffer() {
  for (uint8_t i = 0; i < kSwitchCount; i++) {
    switchTs_[i] = kEmptyTimestamp;
  }
}

void Calibration::AddSwitchTimestamp(uint32_t sampleCount) {
  switchTs_[tsIndex_] = sampleCount;
  tsIndex_ = (tsIndex_ + 1) % kSwitchCount;
}

void Calibration::AddLastFlip(DigitalPin pin) {
  lastFlipped_[lastFlipIndex_] = pin;
  flipCount_++;
  lastFlipIndex_ = (lastFlipIndex_ + 1) % kSwitchSequenceLength;
}

bool Calibration::ShouldCalibrate(uint32_t sampleCount) {
  for (uint8_t i = 0; i < kSwitchCount; i++) {
    if (switchTs_[i] == kEmptyTimestamp
        || sampleCount - switchTs_[i] > kSwitchTimeWindow * sampleRate_) {
      return false;  // stay idle
    }
  }

  if (flipCount_ < kSwitchSequenceLength) {
    return false;
  }

  for (uint8_t i = 0; i < kSwitchSequenceLength; i++) {
    if (triggerSequence[i]
        != lastFlipped_[(i + lastFlipIndex_) % kSwitchSequenceLength]) {
      return false;
    }
  }

  lastStateChangeTimestamp_ = sampleCount;
  return true;
}

void Calibration::ReadVPerOValues(uint8_t rnd) {
  assert_param(0 <= rnd <= 1);

  adcVpero_[rnd] = intAdc_->Values(AdcChannelVPerO);

  for (uint8_t i = 0; i < kNumOps; i++) {
    adcRatioCvVpero_[i][rnd] = intAdc_->Values(AdcChannelRatioCv1 + i);
  }

  adcArFm_[rnd] = extAdc_->Value();
}

void Calibration::SetDacChannels() {
  pfloat_t l;

  switch (State()) {
  case CalibrationStateZeroVoltsPerO: l = 0.0f / 5.0f; break;
  case CalibrationStateFiveVoltsPerO: l = 5.0f / 5.0f; break;
  }

  dac_->SetChannels(l, l, l, l);
}

pfloat_t Calibration::CalcVPerOScale(pfloat_t * adc) {
  return 5.0f / (adc[1] - adc[0]);
}

pfloat_t Calibration::CalcVPerOOffset(pfloat_t * adc) {
  return 5.0f - 5.0f * adc[1] / (adc[1] - adc[0]);
}

pfloat_t Calibration::CalcOffset(pfloat_t read, pfloat_t expected) {
  return expected - read;
}

}  // namespace quadop
