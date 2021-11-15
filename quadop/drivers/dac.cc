// Copyright 2017 Will Stockwell
//
// Driver for external AD5754RBREZ DAC

#include "quadop/drivers/dac.h"

#define DAC_MCLK_Pin GPIO_PIN_2
#define DAC_2SCOMP_Pin GPIO_PIN_3
#define DAC_LRCLK_Pin GPIO_PIN_4
#define DAC_BCLK_Pin GPIO_PIN_5
#define DAC_DATA_Pin GPIO_PIN_6
#define DAC_LDAC_Pin GPIO_PIN_0
#define DAC_CLR_Pin GPIO_PIN_1

#define READ    0x80
#define WRITE   0x00
#define DAC_REG   0x00
#define OUT_RANGE 0x08
#define POWER   0x10
#define CTRL    0x18
#define DAC_A   0x00
#define DAC_B   0x01
#define DAC_C   0x02
#define DAC_D   0x03
#define DAC_ALL   0x04

// Output Range Option Masks
#define P_5     0x00
#define P_10    0x01
#define P_10_8    0x02
#define PN_5    0x03
#define PN_10   0x04
#define PN_10_8   0x05

// Control Register Masks
#define SDO_DIS   0x01
#define CLR_SELECT  0x02
#define CLAMP   0x04
#define TSD     0x08

// Control Register addresses
#define BITS_REG  0x01
#define CLR     0x04
#define LOAD    0x05

// Power Control Register Masks
#define PUA     0x01
#define PUB     0x02
#define PUC     0x04
#define PUD     0x08
#define PUREF   0x10
#define TSD_READ  0x20
#define OCA     0x80
#define OCB     0x01
#define OCC     0x02
#define OCD     0x04

namespace quadop {

void Dac::Init() {
  __HAL_RCC_DMA2_CLK_ENABLE();
  __HAL_RCC_SAI1_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();

  // Setup DAC SAI pins
  {
    GPIO_InitTypeDef gpioInit;

    gpioInit.Pin = DAC_MCLK_Pin|DAC_LRCLK_Pin|DAC_BCLK_Pin|DAC_DATA_Pin;
    gpioInit.Mode = GPIO_MODE_AF_PP;
    gpioInit.Pull = GPIO_NOPULL;
    gpioInit.Speed = GPIO_SPEED_FREQ_LOW;
    gpioInit.Alternate = GPIO_AF6_SAI1;
    HAL_GPIO_Init(GPIOE, &gpioInit);
  }

  // Setup non-SAI DAC pins
  {
    HAL_GPIO_WritePin(GPIOE,
                      DAC_2SCOMP_Pin | DAC_LDAC_Pin | DAC_CLR_Pin,
                      GPIO_PIN_RESET);

    GPIO_InitTypeDef gpioInit;
    gpioInit.Pin = DAC_2SCOMP_Pin|DAC_LDAC_Pin|DAC_CLR_Pin;
    gpioInit.Mode = GPIO_MODE_OUTPUT_PP;
    gpioInit.Pull = GPIO_NOPULL;
    gpioInit.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOE, &gpioInit);
  }

  {
    saiHdl_.Instance = SAI1_Block_A;
    saiHdl_.Init.AudioMode = SAI_MODEMASTER_TX;
    saiHdl_.Init.Synchro = SAI_ASYNCHRONOUS;
    saiHdl_.Init.OutputDrive = SAI_OUTPUTDRIVE_DISABLE;
    saiHdl_.Init.NoDivider = SAI_MASTERDIVIDER_ENABLE;
    saiHdl_.Init.FIFOThreshold = SAI_FIFOTHRESHOLD_EMPTY;
    saiHdl_.Init.AudioFrequency = SAI_AUDIO_FREQUENCY_192K;
    saiHdl_.Init.SynchroExt = SAI_SYNCEXT_DISABLE;
    saiHdl_.Init.MonoStereoMode = SAI_STEREOMODE;
    saiHdl_.Init.CompandingMode = SAI_NOCOMPANDING;
    saiHdl_.Init.TriState = SAI_OUTPUT_NOTRELEASED;
    if (HAL_SAI_InitProtocol(&saiHdl_,
                             SAI_I2S_STANDARD,
                             SAI_PROTOCOL_DATASIZE_24BIT, 2)
        != HAL_OK) {
      Error_Handler();
    }
  }

  {
    dmaSaiHdl_.Instance = DMA2_Stream3;
    dmaSaiHdl_.Init.Channel = DMA_CHANNEL_0;
    dmaSaiHdl_.Init.Direction = DMA_MEMORY_TO_PERIPH;
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

    __HAL_LINKDMA(&saiHdl_, hdmarx, dmaSaiHdl_);
    __HAL_LINKDMA(&saiHdl_, hdmatx, dmaSaiHdl_);
  }
}

void Dac::StartConversion() {
  {
    // SAI1_SCK_A should be 12.288MHz It's 4*2*32*48kHz. 4 channels, stereo, 32 bits.
    // It's I2S 24 bit interface standart.
    SAI1_Block_A->CR1 |= SAI_xCR1_NODIV;
    // Changes SAI1A configuration to allow send 24 bits length data
    SAI1_Block_A->FRCR &= 0xFFFB00FF;
    // FSALL = 24. With this modification LRCLK rises after 24 bits and
    // can work as SPI_CS for AD5754
    SAI1_Block_A->FRCR |= 0x00001700;

    dmaBuf_[0] = ((WRITE | OUT_RANGE | DAC_ALL)<<16) | PN_5;
    dmaBuf_[2] = 0xFF0000;
    dmaBuf_[4] = ((WRITE | POWER)<<16) | (PUA | PUB | PUC | PUD | PUREF);
    dmaBuf_[6] = 0xFF0000;

    HAL_SAI_Transmit_DMA(&saiHdl_, (uint8_t *)&dmaBuf_, 8);
  }

  HAL_GPIO_WritePin(GPIOE, DAC_CLR_Pin, GPIO_PIN_SET);
}

void Dac::SetChannels(pfloat_t value1,
  pfloat_t value2,
  pfloat_t value3,
  pfloat_t value4) {

  dmaBuf_[0] = 0x000000 + FormatSample(value1);
  dmaBuf_[2] = 0x010000 + FormatSample(value2);
  dmaBuf_[4] = 0x020000 + FormatSample(value3);
  dmaBuf_[6] = 0x030000 + FormatSample(value4);
}

int32_t Dac::FormatSample(pfloat_t value) {
  float sampleFloat = static_cast<float>(value) * 0x7fffff00;
  return (static_cast<int32_t>(sampleFloat)>>8 &~(~0 << 24)) >> 8;
}

}  // namespace quadop
