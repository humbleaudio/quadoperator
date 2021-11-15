// Copyright 2017 Will Stockwell

#include "quadop/drivers/scheduler.h"

#include <stm32f7xx_conf.h>

#include "conf/arch_conf.h"

#include "quadop/state.h"

extern "C" {

// TODO(will): this change came late, and breaks the abstraction barrier
// between Scheduler and ExternalAdc (we're essentially now relying on SAI/DMA
// clock to trigger the main synthesis logic driven by Scheduler, which used to
// be driven by it's own hardware timer. Should think about ways to re-establish
// and better division of responsibilities, but for now it's not spiraling
// beyond this. So, I think it's an acceptable leak.
void DMA2_Stream1_IRQHandler(void) {
  quadop::SchedulerBinder::Run();
  quadop::State::extAdc.DmaHandler();
}

}

namespace quadop {

void Scheduler::Init(uint32_t freq) {
  freq_ = freq;

  __HAL_RCC_GPIOC_CLK_ENABLE();

  {
    GPIO_InitTypeDef gpio_init;
    gpio_init.Pin = GPIO_PIN_10 | GPIO_PIN_11;
    gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init.Pull = GPIO_NOPULL;
    gpio_init.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &gpio_init);

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10 | GPIO_PIN_11, GPIO_PIN_RESET);
  }

  HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);
}


uint32_t Scheduler::Freq() {
  return freq_;
}

}  // namespace quadop
