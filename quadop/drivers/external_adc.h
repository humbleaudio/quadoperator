// Copyright 2017 Humble Audio LLC

#ifndef QUADOP_DRIVERS_EXTERNAL_ADC_H_
#define QUADOP_DRIVERS_EXTERNAL_ADC_H_

#include <conf/arch_conf.h>

#include "stmlib/stmlib.h"

namespace quadop {

class ExternalAdc {
 public:
  ExternalAdc() { }
  ~ExternalAdc() { }

  void Init();
  void StartConversion();
  pfloat_t Value();
  void DmaHandler();

 private:
  SAI_HandleTypeDef saiHdl_;
  DMA_HandleTypeDef dmaSaiHdl_;
  uint32_t dmaBuf_[2];

  DISALLOW_COPY_AND_ASSIGN(ExternalAdc);
};

}  // namespace quadop

#endif  // QUADOP_DRIVERS_EXTERNAL_ADC_H_
