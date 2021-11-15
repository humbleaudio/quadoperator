// Copyright 2017 Humble Audio LLC
//
// Driver for internal ADC
//
// MCU pin inputs:
//
// PA0 - ADC1 IN 0  - gain cv 4
// PA1 - ADC1 IN 1  - gain cv 3
// PA2 - ADC1 IN 2  - shape cv 3
// PA3 - ADC1 IN 3  - shape cv 4
// PA4 - ADC1 IN 4  - ratio cv 2
// PA5 - ADC1 IN 5  - ratio cv 1
// PA6 - ADC1 IN 6  - ratio cv 3
// PA7 - ADC2 IN 7  - ratio cv 4
// PB0 - ADC2 IN 8  - coarse
// PB1 - ADC2 IN 9  - fine
// PC0 - ADC2 IN 10 - gain cv 2
// PC1 - ADC2 IN 11 - gain cv 1
// PC2 - ADC2 IN 12 - shape cv 1
// PC3 - ADC1 IN 13 - shape cv 2
// PC4 - ADC2 IN 14 - Algo crossfade cv
// PC5 - ADC2 IN 15 - Mux 1 ADC
// PF3 - ADC3 IN 9  - gain cv ext
// PF4 - ADC3 IN 14 - v/o cv
// PF5 - ADC3 IN 15 - reset cv
// PF6 - ADC3 IN 4  - LF FM cv
// PF7 - ADC3 IN 5  - Mux 2 ADC
// PF8 - ADC3 IN 6  - Mux 3 ADC
// PF9 - ADC3 IN 7  - Mux 4 ADC
// PF10 - ADC3 IN 8 - Algo crossfade knob

// Mux address pins:
//
// PD0 - S0
// PD1 - S1
// PD2 - S2
// PD3 - Mux 1 Enabled
// PD4 - Mux 2 Enabled
// PD5 - Mux 3 Enabled
// PD6 - Mux 4 Enabled
//
// Mux Knob Addresses:
//
// Ratio 1   - Mux 1 X 0
// Ratio 2   - Mux 1 X 1
// Ratio 3   - Mux 1 X 2
// Ratio 4   - Mux 1 X 3
// Detune 1  - Mux 1 X 4
// Detune 2  - Mux 1 X 5
// Detune 3  - Mux 1 X 6
// Detune 4  - Mux 1 X 7
// Shape 1   - Mux 2 X 0
// Shape 2   - Mux 2 X 1
// Shape 3   - Mux 2 X 2
// Shape 4   - Mux 2 X 3
// Ext Md 1  - Mux 2 X 4
// Ext Md 2  - Mux 2 X 5
// Ext Md 3  - Mux 2 X 6
// Ext Md 4  - Mux 2 X 7
// Sr 1 Md 1 - Mux 3 X 0
// Sr 2 Md 1 - Mux 3 X 1
// Sr 3 Md 1 - Mux 3 X 2
// Sr 4 Md 1 - Mux 3 X 3
// Sr 1 Md 2 - Mux 3 X 4
// Sr 2 Md 2 - Mux 3 X 5
// Sr 3 Md 2 - Mux 3 X 6
// Sr 4 Md 2 - Mux 3 X 7
// Sr 1 Md 3 - Mux 4 X 0
// Sr 2 Md 3 - Mux 4 X 1
// Sr 3 Md 3 - Mux 4 X 2
// Sr 4 Md 3 - Mux 4 X 3
// Sr 1 Md 4 - Mux 4 X 4
// Sr 2 Md 4 - Mux 4 X 5
// Sr 3 Md 4 - Mux 4 X 6
// Sr 4 Md 4 - Mux 4 X 7

#include "quadop/drivers/internal_adc.h"

#include <stm32f7xx_conf.h>
#include <stm32f7xx_ll_dma.h>

#include "debug/debug.h"
#include "quadop/state.h"

typedef quadop::State s;

typedef enum {
  AdcChannelTypeNormal,  // non-muxed knob
  AdcChannelTypeMuxed,  // muxed knob
  AdcChannelTypeCv  // CV
} ChannelType;

