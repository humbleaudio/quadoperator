// Copyright 2017 Humble Audio LLC

#include "quadop/settings.h"

#include <math.h>

namespace quadop {

const char kMagicByte = 'M';

void Settings::Init() {
  if (!storage_.ParsimoniousLoad(&data_, &version_token_)
      || data_.magic_byte != kMagicByte) {
    Reset();
  }
  CheckIntegrity();
}

void Settings::Reset() {
  data_.vpero_scale = 7.0f;
  data_.vpero_offset = -1.5f;

  for (uint8_t i = 0; i < kNumOps; i++) {
    data_.ratio_cv_vpero_scale[i] = 11.0f;
    data_.ratio_cv_vpero_offset[i] = -5.5f;

    for (uint8_t j = 0; j < kNumOps+1; j++) {
      for (uint8_t k = 0; k < 3; k++) {
        data_.algo_slot[k][j][i] = 0.0f;
      }
    }
  }

  data_.magic_byte = kMagicByte;
}

void Settings::Save() {
  CheckIntegrity();
  storage_.ParsimoniousSave(data_, &version_token_);
}

pfloat_t Settings::VPerOScale() {
  return data_.vpero_scale;
}

void Settings::SetVPerOScale(pfloat_t vperoScale) {
  assert_param(!isnan(vperoScale));
  data_.vpero_scale = vperoScale;
}

pfloat_t Settings::VPerOOffset() {
  return data_.vpero_offset;
}

void Settings::SetVPerOOffset(pfloat_t vperoOffset) {
  assert_param(!isnan(vperoOffset));
  data_.vpero_offset = vperoOffset;
}

pfloat_t Settings::RatioCvVPerOScale(uint8_t opIndex) {
  assert_param(0 <= opIndex < kNumOps);
  return data_.ratio_cv_vpero_scale[opIndex];
}

void Settings::SetRatioCvVPerOScale(uint8_t opIndex, pfloat_t ratioScale) {
  assert_param(0 <= opIndex < kNumOps);
  assert_param(!isnan(ratioScale));
  data_.ratio_cv_vpero_scale[opIndex] = ratioScale;
}

pfloat_t Settings::RatioCvVPerOOffset(uint8_t opIndex) {
  assert_param(0 <= opIndex < kNumOps);
  return data_.ratio_cv_vpero_offset[opIndex];
}

void Settings::SetRatioCvVPerOOffset(uint8_t opIndex, pfloat_t ratioOffset) {
  assert_param(0 <= opIndex < kNumOps);
  assert_param(!isnan(ratioOffset));
  data_.ratio_cv_vpero_offset[opIndex] = ratioOffset;
}

pfloat_t Settings::ArFmOffset() {
  return data_.ar_fm_offset;
}

void Settings::SetArFmOffset(pfloat_t offset) {
  data_.ar_fm_offset = offset;
}

pfloat_t Settings::AlgoSlotMod(uint8_t slot, uint8_t src, uint8_t dst) {
  assert_param(0 <= slot < 3);
  assert_param(0 <= src < kNumOps+1);
  assert_param(0 <= dst < kNumOps);
  return data_.algo_slot[slot][src][dst];
}

void Settings::SetAlgoSlotMod(uint8_t slot,
  uint8_t src,
  uint8_t dst,
  pfloat_t mod) {
  assert_param(0 <= slot < 3);
  assert_param(0 <= src < kNumOps+1);
  assert_param(0 <= dst < kNumOps);
  assert_param(!isnan(mod));
  data_.algo_slot[slot][src][dst] = mod;
}

void Settings::CheckIntegrity() {
  assert_param(!isnan(data_.vpero_scale));
  assert_param(!isnan(data_.vpero_offset));

  for (uint8_t i = 0; i < kNumOps; i++) {
    assert_param(!isnan(data_.ratio_cv_vpero_scale[i]));
    assert_param(!isnan(data_.ratio_cv_vpero_offset[i]));
  }

  // TODO(will): complete integrity checks
  assert_param(data_.magic_byte == kMagicByte);
}

}  // namespace quadop
