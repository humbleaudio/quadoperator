// Copyright 2017 Humble Audio LLC

#ifndef QUADOP_DRIVERS_DIGITAL_PINS_H_
#define QUADOP_DRIVERS_DIGITAL_PINS_H_

#include "stmlib/stmlib.h"

namespace quadop {

typedef enum {
  DigitalPinLockSwitch1,
  DigitalPinLockSwitch2,
  DigitalPinLockSwitch3,
  DigitalPinLockSwitch4,
  DigitalPinVcoLfoSwitch,
  DigitalPinAlgoSlotA,
  DigitalPinAlgoSlotB,
  DigitalPinAlgoSlotC,
  DigitalPinAlgoSlotLive,
  DigitalPinAlgoExpanderNotConnected,
  NumDigitalPins
} DigitalPin;

class DigitalPinsBinder {
 public:
  static void HandlePin(const DigitalPin pin, const bool pinState);
};

class DigitalPins {
 public:
  DigitalPins() { }
  ~DigitalPins() { }

  void Init();
  void HandleChangedPinStates();
  bool IsSet(const uint8_t pin);

 private:
  bool ReadPin(const uint8_t pin);

  bool pinState_[NumDigitalPins][2];
  uint8_t currentPinIndex_;

  DISALLOW_COPY_AND_ASSIGN(DigitalPins);
};

}  // namespace quadop

#endif  // QUADOP_DRIVERS_DIGITAL_PINS_H_