const uint8_t channelIndex[52][3] = {
  // Key: {signal type, ADC index, ADC multiplexing rank index}
  {AdcChannelTypeCv, 2, 2},  // AdcChannelVPerO,
  {AdcChannelTypeCv, 2, 3},  // AdcChannelReset,
  {AdcChannelTypeCv, 2, 4},  // AdcChannelVibrato,
  {AdcChannelTypeNormal, 1, 1},  // AdcChannelCoarse,
  {AdcChannelTypeNormal, 1, 2},  // AdcChannelFine,
  {AdcChannelTypeMuxed, 0, 0},  // AdcChannelRatioKnob1,
  {AdcChannelTypeMuxed, 0, 1},  // AdcChannelRatioKnob2,
  {AdcChannelTypeMuxed, 0, 2},  // AdcChannelRatioKnob3,
  {AdcChannelTypeMuxed, 0, 3},  // AdcChannelRatioKnob4,
  {AdcChannelTypeMuxed, 0, 4},  // AdcChannelDetuneKnob1,
  {AdcChannelTypeMuxed, 0, 5},  // AdcChannelDetuneKnob2,
  {AdcChannelTypeMuxed, 0, 6},  // AdcChannelDetuneKnob3,
  {AdcChannelTypeMuxed, 0, 7},  // AdcChannelDetuneKnob4,
  {AdcChannelTypeMuxed, 1, 0},  // AdcChannelShapeKnob1,
  {AdcChannelTypeMuxed, 1, 1},  // AdcChannelShapeKnob2,
  {AdcChannelTypeMuxed, 1, 2},  // AdcChannelShapeKnob3,
  {AdcChannelTypeMuxed, 1, 3},  // AdcChannelShapeKnob4,
  {AdcChannelTypeCv, 0, 5},  // AdcChannelRatioCv1,
  {AdcChannelTypeCv, 0, 4},  // AdcChannelRatioCv2,
  {AdcChannelTypeCv, 0, 6},  // AdcChannelRatioCv3,
  {AdcChannelTypeCv, 1, 0},  // AdcChannelRatioCv4,
  {AdcChannelTypeCv, 1, 5},  // AdcChannelShapeCv1,
  {AdcChannelTypeCv, 0, 7},  // AdcChannelShapeCv2,
  {AdcChannelTypeCv, 0, 2},  // AdcChannelShapeCv3,
  {AdcChannelTypeCv, 0, 3},  // AdcChannelShapeCv4,
  {AdcChannelTypeCv, 1, 4},  // AdcChannelGainCv1,
  {AdcChannelTypeCv, 1, 3},  // AdcChannelGainCv2,
  {AdcChannelTypeCv, 0, 1},  // AdcChannelGainCv3,
  {AdcChannelTypeCv, 0, 0},  // AdcChannelGainCv4,
  {AdcChannelTypeCv, 2, 1},  // AdcChannelGainCvExt,
  {AdcChannelTypeMuxed, 2, 0},  // AdcChannelModSrc1Des1,
  {AdcChannelTypeMuxed, 2, 4},  // AdcChannelModSrc1Des2,
  {AdcChannelTypeMuxed, 3, 0},  // AdcChannelModSrc1Des3,
  {AdcChannelTypeMuxed, 3, 4},  // AdcChannelModSrc1Des4,
  {AdcChannelTypeMuxed, 2, 1},  // AdcChannelModSrc2Des1,
  {AdcChannelTypeMuxed, 2, 5},  // AdcChannelModSrc2Des2,
  {AdcChannelTypeMuxed, 3, 1},  // AdcChannelModSrc2Des3,
  {AdcChannelTypeMuxed, 3, 5},  // AdcChannelModSrc2Des4,
  {AdcChannelTypeMuxed, 2, 2},  // AdcChannelModSrc3Des1,
  {AdcChannelTypeMuxed, 2, 6},  // AdcChannelModSrc3Des2,
  {AdcChannelTypeMuxed, 3, 2},  // AdcChannelModSrc3Des3,
  {AdcChannelTypeMuxed, 3, 6},  // AdcChannelModSrc3Des4,
  {AdcChannelTypeMuxed, 2, 3},  // AdcChannelModSrc4Des1,
  {AdcChannelTypeMuxed, 2, 7},  // AdcChannelModSrc4Des2,
  {AdcChannelTypeMuxed, 3, 3},  // AdcChannelModSrc4Des3,
  {AdcChannelTypeMuxed, 3, 7},  // AdcChannelModSrc4Des4,
  {AdcChannelTypeMuxed, 1, 4},  // AdcChannelModSrcExtDes1,
  {AdcChannelTypeMuxed, 1, 5},  // AdcChannelModSrcExtDes2,
  {AdcChannelTypeMuxed, 1, 6},  // AdcChannelModSrcExtDes3,
  {AdcChannelTypeMuxed, 1, 7},  // AdcChannelModSrcExtDes4,
  {AdcChannelTypeNormal, 2, 0},  // AdcChannelAlgoCrossfadeKnob,
  {AdcChannelTypeCv, 1, 7},  // AdcChannelAlgoCrossfadeCv,
};

