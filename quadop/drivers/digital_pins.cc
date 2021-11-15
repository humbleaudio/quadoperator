// Copyright 2017 Humble Audio LLC
//
// MCU pin inputs:
// PB12 - Lock switch operator 1
// PB13 - Lock switch operator 2
// PB14 - Lock switch operator 3
// PB15 - Lock switch operator 4
// PD8  - VCO/LFO switch
// PG2  - Algo slot A
// PG3  - Algo slot B
// PG4  - Algo slot C
// PG5  - Algo slot live
// PG10  - Algo expander detection

#include "quadop/drivers/digital_pins.h"

#include <stm32f7xx_conf.h>

namespace quadop {

void DigitalPins::Init() {
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  {
    GPIO_InitTypeDef gpio_init;
    gpio_init.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_init.Mode = GPIO_MODE_INPUT;
    gpio_init.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &gpio_init);
  }

  {
    GPIO_InitTypeDef gpio_init;
    gpio_init.Pin = GPIO_PIN_8;
    gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_init.Mode = GPIO_MODE_INPUT;
    gpio_init.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOD, &gpio_init);
  }

  {
    GPIO_InitTypeDef gpio_init;
    gpio_init.Pin = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_10;
    gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_init.Mode = GPIO_MODE_INPUT;
    gpio_init.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOG, &gpio_init);
  }

  for (uint8_t pin = DigitalPinLockSwitch1; pin < NumDigitalPins; pin++) {
    pinState_[pin][0] = pinState_[pin][1] = ReadPin(pin);
  }
}

void DigitalPins::HandleChangedPinStates() {
  currentPinIndex_ = (currentPinIndex_ + 1) % 2;

  for (uint8_t pin = DigitalPinLockSwitch1; pin < NumDigitalPins; pin++) {
    bool newState = ReadPin(pin);

    pinState_[pin][currentPinIndex_] = newState;
    if (pinState_[pin][0] != pinState_[pin][1]) {
      DigitalPinsBinder::HandlePin(static_cast<DigitalPin>(pin), newState);
    }
  }
}

bool DigitalPins::ReadPin(const uint8_t pin) {
  GPIO_TypeDef  *gpio;
  uint16_t gpio_pin;

  switch (pin) {
  case DigitalPinLockSwitch1:
    gpio = GPIOB;
    gpio_pin = GPIO_PIN_12;
    break;

  case DigitalPinLockSwitch2:
    gpio = GPIOB;
    gpio_pin = GPIO_PIN_13;
    break;

  case DigitalPinLockSwitch3:
    gpio = GPIOB;
    gpio_pin = GPIO_PIN_14;
    break;

  case DigitalPinLockSwitch4:
    gpio = GPIOB;
    gpio_pin = GPIO_PIN_15;
    break;

  case DigitalPinVcoLfoSwitch:
    gpio = GPIOD;
    gpio_pin = GPIO_PIN_8;
    break;

  case DigitalPinAlgoSlotA:
    gpio = GPIOG;
    gpio_pin = GPIO_PIN_2;
    break;

  case DigitalPinAlgoSlotB:
    gpio = GPIOG;
    gpio_pin = GPIO_PIN_3;
    break;

  case DigitalPinAlgoSlotC:
    gpio = GPIOG;
    gpio_pin = GPIO_PIN_4;
    break;

  case DigitalPinAlgoSlotLive:
    gpio = GPIOG;
    gpio_pin = GPIO_PIN_5;
    break;

  case DigitalPinAlgoExpanderNotConnected:
    gpio = GPIOG;
    gpio_pin = GPIO_PIN_10;
    break;
  }

  return HAL_GPIO_ReadPin(gpio, gpio_pin) == GPIO_PIN_SET;
}

bool DigitalPins::IsSet(const uint8_t pin) {
  return pinState_[pin][currentPinIndex_];
}

}  // namespace quadop
