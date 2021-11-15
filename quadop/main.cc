// Copyright 2017 Humble Audio LLC

#include <stdarg.h>
#include <stdio.h>
#include <stm32f7xx_hal.h>

#include "debug/debug.h"
#include "lib/math/math.h"
#include "quadop/dsp/operator_bank.h"
#include "quadop/parameters.h"
#include "quadop/state.h"

typedef quadop::State s;

const pfloat_t kSampleRate = 48000.0f;
const pfloat_t kTimerPeriod = 4095.0f;

#ifdef EVAL
uint16_t debugCount = 0;
#endif

// workaround missing errno reference in math functions
// true errno not needed for now
int __errno;

extern "C" {

void HardFault_Handler(void) { while (1); }
void MemManage_Handler(void) { while (1); }
void BusFault_Handler(void) { while (1); }
void UsageFault_Handler(void) { while (1); }
void NMI_Handler(void) { }
void SVC_Handler(void) { }
void DebugMon_Handler(void) { }
void PendSV_Handler(void) { }
void SysTick_Handler(void) { }

void Error_Handler(void) {
  // TODO(will): signal error to user somehow?
  assert_param(1 == 0);
}

}

void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 256;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  if (HAL_PWREx_EnableOverDrive() != HAL_OK) {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6) != HAL_OK) {
    Error_Handler();
  }

  PeriphClkInitStruct.PeriphClockSelection
    = RCC_PERIPHCLK_USART1
      | RCC_PERIPHCLK_SAI1
      | RCC_PERIPHCLK_SAI2
      | RCC_PERIPHCLK_I2C2
      | RCC_PERIPHCLK_I2C4;
  PeriphClkInitStruct.PLLSAI.PLLSAIN = 128;
  PeriphClkInitStruct.PLLSAI.PLLSAIR = 2;
  PeriphClkInitStruct.PLLSAI.PLLSAIQ = 8;
  PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV2;
  PeriphClkInitStruct.PLLSAIDivQ = 2;
  PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_2;
  PeriphClkInitStruct.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLLSAI;
  PeriphClkInitStruct.Sai2ClockSelection = RCC_SAI2CLKSOURCE_PLLSAI;
  PeriphClkInitStruct.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInitStruct.I2c2ClockSelection = RCC_I2C2CLKSOURCE_PCLK1;
  PeriphClkInitStruct.I2c4ClockSelection = RCC_I2C4CLKSOURCE_PCLK1;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
    Error_Handler();
  }

  HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_PLLCLK, RCC_MCODIV_4);

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_2);
}

void Init() {
  SystemInit();

  SCB_EnableICache();
  SCB_EnableDCache();

  HAL_Init();

  SystemClock_Config();

  // Initialize drivers
  {
    s::settings.Init();
    s::leds.Init(kTimerPeriod);
    s::bank.Init(kSampleRate);
    s::pins.Init();
    s::extAdc.Init();
    s::intAdc.Init();
    s::cal.Init(kSampleRate, &s::intAdc, &s::extAdc, &s::dac, &s::settings);
    s::cvTest.Init();
    s::dac.Init();
    s::scheduler.Init(kSampleRate);
    s::params.Init(&s::intAdc, &s::extAdc, &s::settings);

    // TODO(will): check pin state properly when
    // hardware shortcoming is resolved
    s::algoAttached = true;

    s::algo.Init(&s::params, &s::settings);
  }
}

int main(void) {
  Init();

  // Set operator bank parameters
  {
    quadop::OperatorBankParameters params;

    params.gain[quadop::kExtId] = 0.0f;

    for (uint8_t i = 0; i < quadop::kNumOps; i++) {
      params.gain[i] = 1.0f;
      params.variable[i] = 0.0f;
      params.frequency[i] = 1.0f;

      for (uint8_t j = 0; j < quadop::kNumOps+1; j++) {
        params.modulation[j][i] = 0.0f;
      }
    }

    s::bank.SetParameters(&params);
  }

  s::leds.StartTimer();
  s::extAdc.StartConversion();
  s::dac.StartConversion();
  s::intAdc.StartConversion();

  while (1) {}  // All the runtime work is done via SchedulerBinder::Run
}
