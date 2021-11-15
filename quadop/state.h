// Copyright 2017 Humble Audio LLC

#ifndef QUADOP_STATE_H_
#define QUADOP_STATE_H_

#include "stmlib/stmlib.h"

#include "quadop/algo.h"
#include "quadop/calibration.h"
#include "quadop/cv_test.h"
#include "quadop/drivers/dac.h"
#include "quadop/drivers/digital_pins.h"
#include "quadop/drivers/external_adc.h"
#include "quadop/drivers/internal_adc.h"
#include "quadop/drivers/pwm_timer.h"
#include "quadop/drivers/scheduler.h"
#include "quadop/dsp/operator_bank.h"
#include "quadop/parameters.h"
#include "quadop/settings.h"

namespace quadop {

class State {
 public:
  State() { }
  ~State() { }

  static quadop::OperatorBank bank;
  static quadop::Algo algo;
  static quadop::Calibration cal;
  static quadop::CvTest cvTest;
  static quadop::Dac dac;
  static quadop::ExternalAdc extAdc;
  static quadop::InternalAdc intAdc;
  static quadop::PwmTimer leds;
  static quadop::Parameters params;
  static quadop::DigitalPins pins;
  static quadop::Scheduler scheduler;
  static quadop::Settings settings;

  static volatile uint32_t sampleCount;
  static volatile bool algoAttached;
  static volatile pfloat_t algoLeftGain;
  static volatile pfloat_t algoRightGain;
  static volatile OperatorBankParameters bankParams;

 private:
  DISALLOW_COPY_AND_ASSIGN(State);
};

}  // namespace quadop

#endif  // QUADOP_STATE_H_