const uint8_t modIndex[5][4][2] = {
  {{2, 0}, {2, 4}, {3, 0}, {3, 4}},
  {{2, 1}, {2, 5}, {3, 1}, {3, 5}},
  {{2, 2}, {2, 6}, {3, 2}, {3, 6}},
  {{2, 3}, {2, 7}, {3, 3}, {3, 7}},
  {{1, 4}, {1, 5}, {1, 6}, {1, 7}}
};

extern "C" {

void DMA2_Stream0_IRQHandler(void) {
  s::intAdc.Dma2Stream0Handler();
}

}

namespace quadop {

void InternalAdc::InitAdc(ADC_TypeDef * adc,
  ADC_HandleTypeDef * adcHdl,
  uint32_t numConv) {
  adcHdl->Instance = adc;
  adcHdl->Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV8;
  adcHdl->Init.Resolution = ADC_RESOLUTION_12B;
  adcHdl->Init.ScanConvMode = (numConv == 1 ? DISABLE : ENABLE);
  adcHdl->Init.ContinuousConvMode = ENABLE;
  adcHdl->Init.DiscontinuousConvMode = DISABLE;
  adcHdl->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  adcHdl->Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC1;
  adcHdl->Init.DataAlign = ADC_DATAALIGN_RIGHT;
  adcHdl->Init.NbrOfConversion = numConv;
  adcHdl->Init.NbrOfDiscConversion = 0;
  adcHdl->Init.DMAContinuousRequests = (adc == ADC1 ? ENABLE : DISABLE);
  adcHdl->Init.EOCSelection = DISABLE;

  if (HAL_ADC_Init(adcHdl) != HAL_OK) {
    Error_Handler();
  }
}

void InternalAdc::InitChannel(ADC_HandleTypeDef * adcHdl,
  uint32_t chan,
  uint32_t rank) {
  ADC_ChannelConfTypeDef sConfig;

  sConfig.Channel      = chan;
  sConfig.Rank         = rank;
  sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
  sConfig.Offset       = 0;

  if (HAL_ADC_ConfigChannel(adcHdl, &sConfig) != HAL_OK) {
    Error_Handler();
  }
}

void InternalAdc::InitAdcDma(DMA_HandleTypeDef * dmaHdl,
  ADC_HandleTypeDef * adcHdl,
  DMA_Stream_TypeDef * dmaStream,
  uint32_t channel) {
  dmaHdl->Instance = dmaStream;
  dmaHdl->Init.Channel = channel;
  dmaHdl->Init.Direction = DMA_PERIPH_TO_MEMORY;
  dmaHdl->Init.PeriphInc = DMA_PINC_DISABLE;
  dmaHdl->Init.MemInc = DMA_MINC_ENABLE;
  dmaHdl->Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  dmaHdl->Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
  dmaHdl->Init.Mode = DMA_CIRCULAR;
  dmaHdl->Init.Priority = DMA_PRIORITY_LOW;
  dmaHdl->Init.FIFOMode = DMA_FIFOMODE_DISABLE;
  dmaHdl->Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
  dmaHdl->Init.MemBurst = DMA_MBURST_SINGLE;
  dmaHdl->Init.PeriphBurst = DMA_PBURST_SINGLE;

  if (HAL_DMA_Init(dmaHdl) != HAL_OK) {
    Error_Handler();
  }

  __HAL_LINKDMA(adcHdl, DMA_Handle, *dmaHdl);
}

void InternalAdc::Init() {
  memset((void *)values_, 0, sizeof(values_));  // NOLINT(readability/casting)
  memset((void *)muxValues_,  // NOLINT(readability/casting)
         0,
         sizeof(muxValues_));

  muxUpdateTimestamp_ = 0;
  muxCount_ = 0;

  __HAL_RCC_ADC1_CLK_ENABLE();
  __HAL_RCC_ADC2_CLK_ENABLE();
  __HAL_RCC_ADC3_CLK_ENABLE();
  __HAL_RCC_DMA2_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();

  {
    HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
  }

  {
    GPIO_InitTypeDef gpio_init;
    gpio_init.Pin = GPIO_PIN_0
                    | GPIO_PIN_1
                    | GPIO_PIN_2
                    | GPIO_PIN_3
                    | GPIO_PIN_4
                    | GPIO_PIN_5
                    | GPIO_PIN_6
                    | GPIO_PIN_7;
    gpio_init.Mode = GPIO_MODE_ANALOG;
    gpio_init.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &gpio_init);

    gpio_init.Pin = GPIO_PIN_0 | GPIO_PIN_1;
    gpio_init.Mode = GPIO_MODE_ANALOG;
    gpio_init.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &gpio_init);

