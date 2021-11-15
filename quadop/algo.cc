// Copyright 2018 Humble Audio LLC

#include "quadop/algo.h"

namespace quadop {

const uint32_t kEmptyTimestamp = 0xffffffff;
// It's hacky, but for reasons defined in scheduler_binder_impl.cc
// AdvanceState() is called once every 6 samples. So, these numbers
// below divided by 6.
const uint32_t kLongPressDuration = 8000;  // 8000 / (48000 / 6) = 1.0 s
const uint32_t kBlinkDuration = 4000;  // 4000 / (48000 / 6) = 0.5 s
const uint32_t kBlinkDurationMod2 = 2000;  // 0.25 s
const uint32_t kBlinkDurationMod4 = 1000;  // 0.125 s

void AlgoButtonPressStateMachine::Init(uint32_t longPressDuration) {
  longPressDuration_ = longPressDuration;
  state_ = AlgoButtonPressStateUp;
  pressInc_ = 0;
}

AlgoButtonPressState AlgoButtonPressStateMachine::State() {
  return state_;
}

AlgoButtonPressState AlgoButtonPressStateMachine::AdvanceState() {
  if (state_ == AlgoButtonPressStateDown) {
    pressInc_++;

    if (pressInc_ == longPressDuration_) {
      pressInc_ = 0;
      state_ = AlgoButtonPressStateDownAndSaved;
      return AlgoButtonPressStateDownAndSaveNow;
    }
  }

  return state_;
}

AlgoButtonPressState AlgoButtonPressStateMachine::SetPressChange(bool isPressed) {  // NOLINT(whitespace/line_length)
  if (isPressed) {  // newly down
    switch (state_) {
    case AlgoButtonPressStateUp:
    {
      state_ = AlgoButtonPressStateDown;
      return state_;
    }
    }
  } else {  // newly up
    switch (state_) {
    case AlgoButtonPressStateDown:
    {
      pressInc_ = 0;
      state_ = AlgoButtonPressStateUp;
      // this is an instantial state that we never set to the internal
      // state_ member -- it's here just to signal to the caller (the Algo
      // class instance) that is should change it's internal representation
      // of left and right presets in the crossfade
      return AlgoButtonPressStateUpAndSetNewLeft;
    }

    case AlgoButtonPressStateDownAndSaved:
    {
      pressInc_ = 0;
      state_ = AlgoButtonPressStateUp;
      return state_;
    }
  }
  }

  return state_;
}


void AlgoButtonBlinkStateMachine::Init() {
  state_ = AlgoButtonBlinkStateInactive;
  blinkInc_ = 0;
}

AlgoButtonBlinkState AlgoButtonBlinkStateMachine::State() {
  return state_;
}

void AlgoButtonBlinkStateMachine::AdvanceState() {
  if (state_ == AlgoButtonBlinkStateActive) {
    blinkInc_++;

    if (blinkInc_ == kBlinkDuration) {
      blinkInc_ = 0;
      state_ = AlgoButtonBlinkStateInactive;
    }
  }
}

void AlgoButtonBlinkStateMachine::StartBlink() {
  state_ = AlgoButtonBlinkStateActive;
}

bool AlgoButtonBlinkStateMachine::IsBlinkHigh() {
  if (state_ == AlgoButtonBlinkStateInactive) {
    return false;
  }

  return (blinkInc_ < kBlinkDurationMod4
          || (kBlinkDurationMod2 <= blinkInc_
              && blinkInc_ < (kBlinkDurationMod2 + kBlinkDurationMod4)));
}


void Algo::Init(Parameters * params, Settings * settings) {
  params_ = params;
  set_ = settings;

  // Initiate the algo slot press and blink state machines state machines
  for (uint8_t slot = 0; slot < kNumOps-1; slot++) {
    pressStm_[slot].Init(kLongPressDuration);
    blinkStm_[slot].Init();
  }

  // The live button save presets (or blink), so it give it an effecitively
  // infinite long press duration
  pressStm_[kNumOps-1].Init(0xffffffff);

  // These active slot defaults are also beneficial if algo is not
  // attached. The result is we can avoid branching logic in the main
  // execution of SchedulerBinder::Run()
  activeSlots_[0] = AlgoSlotLive;
  activeSlots_[1] = AlgoSlotLive;
}

void Algo::SetSlotPress(AlgoSlot slot, bool on, uint32_t timestamp) {
  AlgoButtonPressState pressState = pressStm_[slot].SetPressChange(on);
  if (pressState == AlgoButtonPressStateUpAndSetNewLeft) {
    SetNewActiveSlot(slot);
  }
}

bool Algo::IsBlinking(AlgoSlot slot) {
  if (slot < NumAlgoSlots-1) {
    return blinkStm_[slot].State() == AlgoButtonBlinkStateActive;
  } else {  // live slot
    return false;
  }
}

bool Algo::IsBlinkingHighNow(AlgoSlot slot) {
  if (slot < NumAlgoSlots-1) {
    return blinkStm_[slot].IsBlinkHigh();
  } else {  // live slot
    return false;
  }
}

void Algo::AdvanceState(AlgoSlot slot) {
  AlgoButtonPressState pressState = pressStm_[slot].AdvanceState();
  if (pressState == AlgoButtonPressStateDownAndSaveNow) {
    // Saving the preset
    for (uint8_t j = 0; j < kNumOps+1; j++) {
      for (uint8_t i = 0; i < kNumOps; i++) {
        set_->SetAlgoSlotMod(slot, j, i, params_->Mod(AlgoSlotLive, j, i));
      }
    }

    set_->Save();

    // Initiate the LED blink
    if (slot < NumAlgoSlots-1) {
      blinkStm_[slot].StartBlink();
    }
  }

  if (slot < NumAlgoSlots-1) {
    blinkStm_[slot].AdvanceState();
  }
}

AlgoSlot Algo::LeftActiveSlot() {
  return activeSlots_[0];
}

AlgoSlot Algo::RightActiveSlot() {
  return activeSlots_[1];
}

void Algo::SetNewActiveSlot(AlgoSlot slot) {
  activeSlots_[0] = activeSlots_[1];
  activeSlots_[1] = slot;
}

}  // namespace quadop
