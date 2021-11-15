// Copyright 2017 Humble Audio LLC
//
// Driver for external PCM1802 ADC
//
// MCU pin inputs:
// PE7 - OSR
// PE8 - NPDWN
// PE9 - BYPAS
// PE11 - DOUT
// PE12 - BCK
// PE13 - LRCK
// PE14 - SCKI
// PE15 - FSYNC
// PG0 - FMT1
// PG1 - FMT0

// TODO(will): address the fact that this class is intrinsically dependent
// on InternalAdc initialization.
// - Combine the classes?
// - Abstract DMA controller?

#include "quadop/drivers/external_adc.h"

#include <stm32f7xx_conf.h>

#include "debug/debug.h"

namespace quadop {

void ExternalAdc::Init() {
  __HAL_RCC_DMA2_CLK_ENABLE();
  __HAL_RCC_SAI2_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  {  // Set pin states as desired
    // Set PCM1802 OSR & NPDWN pins = 0
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7 | GPIO_PIN_8, GPIO_PIN_RESET);

    // Set PCM1802 BYPAS & FSYNC pins = 1
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_9 | GPIO_PIN_15, GPIO_PIN_SET);

    // SET PCM1802 FMT1 & FMT0 pins for desired configuration
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_0, GPIO_PIN_RESET);  // FMT1
    HAL_GPIO_WritePin(GPIOG, GPIO_PIN_1, GPIO_PIN_SET);  // FMT0
  }

  {  // FMT1, FMT0
    GPIO_InitTypeDef gpio_init;

    gpio_init.Pin = GPIO_PIN_0 | GPIO_PIN_1;
    gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init.Pull = GPIO_NOPULL;
    gpio_init.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(GPIOG, &gpio_init);
  }

  {  // OSR, NPDWN, BYPAS, FSYNC
    GPIO_InitTypeDef gpio_init;

    gpio_init.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_15;
    gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init.Pull = GPIO_NOPULL;
    gpio_init.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(GPIOE, &gpio_init);
  }

  {  // DOUT, BCK, LRCK, SCKI
    GPIO_InitTypeDef gpio_init;

    gpio_init.Pin = GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14;
    gpio_init.Mode = GPIO_MODE_AF_PP;
    gpio_init.Pull = GPIO_NOPULL;
    gpio_init.Speed = GPIO_SPEED_FREQ_LOW;
    gpio_init.Alternate = GPIO_AF10_SAI2;

    HAL_GPIO_Init(GPIOE, &gpio_init);
  }

  {
    saiHdl_.Instance = SAI2_Block_B;
    saiHdl_.Init.AudioMode = SAI_MODEMASTER_RX;
    saiHdl_.Init.Synchro = SAI_ASYNCHRONOUS;
    saiHdl_.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
    saiHdl_.Init.NoDivider = SAI_MASTERDIVIDER_ENABLE;
    saiHdl_.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_EMPTY;
    saiHdl_.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_96K;
    saiHdl_.Init.SynchroExt = SAI_SYNCEXT_DISABLE;
    saiHdl_.Init.MonoStereoMode = SAI_STEREOMODE;
    saiHdl_.Init.CompandingMode = SAI_NOCOMPANDING;

    if (HAL_SAI_InitProtocol(&saiHdl_,
                             SAI_I2S_STANDARD,
                             SAI_PROTOCOL_DATASIZE_24BIT, 2)
        != HAL_OK) {
      Error_Handler();
    }
  }

  {
    dmaSaiHdl_.Instance = DMA2_Stream1;
    dmaSaiHdl_.Init.Channel = DMA_CHANNEL_10;
    dmaSaiHdl_.Init.Direction = DMA_PERIPH_TO_MEMORY;
    dmaSaiHdl_.Init.PeriphInc = DMA_PINC_DISABLE;
    dmaSaiHdl_.Init.MemInc = DMA_MINC_ENABLE;
    dmaSaiHdl_.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    dmaSaiHdl_.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    dmaSaiHdl_.Init.Mode = DMA_CIRCULAR;
    dmaSaiHdl_.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    dmaSaiHdl_.Init.FIFOMode = DMA_FIFOMODE_DISABLE;

    if (HAL_DMA_Init(&dmaSaiHdl_) != HAL_OK) {
      Error_Handler();
    }
  }

  __HAL_LINKDMA(&saiHdl_, hdmarx, dmaSaiHdl_);
  __HAL_LINKDMA(&saiHdl_, hdmatx, dmaSaiHdl_);
}

void ExternalAdc::StartConversion() {
  HAL_DMA_Start_IT(&dmaSaiHdl_,
                   (uint32_t)&saiHdl_.Instance->DR,
                   (uint32_t)&dmaBuf_,
                   2);
  HAL_SAI_DMAResume(&saiHdl_);

  // Raise NPDWN
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_SET);
}

pfloat_t ExternalAdc::Value() {
  int32_t sample = ((int32_t) (dmaBuf_[0] << 8));
  // TOOD(will): eliminate this divide by constant
  return (static_cast<float>(sample) / 0x7fffff00);
}

void ExternalAdc::DmaHandler() {
    HAL_DMA_IRQHandler(&dmaSaiHdl_);
}

}  // namespace quadop
