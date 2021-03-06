// Copyright 2016 Will Stockwell

#ifndef QUADOP_DRIVERS_DAC_H_
#define QUADOP_DRIVERS_DAC_H_

#include <stm32f7xx_conf.h>

#include "conf/arch_conf.h"
#include "stmlib/stmlib.h"

namespace quadop {

class Dac {
 public:
  Dac() { }
  ~Dac() { }

  void Init();
  void StartConversion();
  void SetChannels(pfloat_t value1,
    pfloat_t value2,
    pfloat_t value3,
    pfloat_t value4);

 private:
  int32_t FormatSample(pfloat_t value);

  SAI_HandleTypeDef saiHdl_;
  DMA_HandleTypeDef dmaSaiHdl_;
  uint32_t dmaBuf_[8];

  DISALLOW_COPY_AND_ASSIGN(Dac);
};

}  // namespace quadop

#endif  // QUADOP_DRIVERS_DAC_H_
