// Copyright 2017 Will Stockwell

#include "quadop/dsp/operator_bank.h"

#include <string.h>

#include "lib/dsp/wave_shape.h"
#include "lib/math/math.h"

// Use a lower max index for self-modulation which quickly
// goes off the rails and pretty consistently like white noise
// beyond an index of 2 of so
// TODO(will): finalize maximum modulation index
const pfloat_t kMaxModAmplitude[5][4] = {
  {2.5f, 5.0f, 5.0f, 5.0f},
  {5.0f, 2.5f, 5.0f, 5.0f},
  {5.0f, 5.0f, 2.5f, 5.0f},
  {5.0f, 5.0f, 5.0f, 2.5f},
  {5.0f, 5.0f, 5.0f, 5.0f}
};

namespace quadop {

void OperatorBank::Init(pfloat_t sampleRate) {
  sampleRate_ = sampleRate;
  samplePeriod_ = 1.0f / sampleRate;
  Reset();
}


void OperatorBank::Reset() {
  for (uint8_t i = 0; i < kNumOps; i++) {
    opState_[i].phase = -1.0f;
  }
}

void OperatorBank::SetParameters(volatile OperatorBankParameters * params) {
  memcpy((void *)&params_,  // NOLINT(readability/casting)
         (void *)params,  // NOLINT(readability/casting)
         sizeof(OperatorBankParameters));

  for (uint8_t i = 0; i < kNumOps; i++) {
    opState_[i].phaseInc = 2.0f * params_.frequency[i] * samplePeriod_;
    opState_[i].invPhaseInc = 1.0f / opState_[i].phaseInc;
  }
}

void OperatorBank::RenderNextFrame(pfloat_t extLevel) {
  pfloat_t newOpLevel[kNumOps];

  for (uint8_t i = 0; i < kNumOps; i++) {
    opState_[i].phase = wrap_down(opState_[i].phase + opState_[i].phaseInc);

    pfloat_t modPhsOffset =
      extLevel * params_.modulation[kExtId][i] * params_.gain[kExtId];
    for (uint8_t j = 0; j < kNumOps; j++) {
      modPhsOffset +=
        opState_[j].level * params_.modulation[j][i] * kMaxModAmplitude[j][i];
    }

    pfloat_t truePhase = wrap(opState_[i].phase + modPhsOffset);

    newOpLevel[i] = wave_shape_variable(truePhase,
                                        opState_[i].phaseInc,
                                        opState_[i].invPhaseInc,
                                        params_.frequency[i],
                                        params_.variable[i]);
  }

  for (uint8_t i = 0; i < kNumOps; i++) {
    opState_[i].level = newOpLevel[i] * params_.gain[i];
  }
}

pfloat_t OperatorBank::OperatorLevel(uint16_t op) {
  return opState_[op].level;
}

}  // namespace quadop
