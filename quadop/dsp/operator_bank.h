// Copyright 2017 Will Stockwell

#ifndef QUADOP_DSP_OPERATOR_BANK_H_
#define QUADOP_DSP_OPERATOR_BANK_H_

#include <conf/arch_conf.h>

#include "stmlib/stmlib.h"

namespace quadop {

#ifdef DEBUG
const uint8_t kNumOps = 2;
#else
const uint8_t kNumOps = 4;
#endif
const uint8_t kExtId = kNumOps;

typedef struct {
  volatile pfloat_t frequency[kNumOps];
  volatile pfloat_t variable[kNumOps];
  volatile pfloat_t gain[kNumOps+1];  // +1 term for external signal's gain
  volatile pfloat_t modulation[kNumOps+1][kNumOps];  // +1 term for ext signal
} OperatorBankParameters;

typedef struct {
  volatile pfloat_t phase;
  volatile pfloat_t phaseInc;
  volatile pfloat_t invPhaseInc;
  volatile pfloat_t level;  // rendered level
} OperatorState;

class OperatorBank {
 public:
  OperatorBank() { }
  ~OperatorBank() { }

  void Init(pfloat_t sampleRate);
  void Reset();

  void SetParameters(volatile OperatorBankParameters * params);

  void RenderNextFrame(pfloat_t externalLevel);
  pfloat_t OperatorLevel(uint16_t op);

 private:
  volatile pfloat_t sampleRate_;
  volatile pfloat_t samplePeriod_;

  volatile OperatorBankParameters params_;
  OperatorState opState_[kNumOps];

  DISALLOW_COPY_AND_ASSIGN(OperatorBank);
};

}  // namespace quadop

#endif  // QUADOP_DSP_OPERATOR_BANK_H_
