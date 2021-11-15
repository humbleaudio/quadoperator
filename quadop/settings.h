// Copyright 2017 Humble Audio LLC

#ifndef QUADOP_SETTINGS_H_
#define QUADOP_SETTINGS_H_

#include <conf/arch_conf.h>

#include "stmlib/stmlib.h"
#include "quadop/drivers/sector_storage.h"
#include "quadop/dsp/operator_bank.h"

namespace quadop {

typedef struct {
  pfloat_t vpero_scale;
  pfloat_t vpero_offset;
  pfloat_t ratio_cv_vpero_scale[kNumOps];
  pfloat_t ratio_cv_vpero_offset[kNumOps];
  pfloat_t ar_fm_offset;
  pfloat_t algo_slot[3][kNumOps+1][kNumOps];
  uint8_t magic_byte;
} SettingsData;

class Settings {
 public:
  Settings() { }
  ~Settings() { }

  void Init();
  void Reset();
  void Save();

  pfloat_t VPerOScale();
  void SetVPerOScale(pfloat_t vperoScale);
  pfloat_t VPerOOffset();
  void SetVPerOOffset(pfloat_t vperoOffset);

  // Calibration data for the operators' free state where ratio CV
  // becomes volt per octave input
  pfloat_t RatioCvVPerOScale(uint8_t opIndex);
  void SetRatioCvVPerOScale(uint8_t opIndex, pfloat_t ratioScale);
  pfloat_t RatioCvVPerOOffset(uint8_t opIndex);
  void SetRatioCvVPerOOffset(uint8_t opIndex, pfloat_t ratioOffset);
  pfloat_t ArFmOffset();
  void SetArFmOffset(pfloat_t offset);

  pfloat_t AlgoSlotMod(uint8_t slot, uint8_t src, uint8_t dst);
  void SetAlgoSlotMod(uint8_t slot, uint8_t src, uint8_t dst, pfloat_t mod);

 private:
  void CheckIntegrity();

  quadop::Storage<10> storage_;
  SettingsData data_;
  uint16_t version_token_;

  DISALLOW_COPY_AND_ASSIGN(Settings);
};

}  // namespace quadop

#endif  // QUADOP_SETTINGS_H_