    gpio_init.Pin = GPIO_PIN_0
                    | GPIO_PIN_1
                    | GPIO_PIN_2
                    | GPIO_PIN_3
                    | GPIO_PIN_4
                    | GPIO_PIN_5;
    gpio_init.Mode = GPIO_MODE_ANALOG;
    gpio_init.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &gpio_init);

    gpio_init.Pin = GPIO_PIN_3
                    | GPIO_PIN_4
                    | GPIO_PIN_5
                    | GPIO_PIN_6
                    | GPIO_PIN_7
                    | GPIO_PIN_8
                    | GPIO_PIN_9
                    | GPIO_PIN_10;
    gpio_init.Mode = GPIO_MODE_ANALOG;
    gpio_init.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOF, &gpio_init);
  }

  // Mux address/enable lines output setup
  {
    GPIO_InitTypeDef gpio_init;
    gpio_init.Pin = GPIO_PIN_0
                    | GPIO_PIN_1
                    | GPIO_PIN_2
                    | GPIO_PIN_3
                    | GPIO_PIN_4
                    | GPIO_PIN_5
                    | GPIO_PIN_6;
    gpio_init.Pull = GPIO_NOPULL;
    gpio_init.Speed = GPIO_SPEED_FREQ_LOW;
    gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(GPIOD, &gpio_init);

    // Leave all the mux chips enabled permanently
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, GPIO_PIN_RESET);
  }

  {
    InitAdc(ADC3, &adc3Hdl_, kNumRanks);

    InitAdcDma(&dmaAdc3Hdl_, &adc3Hdl_, DMA2_Stream0, DMA_CHANNEL_0);

    // ADC3 channels
    InitChannel(&adc3Hdl_, ADC_CHANNEL_8, 1);
    InitChannel(&adc3Hdl_, ADC_CHANNEL_9,  2);
    InitChannel(&adc3Hdl_, ADC_CHANNEL_14, 3);
    InitChannel(&adc3Hdl_, ADC_CHANNEL_15, 4);
    InitChannel(&adc3Hdl_, ADC_CHANNEL_4,  5);
    InitChannel(&adc3Hdl_, ADC_CHANNEL_5,  6);
    InitChannel(&adc3Hdl_, ADC_CHANNEL_6,  7);
    InitChannel(&adc3Hdl_, ADC_CHANNEL_7,  8);
  }

  {
    InitAdc(ADC2, &adc2Hdl_, kNumRanks);

    InitAdcDma(&dmaAdc2Hdl_, &adc2Hdl_, DMA2_Stream0, DMA_CHANNEL_0);

    // ADC2 channels
    InitChannel(&adc2Hdl_, ADC_CHANNEL_7, 1);
    InitChannel(&adc2Hdl_, ADC_CHANNEL_8, 2);
    InitChannel(&adc2Hdl_, ADC_CHANNEL_9, 3);
    InitChannel(&adc2Hdl_, ADC_CHANNEL_10, 4);
    InitChannel(&adc2Hdl_, ADC_CHANNEL_11, 5);
    InitChannel(&adc2Hdl_, ADC_CHANNEL_12, 6);
    InitChannel(&adc2Hdl_, ADC_CHANNEL_15, 7);
    InitChannel(&adc2Hdl_, ADC_CHANNEL_14, 8);
  }

  {
    // ADC1 is master for multimode, so we do it last
    InitAdc(ADC1, &adc1Hdl_, kNumRanks);

    InitAdcDma(&dmaAdc1Hdl_, &adc1Hdl_, DMA2_Stream0, DMA_CHANNEL_0);

    // ADC1 channels
    InitChannel(&adc1Hdl_, ADC_CHANNEL_0, 1);
    InitChannel(&adc1Hdl_, ADC_CHANNEL_1, 2);
    InitChannel(&adc1Hdl_, ADC_CHANNEL_2, 3);
    InitChannel(&adc1Hdl_, ADC_CHANNEL_3, 4);
    InitChannel(&adc1Hdl_, ADC_CHANNEL_4, 5);
    InitChannel(&adc1Hdl_, ADC_CHANNEL_5, 6);
    InitChannel(&adc1Hdl_, ADC_CHANNEL_6, 7);
    InitChannel(&adc1Hdl_, ADC_CHANNEL_13, 8);
  }

  {
    ADC_MultiModeTypeDef mode;
    mode.Mode = ADC_TRIPLEMODE_REGSIMULT;
    mode.DMAAccessMode = ADC_DMAACCESSMODE_1;
    mode.TwoSamplingDelay = ADC_TWOSAMPLINGDELAY_5CYCLES;

    if (HAL_ADCEx_MultiModeConfigChannel(&adc1Hdl_, &mode) != HAL_OK) {
      Error_Handler();
    }
  }
}

