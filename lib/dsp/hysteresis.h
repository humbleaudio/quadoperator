// Copyright 2017 Humble Audio LLC

#ifndef LIB_DSP_HYSTERESIS_H_
#define LIB_DSP_HYSTERESIS_H_

#include <conf/arch_conf.h>

#include "stmlib/stmlib.h"

namespace dsp {

class Hysteresis {
 public:
  Hysteresis() { }
  ~Hysteresis() { }

  void Init(pfloat_t unitChangeThreshold);
  pfloat_t Update(const pfloat_t unit, const pfloat_t val);
  pfloat_t Value();

 private:
  pfloat_t unitChangeThres_;
  pfloat_t unit_;
  pfloat_t val_;

  DISALLOW_COPY_AND_ASSIGN(Hysteresis);
};

}  // namespace dsp

#endif  // LIB_DSP_HYSTERESIS_H_
