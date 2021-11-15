// Copyright 2017 Humble Audio LLC

#include "quadop/drivers/scheduler.h"

#include "lib/math/math.h"
#include "quadop/dsp/operator_bank.h"
#include "quadop/parameters.h"
#include "quadop/state.h"

typedef quadop::State s;

bool blink_high() {
  return s::sampleCount % s::scheduler.Freq() < s::scheduler.Freq() / 2;
}

namespace quadop {

void SchedulerBinder::Run() {
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_SET);

  if (s::cal.State() == CalibrationStateIdle) {
#ifdef DEBUG
    s::dac.SetChannels(
      s::bank.OperatorLevel(0),
      s::bank.OperatorLevel(1),
      0.0f,
      0.0f);
#elif defined(CVTEST)
    s::dac.SetChannels(
        s::cvTest.Output(0),
        s::cvTest.Output(1),
        s::cvTest.Output(2),
        s::cvTest.Output(3));
#else
    s::dac.SetChannels(
      s::bank.OperatorLevel(0),
      s::bank.OperatorLevel(1),
      s::bank.OperatorLevel(2),
      s::bank.OperatorLevel(3));
#endif
  }

  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_RESET);

  pfloat_t extLevel = s::params.UpdateExternalLevel();

  if (s::params.Reset()) {
    s::bank.Reset();
  }

  uint8_t iteration = s::sampleCount % 6;

  switch (iteration) {
  case 0:
  {
    OscState oscState
      = s::pins.IsSet(DigitalPinVcoLfoSwitch) ? OscStateLfo : OscStateVco;
    s::params.UpdateMasterFrequency(oscState);
    s::bankParams.gain[kExtId] = s::params.GainExt();

    // effectively a no-op when algo is not attached
    s::algoRightGain = s::params.AlgoRightGain();
    s::algoLeftGain  = 1.0f - s::algoRightGain;

    break;
  }

  case 1:
  case 2:
#ifndef DEBUG
  case 3:
  case 4:
#endif
  {
    uint8_t i = iteration - 1;

    s::bankParams.gain[i] = s::params.Gain(i);
    s::bankParams.variable[i] = s::params.Shape(i);
    s::bankParams.frequency[i] = s::pins.IsSet(i) ?  // Pin set means 'free'
      s::params.FreeFrequency(i) : s::params.LockedFrequency(i);

    for (uint8_t j = 0; j < kNumOps+1; j++) {
      if (s::pins.IsSet(DigitalPinAlgoExpanderNotConnected)) {
        s::bankParams.modulation[j][i] = s::params.Mod(AlgoSlotLive, j, i);
      } else {
        s::bankParams.modulation[j][i]
          = s::algoLeftGain  * s::params.Mod(s::algo.LeftActiveSlot(),  j, i) +
            s::algoRightGain * s::params.Mod(s::algo.RightActiveSlot(), j, i);
      }
    }
    break;
  }

  case 5:
  {
    s::intAdc.UpdateMuxAddress(s::sampleCount);

    s::bank.SetParameters(&s::bankParams);

    s::pins.HandleChangedPinStates();

#if defined(CVTEST)
    for (uint8_t i = 0; i < kNumCvTestSets) {
      if (i == s::cvTest.CurrentSetIndex()) {
        s::leds.SetDutyCycle(i, 1.0f);
      } else {
        s::leds.SetDutyCycle(i, 0.0f);
      }
    }
#else
    if (s::cal.State() == CalibrationStateIdle) {
      for (uint8_t i = 0; i < kNumOps+1; i++) {
        pfloat_t brightness = s::bankParams.gain[i];
        s::leds.SetDutyCycle(PwmTimerLed1 + i, brightness);
      }

      s::leds.SetDutyCycle(PwmTimerClippingLed,
                           s::params.ClipIndicatorLevel());
    } else {  // calibration in progress
      bool high = blink_high() ? 1.0f : 0.0f;
      for (uint8_t i = 0; i < PwmTimerAlgoLedA; i++) {
        s::leds.SetDutyCycle(i, high);
      }

      s::cal.AdvanceState(s::sampleCount);
    }
#endif

    // effectively a no-op when algo is not attached
    for (uint8_t i = 0; i < NumAlgoSlots; i++) {
      pfloat_t brightness;
      AlgoSlot slot = static_cast<AlgoSlot>(AlgoSlotA + i);

      s::algo.AdvanceState(slot);

      if (s::algo.IsBlinking(slot)) {
        brightness = s::algo.IsBlinkingHighNow(slot) ? 1.0f : 0.0f;
      } else {
        if (slot == s::algo.LeftActiveSlot()
            && slot == s::algo.RightActiveSlot()) {
          brightness = 1.0f;
        } else if (slot == s::algo.LeftActiveSlot()) {
          brightness = 1.0f - s::params.AlgoRightGain();
        } else if (slot == s::algo.RightActiveSlot()) {
          brightness = s::params.AlgoRightGain();
        } else {
          brightness = 0.0f;
        }
      }

      s::leds.SetDutyCycle(PwmTimerAlgoLedA + i, brightness);
    }
  }
  }

  s::bank.RenderNextFrame(extLevel);
  s::sampleCount++;

  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_RESET);
}

}  // namespace quadop