void InternalAdc::StartConversion() {
  if (HAL_ADC_Start(&adc3Hdl_) != HAL_OK) {
    Error_Handler();
  }

  if (HAL_ADC_Start(&adc2Hdl_) != HAL_OK) {
    Error_Handler();
  }

  if (HAL_ADCEx_MultiModeStart_DMA(&adc1Hdl_,
                                   (uint32_t *)values_,  // NOLINT
                                   kNumAdcs * kNumRanks) != HAL_OK) {
    Error_Handler();
  }
}

pfloat_t InternalAdc::AdcValueToUnit(const uint32_t adcValue) {
  return static_cast<pfloat_t>(adcValue) * 0.0002442002442f;
}

pfloat_t InternalAdc::Values(const uint8_t i) {
  uint32_t v;

  if (channelIndex[i][0] == AdcChannelTypeCv) {
    v = 4095 - values_[channelIndex[i][2]][channelIndex[i][1]];
  } else if (channelIndex[i][0] == AdcChannelTypeNormal) {
    v = values_[channelIndex[i][2]][channelIndex[i][1]];
  } else if (channelIndex[i][0] == AdcChannelTypeMuxed) {
    v = muxValues_[channelIndex[i][1]][channelIndex[i][2]];
  } else {
    assert_param(1 == 0);
  }

  return AdcValueToUnit(v);
}

pfloat_t InternalAdc::ModulationValues(const uint8_t src,
  const uint8_t dst) {
  uint32_t v = muxValues_[modIndex[src][dst][0]][modIndex[src][dst][1]];
  return AdcValueToUnit(v);
}

void InternalAdc::Dma2Stream0Handler() {
  // Give the muxes a bit of time to settle before reading from them.
  // Possibly overkill as the intrinsic amount of time for things to happen
  // is much longer than mux progragation time (in nanosecs range), but I'd
  // rather just avoid the possibility of writing a wrong value into muxValues_
  // after DMA completes
  if ((s::sampleCount - muxUpdateTimestamp_) >= 1) {
    muxValues_[0][muxCount_] = values_[6][1];  // first mux channel from ADC 2

    // other 3 mux channels from ADC 3
    for (uint8_t i = 0; i < kNumMuxChannels - 1; i++) {
      muxValues_[i+1][muxCount_] = values_[5+i][2];
    }
  }

  HAL_DMA_IRQHandler(&dmaAdc1Hdl_);
}

void InternalAdc::UpdateMuxAddress(uint32_t sampleTimestamp) {
  muxUpdateTimestamp_ = sampleTimestamp;
  muxCount_ = (muxCount_ + 1) % kNumMuxAddresses;

  GPIOD->ODR  = (GPIOD->ODR & 0xFFFFFFF8) | muxCount_;
}

}  // namespace quadop
