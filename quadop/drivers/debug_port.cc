// Copyright 2017 Will Stockwell
//
// MCU pins:
// PB6 - USART1 TX
// PB7 - USART1 RX

#include "quadop/drivers/debug_port.h"

#include <stm32f7xx_conf.h>

namespace quadop {

void DebugPort::Init() {
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_USART1_CLK_ENABLE();

  {
    GPIO_InitTypeDef gpio_init;

    gpio_init.Mode      = GPIO_MODE_AF_PP;
    gpio_init.Pull      = GPIO_PULLUP;
    gpio_init.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;

    gpio_init.Pin       = GPIO_PIN_6;
    gpio_init.Alternate = GPIO_AF7_USART1;

    HAL_GPIO_Init(GPIOB, &gpio_init);

    gpio_init.Pin = GPIO_PIN_7;
    gpio_init.Alternate = GPIO_AF7_USART1;

    HAL_GPIO_Init(GPIOB, &gpio_init);
  }

  {
    UART_HandleTypeDef uartHdl;

    uartHdl.Instance        = USART1;

    uartHdl.Init.BaudRate   = 115200;
    uartHdl.Init.WordLength = UART_WORDLENGTH_8B;
    uartHdl.Init.StopBits   = UART_STOPBITS_1;
    uartHdl.Init.Parity     = UART_PARITY_NONE;
    uartHdl.Init.Mode       = UART_MODE_TX_RX;
    uartHdl.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
    uartHdl.Init.OverSampling = UART_OVERSAMPLING_16;
    uartHdl.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    uartHdl.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

    if (HAL_UART_DeInit(&uartHdl) != HAL_OK) {
      while (1) {}
    }

    if (HAL_UART_Init(&uartHdl) != HAL_OK) {
      while (1) {}
    }
  }
}

bool DebugPort::Writable() {
  return USART1->ISR & USART_FLAG_TXE;
}

bool DebugPort::Readable() {
  return USART1->ISR & USART_FLAG_RXNE;
}

void DebugPort::Overwrite(uint8_t byte) {
  USART1->TDR = byte;
}

uint8_t DebugPort::ImmediateRead() {
  return USART1->RDR;
}

}  // namespace quadop
