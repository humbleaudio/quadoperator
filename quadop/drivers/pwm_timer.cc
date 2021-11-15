// Copyright 2017 Humble Audio LLC
//
// MCU pin outputs:
// * PB10 - TIM2_CH3 - op 1 gain LED
// * PB11 - TIM2_CH4 - clipping LED
// * PC6 - TIM3_CH1 - algo slot A LED
// * PC7 - TIM3_CH2 - algo slot B LED
// * PC8 - TIM3_CH3 - algo slot C LED
// * PC9 - TIM3_CH4 - algo slot live LED
// * PD12 - TIM4_CH1 - ext fm gain LED
// * PD13 - TIM4_CH2 - op 4 gain LED
// * PD14 - TIM4_CH3 - op 3 gain LED
// * PD15 - TIM4_CH4 - op 2 gain LED

#include "quadop/drivers/pwm_timer.h"

#include "conf/arch_conf.h"

const float kTimerPeriod = 1024.0f;

namespace quadop {

void PwmTimer::Init(uint32_t timerPeriod) {
  timerPeriod_ = timerPeriod;

  __HAL_RCC_TIM2_CLK_ENABLE();
  __HAL_RCC_TIM3_CLK_ENABLE();
  __HAL_RCC_TIM4_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  {
    tim2Hdl_.Instance = TIM2;

    tim2Hdl_.Init.Prescaler = 0;
    tim2Hdl_.Init.CounterMode = TIM_COUNTERMODE_UP;
    tim2Hdl_.Init.Period = timerPeriod;
    tim2Hdl_.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    tim2Hdl_.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

    if (HAL_TIM_Base_Init(&tim2Hdl_) != HAL_OK) {
      Error_Handler();
    }

    TIM_ClockConfigTypeDef sClockSourceConfig;
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&tim2Hdl_, &sClockSourceConfig) != HAL_OK) {
      Error_Handler();
    }

    if (HAL_TIM_PWM_Init(&tim2Hdl_) != HAL_OK) {
      Error_Handler();
    }

