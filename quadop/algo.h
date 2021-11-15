// Copyright 2018 Humble Audio LLC

#ifndef QUADOP_ALGO_H_
#define QUADOP_ALGO_H_

#include "stmlib/stmlib.h"

#include "quadop/parameters.h"
#include "quadop/settings.h"

namespace quadop {

typedef enum {
  AlgoSlotA,
  AlgoSlotB,
  AlgoSlotC,
  AlgoSlotLive,
  NumAlgoSlots
} AlgoSlot;

typedef enum {
  AlgoButtonPressStateUp,
  AlgoButtonPressStateUpAndSetNewLeft,
  AlgoButtonPressStateDown,
  AlgoButtonPressStateDownAndSaveNow,
  AlgoButtonPressStateDownAndSaved,
  NumAlgoButtonPressStates
} AlgoButtonPressState;

typedef enum {
  AlgoButtonBlinkStateInactive,
  AlgoButtonBlinkStateActive,
  NumAlgoButtonBlinkStates
} AlgoButtonBlinkState;

class AlgoButtonPressStateMachine {
 public:
  AlgoButtonPressStateMachine() { }
  ~AlgoButtonPressStateMachine() { }

  void Init(uint32_t longPressDuration);
  AlgoButtonPressState State();
  AlgoButtonPressState AdvanceState();  // returns new state
  AlgoButtonPressState SetPressChange(bool isPressed);  // returns new state
 private:
  uint32_t longPressDuration_;
  AlgoButtonPressState state_;
  uint32_t pressInc_;

  DISALLOW_COPY_AND_ASSIGN(AlgoButtonPressStateMachine);
};

class AlgoButtonBlinkStateMachine {
 public:
  AlgoButtonBlinkStateMachine() { }
  ~AlgoButtonBlinkStateMachine() { }

  void Init();
  AlgoButtonBlinkState State();
  void AdvanceState();  // called in each sample frame
  void StartBlink();
  bool IsBlinkHigh();
 private:
  AlgoButtonBlinkState state_;
  uint32_t blinkInc_;

  DISALLOW_COPY_AND_ASSIGN(AlgoButtonBlinkStateMachine);
};

class Algo {
 public:
  Algo() { }
  ~Algo() { }

  void Init(Parameters * params, Settings * settings);
  void SetSlotPress(AlgoSlot slot, bool on, uint32_t timestamp);
  bool IsBlinking(AlgoSlot slot);
  bool IsBlinkingHighNow(AlgoSlot slot);
  void AdvanceState(AlgoSlot slot);
  AlgoSlot LeftActiveSlot();
  AlgoSlot RightActiveSlot();

 private:
  void SetNewActiveSlot(AlgoSlot slot);

  Parameters * params_;
  Settings * set_;
  AlgoButtonPressStateMachine pressStm_[4];
  AlgoButtonBlinkStateMachine blinkStm_[3];
  AlgoSlot activeSlots_[2];  // left is 0, right is 1

  DISALLOW_COPY_AND_ASSIGN(Algo);
};

}  // namespace quadop

#endif  // QUADOP_ALGO_H_
