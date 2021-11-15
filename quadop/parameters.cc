// Copyright 2017 Humble Audio LLC

#include "quadop/parameters.h"

#include "lib/math/math.h"
#include "lib/unit.h"

const uint32_t kEmptyTimestamp = 0xffffffff;

const pfloat_t kNumRatios = 21;
const pfloat_t kMasterCorrectionFactor = 0.988954386815f;
const pfloat_t kRatioOneToThreeCorrectionFactor = 0.993455199222f;

// TODO(will): finalize set of ratio values
const pfloat_t kRatios[22] = {
  0.0909091f,  // 1/11th
  0.1f,
  0.1111111f,
  0.125f,
  0.1428571f,
  0.1666667f,
  0.2f,
  0.25f,
  0.3333333f,
  0.5f,
  1.0f,
  2.0f,
  3.0f,
  4.0f,
  5.0f,
  6.0f,
  7.0f,
  8.0f,
  9.0f,
  10.0f,
  11.0f,
  11.0f,
  // 11.0f,
  // 12.0f,
  // 13.0f,
  // 14.0f,
  // 15.0f,
  // 16.0f,
  // 17.0f,
  // 18.0f,
  // 19.0f,
  // 20.0f,
  // 21.0f,
  // 22.0f,
  // 23.0f,
  // 24.0f,
  // 24.0f
};

pfloat_t unit_to_ratio(const pfloat_t ratio_unit) {
  return kRatios[(uint32_t)(ratio_unit * kNumRatios)];
}

pfloat_t unit_to_detune(const pfloat_t detune_unit) {
  // +/- 6 semitones
  if (detune_unit < 0.5f) {
    return 0.75f + 0.5f * detune_unit;
  } else {
    return 0.5f + 1.0f * detune_unit;
  }
}

pfloat_t units_to_frequency(OscState oscState,
  const pfloat_t coarse,
  const pfloat_t fine,
  const pfloat_t lf_fm,
  const pfloat_t vpero,
  const pfloat_t vpero_scale_cal,
  const pfloat_t vpero_offset_cal,
  const pfloat_t vpero_correction_factor) {
  pfloat_t cal_vpero = (vpero_scale_cal * vpero + vpero_offset_cal)
                        * vpero_correction_factor;
  pfloat_t true_vpero
    = 8.0f * coarse + unit_to_bi(fine) + unit_to_bi(lf_fm) + cal_vpero;

  // Base is C0 or C0 / 32 (5 octaves below)
  pfloat_t base_freq = oscState == OscStateVco ? 16.35f : 0.5109375f;
  return base_freq * wexp2(wmaxf(0.0f, wminf(11.0f, true_vpero)));
}

