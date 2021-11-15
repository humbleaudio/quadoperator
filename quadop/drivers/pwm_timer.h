// Copyright 2017 Humble Audio LLC

#ifndef QUADOP_DRIVERS_PWM_TIMER_H_
#define QUADOP_DRIVERS_PWM_TIMER_H_

#include <stm32f7xx_conf.h>

#include "stmlib/stmlib.h"

namespace quadop {

typedef enum {
  PwmTimerLed1 = 0,
  PwmTimerLed2,
  PwmTimerLed3,
  PwmTimerLed4,
  PwmTimerLed5,
  PwmTimerClippingLed,
  PwmTimerAlgoLedA,
  PwmTimerAlgoLedB,
  PwmTimerAlgoLedC,
  PwmTimerAlgoLedLive,
  NumPwmTimerChannels
} PwmTimerChannel;

class PwmTimer {
 public:
  PwmTimer() { }
  ~PwmTimer() { }

  void Init(uint32_t timerPeriod);
  void StartTimer();

  void SetDutyCycle(const uint8_t channel, const float dutyCycle);

 private:
  float timerPeriod_;
  TIM_HandleTypeDef tim2Hdl_;
  TIM_HandleTypeDef tim3Hdl_;
  TIM_HandleTypeDef tim4Hdl_;
  TIM_OC_InitTypeDef commOcCfg_;

  DISALLOW_COPY_AND_ASSIGN(PwmTimer);
};

}  // namespace quadop

#endif  // QUADOP_DRIVERS_PWM_TIMER_H_
