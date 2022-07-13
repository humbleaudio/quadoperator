// Copyright 2017 Humble Audio LLC

#include "lib/unit.h"

#include "lib/math/math.h"
#include "lib/unit_exp_resource.h"

pfloat_t unit_wide_zero(const pfloat_t unit, const pfloat_t width) {
  if (unit <= width) {
    return 0.0f;
  } else {
    return (unit - 1.0f) / (1.0f - width) + 1.0f;
  }
}

pfloat_t unit_detent(const pfloat_t unit, const pfloat_t width) {
  pfloat_t slope = 1.0f / (1.0f - width);
  if (unit < (0.5f - width * 0.5f)) {
    return unit * slope;
  } else if (unit > (0.5f + width * 0.5f)) {
    return unit * slope + 1.0f - slope;
  } else {
    return 0.5f;
  }
}

pfloat_t unit_to_bi(const pfloat_t unit) {
  return 2.0f * unit - 1.0f;
}

pfloat_t unit_exp(const pfloat_t unit) {
  assert_param(0.0f <= unit && unit <= 1.0f);

  pfloat_t semi = unit * 100.0f;
  uint16_t index = (uint16_t)semi;
  pfloat_t p = (semi - (pfloat_t)index);  // NOLINT(readability/casting)

  return (1 - p) * unitExpTable[index] + p * unitExpTable[index+1];
}