    TIM_MasterConfigTypeDef mstrCfg;
    mstrCfg.MasterOutputTrigger = TIM_TRGO_RESET;
    mstrCfg.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&tim2Hdl_, &mstrCfg) != HAL_OK) {
      Error_Handler();
    }
  }

  {
    tim3Hdl_.Instance = TIM3;

    tim3Hdl_.Init.Prescaler = 0;
    tim3Hdl_.Init.CounterMode = TIM_COUNTERMODE_UP;
    tim3Hdl_.Init.Period = timerPeriod;
    tim3Hdl_.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    tim3Hdl_.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

    if (HAL_TIM_Base_Init(&tim3Hdl_) != HAL_OK) {
      Error_Handler();
    }

    TIM_ClockConfigTypeDef sClockSourceConfig;
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&tim3Hdl_, &sClockSourceConfig) != HAL_OK) {
      Error_Handler();
    }

    if (HAL_TIM_PWM_Init(&tim3Hdl_) != HAL_OK) {
      Error_Handler();
    }

    TIM_MasterConfigTypeDef mstrCfg;
    mstrCfg.MasterOutputTrigger = TIM_TRGO_RESET;
    mstrCfg.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&tim3Hdl_, &mstrCfg) != HAL_OK) {
      Error_Handler();
    }
  }

  {
    tim4Hdl_.Instance = TIM4;

    tim4Hdl_.Init.Prescaler = 0;
    tim4Hdl_.Init.CounterMode = TIM_COUNTERMODE_UP;
    tim4Hdl_.Init.Period = timerPeriod;
    tim4Hdl_.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    tim4Hdl_.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

    if (HAL_TIM_Base_Init(&tim4Hdl_) != HAL_OK) {
      Error_Handler();
    }

    TIM_ClockConfigTypeDef sClockSourceConfig;
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&tim4Hdl_, &sClockSourceConfig) != HAL_OK) {
      Error_Handler();
    }

    if (HAL_TIM_PWM_Init(&tim4Hdl_) != HAL_OK) {
      Error_Handler();
    }

    TIM_MasterConfigTypeDef mstrCfg;
    mstrCfg.MasterOutputTrigger = TIM_TRGO_RESET;
    mstrCfg.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&tim4Hdl_, &mstrCfg) != HAL_OK) {
      Error_Handler();
    }
  }

  /* Common configuration for all channels */
  TIM_OC_InitTypeDef ocCfg;
  ocCfg.OCMode = TIM_OCMODE_PWM1;
  ocCfg.Pulse = 512;
  ocCfg.OCPolarity = TIM_OCPOLARITY_HIGH;
  ocCfg.OCFastMode = TIM_OCFAST_DISABLE;

  // TIM2
  HAL_TIM_PWM_ConfigChannel(&tim2Hdl_, &ocCfg, TIM_CHANNEL_3);
  HAL_TIM_PWM_ConfigChannel(&tim2Hdl_, &ocCfg, TIM_CHANNEL_4);

  // TIM3
  HAL_TIM_PWM_ConfigChannel(&tim3Hdl_, &ocCfg, TIM_CHANNEL_1);
  HAL_TIM_PWM_ConfigChannel(&tim3Hdl_, &ocCfg, TIM_CHANNEL_2);
  HAL_TIM_PWM_ConfigChannel(&tim3Hdl_, &ocCfg, TIM_CHANNEL_3);
  HAL_TIM_PWM_ConfigChannel(&tim3Hdl_, &ocCfg, TIM_CHANNEL_4);

  // TIM4
  HAL_TIM_PWM_ConfigChannel(&tim4Hdl_, &ocCfg, TIM_CHANNEL_1);
  HAL_TIM_PWM_ConfigChannel(&tim4Hdl_, &ocCfg, TIM_CHANNEL_2);
  HAL_TIM_PWM_ConfigChannel(&tim4Hdl_, &ocCfg, TIM_CHANNEL_3);
  HAL_TIM_PWM_ConfigChannel(&tim4Hdl_, &ocCfg, TIM_CHANNEL_4);

  // TIM2 pins
  {
    GPIO_InitTypeDef gpioInit;
    gpioInit.Pin = GPIO_PIN_10 | GPIO_PIN_11;
    gpioInit.Mode = GPIO_MODE_AF_PP;
    gpioInit.Pull = GPIO_NOPULL;
    gpioInit.Speed = GPIO_SPEED_FREQ_LOW;
    gpioInit.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOB, &gpioInit);
  }

  // TIM3 pins
  {
    GPIO_InitTypeDef  gpioInit;
    gpioInit.Pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;
    gpioInit.Mode = GPIO_MODE_AF_PP;
    gpioInit.Pull = GPIO_NOPULL;
    gpioInit.Speed = GPIO_SPEED_FREQ_LOW;
    gpioInit.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(GPIOC, &gpioInit);
  }

  // TIM4 pins
  {
    GPIO_InitTypeDef  gpioInit;
    gpioInit.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    gpioInit.Mode = GPIO_MODE_AF_PP;
    gpioInit.Pull = GPIO_NOPULL;
    gpioInit.Speed = GPIO_SPEED_FREQ_LOW;
    gpioInit.Alternate = GPIO_AF2_TIM4;
    HAL_GPIO_Init(GPIOD, &gpioInit);
  }
}

void PwmTimer::StartTimer() {
  // TIM2
  HAL_TIM_PWM_Start(&tim2Hdl_, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&tim2Hdl_, TIM_CHANNEL_4);

  // TIM3
  HAL_TIM_PWM_Start(&tim3Hdl_, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&tim3Hdl_, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&tim3Hdl_, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&tim3Hdl_, TIM_CHANNEL_4);

  // TIM4
  HAL_TIM_PWM_Start(&tim4Hdl_, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&tim4Hdl_, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&tim4Hdl_, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&tim4Hdl_, TIM_CHANNEL_4);
}

void PwmTimer::SetDutyCycle(const uint8_t channel, const float dutyCycle) {
  assert_param(0 <= channel && channel <= NumPwmTimerChannels);

  uint32_t pulse = dutyCycle * timerPeriod_;
  switch (channel) {
  case PwmTimerLed1: TIM2->CCR3 = pulse; break;
  case PwmTimerLed2: TIM4->CCR4 = pulse; break;
  case PwmTimerLed3: TIM4->CCR3 = pulse; break;
  case PwmTimerLed4: TIM4->CCR2 = pulse; break;
  case PwmTimerLed5: TIM4->CCR1 = pulse; break;
  case PwmTimerClippingLed: TIM2->CCR4 = pulse; break;
  case PwmTimerAlgoLedA: TIM3->CCR1 = pulse; break;
  case PwmTimerAlgoLedB: TIM3->CCR2 = pulse; break;
  case PwmTimerAlgoLedC: TIM3->CCR3 = pulse; break;
  case PwmTimerAlgoLedLive: TIM3->CCR4 = pulse; break;
  }
}

}  // namespace quadop