namespace quadop {

void Parameters::Init(InternalAdc * intAdc,
  ExternalAdc * extAdc,
  Settings * settings) {
  intAdc_ = intAdc;
  extAdc_ = extAdc;
  settings_ = settings;

  // Most parameters are sampled at 1/6th the audio rate
  // (as defined by the beahvior of SchedulerBinder's round robin
  // scheduling mechanism, which is not neccessarily consistent with
  // how frequently the DMA + ADC interaction writes values to memory)
  pfloat_t paramSampleRate = kSampleRate / 6.0;

  for (uint8_t i = 0; i < quadop::kNumOps; i++) {
    op_[i].ratioHys.Init(0.25f / kNumRatios);
    op_[i].ratioKnobAvg.Init();
    op_[i].ratioCvAvg.Init();
    op_[i].detuneAvg.Init();
    op_[i].shapeAvg.Init();
    op_[i].gainAvg.Init();

    for (uint8_t j = 0; j < quadop::kNumOps+1; j++) {
      op_[i].modAvg[j].Init();
    }
  }

  coarseAvg_.Init();
  fineAvg_.Init();
  gainExtAvg_.Init();
  crossFadeAvg_.Init();

  // Low freqeuncy FM is an exception--we sample it at the full
  // audio
  lowFreqFmAvg_.Init();
  resetDetect_.Init(0.5f);
  clipInd_.Init();
}

void Parameters::UpdateMasterFrequency(OscState oscState) {
  oscState_ = oscState;

  masterFreq_
    = units_to_frequency(oscState_,
                         coarseAvg_.Update(intAdc_->Values(AdcChannelCoarse)),
                         fineAvg_.Update(intAdc_->Values(AdcChannelFine)),
                         lowFreqFmAvg_.Update(intAdc_->Values(AdcChannelLowFreqFm)),
                         intAdc_->Values(AdcChannelVPerO),
                         settings_->VPerOScale(),
                         settings_->VPerOOffset(),
                         kMasterCorrectionFactor
                         );
}

pfloat_t Parameters::LowFreqFm() {
  pfloat_t fm
    = intAdc_->Values(AdcChannelLowFreqFm);
  return unit_to_bi(lowFreqFmAvg_.Update(fm));
}

bool Parameters::Reset() {
  return resetDetect_.Update(unit_to_bi(intAdc_->Values(AdcChannelReset)));
}

pfloat_t Parameters::GainExt() {
  return bound(0.0f, 1.0f,
               unit_wide_zero(gainExtAvg_.Update(intAdc_->Values(AdcChannelGainCvExt)), 0.01f));
}

pfloat_t Parameters::Gain(uint8_t i) {
  assert_param(0 <= i && i < kNumOps);

  AdcChannel gainCh = static_cast<AdcChannel>(AdcChannelGainCv1 + i);
  return bound(0.0f, 1.0f,
               unit_wide_zero(op_[i].gainAvg.Update(intAdc_->Values(gainCh)), 0.01f));
}

pfloat_t Parameters::Shape(uint8_t i) {
  assert_param(0 <= i && i < kNumOps);

  AdcChannel shapeCvCh = static_cast<AdcChannel>(AdcChannelShapeCv1 + i);
  AdcChannel shapeKnobCh = static_cast<AdcChannel>(AdcChannelShapeKnob1 + i);

  pfloat_t trueShapeParam
    = unit_to_bi(intAdc_->Values(shapeCvCh)) + intAdc_->Values(shapeKnobCh);

  return bound(0.0f, 1.0f, op_[i].shapeAvg.Update(trueShapeParam));
}

pfloat_t Parameters::Mod(uint8_t slot, uint8_t j, uint8_t i) {
  assert_param(0 <= slot && slot < 4);
  assert_param(0 <= j && j < kNumOps+1);
  assert_param(0 <= i && i < kNumOps);

  switch (slot) {
    case 0:
    case 1:
    case 2:
      return settings_->AlgoSlotMod(slot, j, i);

    case 3:
    {
      pfloat_t mod = op_[i].modAvg[j].Update(intAdc_->ModulationValues(j, i));
      return unit_exp(unit_wide_zero(mod, 0.01f));
    }
    default:
      return 0.0f;
  }
}

pfloat_t Parameters::LockedFrequency(uint8_t i) {
  assert_param(0 <= i && i < kNumOps);

  AdcChannel ratioCvCh = static_cast<AdcChannel>(AdcChannelRatioCv1 + i);
  AdcChannel ratioKnobCh = static_cast<AdcChannel>(AdcChannelRatioKnob1 + i);
  AdcChannel detuneCh = static_cast<AdcChannel>(AdcChannelDetuneKnob1 + i);

  // 1.1 factor compensates for -5.5v to 5.5v of ratio CV to behave like
  // -5v to 5v in non-v/o case (with bounding to limit range to -5v to 5v).
  pfloat_t ratioCv
    = bound(-1.0f, 1.0f,
            1.1f * unit_to_bi(intAdc_->Values(ratioCvCh)));

  pfloat_t aggRatioParam
    = bound(0.0f, 1.0f,
            intAdc_->Values(ratioKnobCh) + ratioCv);

  pfloat_t opRatio
    = op_[i].ratioHys.Update(aggRatioParam,
                             unit_to_ratio(aggRatioParam));

  pfloat_t filteredDetune = op_[i].detuneAvg.Update(intAdc_->Values(detuneCh));
  pfloat_t opDetune = unit_to_detune(unit_detent(filteredDetune, 0.05f));
  return opRatio * opDetune * masterFreq_;
}

pfloat_t Parameters::FreeFrequency(uint8_t i) {
  assert_param(0 <= i && i < kNumOps);

  AdcChannel ratioCvCh = static_cast<AdcChannel>(AdcChannelRatioCv1 + i);
  AdcChannel ratioKnobCh = static_cast<AdcChannel>(AdcChannelRatioKnob1 + i);
  AdcChannel detuneCh = static_cast<AdcChannel>(AdcChannelDetuneKnob1 + i);

  return units_to_frequency(oscState_,
                            op_[i].ratioKnobAvg.Update(intAdc_->Values(ratioKnobCh)),
                            op_[i].detuneAvg.Update(intAdc_->Values(detuneCh)),
                            0.0f,  // Ignore "LF FM" in free mode
                            op_[i].ratioCvAvg.Update(intAdc_->Values(ratioCvCh)),
                            settings_->RatioCvVPerOScale(i),
                            settings_->RatioCvVPerOOffset(i),
                            i == 3 ? kMasterCorrectionFactor : kRatioOneToThreeCorrectionFactor
                            );
}

pfloat_t Parameters::UpdateExternalLevel() {
  pfloat_t extLevel = extAdc_->Value();
  clipInd_.AddSample(extLevel);
  return extLevel + settings_->ArFmOffset();
}

pfloat_t Parameters::ClipIndicatorLevel() {
  return clipInd_.IndicatorLevel();
}

pfloat_t Parameters::AlgoRightGain() {
  pfloat_t gain = crossFadeAvg_.Update(
    intAdc_->Values(AdcChannelAlgoCrossfadeKnob)
      + unit_to_bi(intAdc_->Values(AdcChannelAlgoCrossfadeCv)));
  return bound(0.0f, 1.0f, gain);
}

}  // namespace quadop
