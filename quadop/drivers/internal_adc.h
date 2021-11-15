// Copyright 2017 Humble Audio LLC

#ifndef QUADOP_DRIVERS_INTERNAL_ADC_H_
#define QUADOP_DRIVERS_INTERNAL_ADC_H_

#include <conf/arch_conf.h>

#include "stmlib/stmlib.h"

extern const pfloat_t kSampleRate;

namespace quadop {

const uint8_t kNumAdcs = 3;
const uint8_t kNumRanks = 8;
const uint8_t kNumMuxChannels = 4;
const uint8_t kNumMuxAddresses = 8;

typedef enum {
  AdcChannelVPerO,
  AdcChannelReset,
  AdcChannelLowFreqFm,
  AdcChannelCoarse,
  AdcChannelFine,
  AdcChannelRatioKnob1,
  AdcChannelRatioKnob2,
  AdcChannelRatioKnob3,
  AdcChannelRatioKnob4,
  AdcChannelDetuneKnob1,
  AdcChannelDetuneKnob2,
  AdcChannelDetuneKnob3,
  AdcChannelDetuneKnob4,
  AdcChannelShapeKnob1,
  AdcChannelShapeKnob2,
  AdcChannelShapeKnob3,
  AdcChannelShapeKnob4,
  AdcChannelRatioCv1,
  AdcChannelRatioCv2,
  AdcChannelRatioCv3,
  AdcChannelRatioCv4,
  AdcChannelShapeCv1,
  AdcChannelShapeCv2,
  AdcChannelShapeCv3,
  AdcChannelShapeCv4,
  AdcChannelGainCv1,
  AdcChannelGainCv2,
  AdcChannelGainCv3,
  AdcChannelGainCv4,
  AdcChannelGainCvExt,
  AdcChannelModSrc1Des1,
  AdcChannelModSrc1Des2,
  AdcChannelModSrc1Des3,
  AdcChannelModSrc1Des4,
  AdcChannelModSrc2Des1,
  AdcChannelModSrc2Des2,
  AdcChannelModSrc2Des3,
  AdcChannelModSrc2Des4,
  AdcChannelModSrc3Des1,
  AdcChannelModSrc3Des2,
  AdcChannelModSrc3Des3,
  AdcChannelModSrc3Des4,
  AdcChannelModSrc4Des1,
  AdcChannelModSrc4Des2,
  AdcChannelModSrc4Des3,
  AdcChannelModSrc4Des4,
  AdcChannelModSrcExtDes1,
  AdcChannelModSrcExtDes2,
  AdcChannelModSrcExtDes3,
  AdcChannelModSrcExtDes4,
  AdcChannelAlgoCrossfadeKnob,
  AdcChannelAlgoCrossfadeCv,
  NumAdcChannels
} AdcChannel;

class InternalAdc {
 public:
  InternalAdc() { }
  ~InternalAdc() { }

  void Init();
  pfloat_t Values(const uint8_t channel);
  pfloat_t ModulationValues(const uint8_t source, const uint8_t destination);
  void StartConversion();
  void UpdateMuxAddress(uint32_t sampleTimestamp);
  void Dma2Stream0Handler();

 private:
  void InitAdc(ADC_TypeDef * adc, ADC_HandleTypeDef * adcHdl, uint32_t numConv);
  void InitChannel(ADC_HandleTypeDef * adcHdl, uint32_t chan, uint32_t rank);
  void InitAdcDma(DMA_HandleTypeDef * dmaHdl,
    ADC_HandleTypeDef * adcHdl,
    DMA_Stream_TypeDef * dma_stream,
    uint32_t channel);
  pfloat_t AdcValueToUnit(const uint32_t adcValue);

  DMA_HandleTypeDef dmaAdc1Hdl_, dmaAdc2Hdl_, dmaAdc3Hdl_;
  ADC_HandleTypeDef adc1Hdl_, adc2Hdl_, adc3Hdl_;
  uint32_t muxUpdateTimestamp_;

  volatile uint8_t muxCount_;
  volatile uint32_t values_[kNumRanks][kNumAdcs];
  volatile uint32_t muxValues_[kNumMuxChannels][kNumMuxAddresses];

  DISALLOW_COPY_AND_ASSIGN(InternalAdc);
};

}  // namespace quadop

#endif  // QUADOP_DRIVERS_INTERNAL_ADC_H_
