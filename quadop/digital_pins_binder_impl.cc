// Copyright 2017 Humble Audio LLC

#include "quadop/state.h"

typedef quadop::State s;

namespace quadop {

void DigitalPinsBinder::HandlePin(const DigitalPin pin, const bool pinState) {
  switch (pin) {
  case DigitalPinLockSwitch1:
  case DigitalPinLockSwitch2:
  case DigitalPinLockSwitch3:
  case DigitalPinLockSwitch4:
  case DigitalPinVcoLfoSwitch:
#if defined(CVTEST)
    s::cvTest.HandleSwitchFlip();
#else
    s::cal.HandleSwitchChange(pin, s::sampleCount);
#endif
    break;

  case DigitalPinAlgoSlotA:
  case DigitalPinAlgoSlotB:
  case DigitalPinAlgoSlotC:
  case DigitalPinAlgoSlotLive:
    if (s::algoAttached) {
      AlgoSlot slot = static_cast<AlgoSlot>(pin - DigitalPinAlgoSlotA);
      // invert pin state since button press connects it to ground
      s::algo.SetSlotPress(slot, ! pinState, s::sampleCount);
    }
    break;
  }
}

}  // namespace quadop
