// Copyright 2016 Will Stockwell

#include "lib/math/approx_equal.h"

#include <conf/arch_conf.h>

uint8_t approx_equal(float x, float y, uint32_t places) {
  for (uint8_t i = 0; i < places; i++) {
    x *= 10;
    y *= 10;
  }

  uint32_t xi = (int)x;  // NOLINT(readability/casting)
  uint32_t yi = (int)y;  // NOLINT(readability/casting)

  return xi == yi;
}
