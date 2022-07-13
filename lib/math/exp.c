// Copyright 2015 Will Stockwell

#include <conf/arch_conf.h>

#include "lib/math/exp.h"
#include "lib/math/exp_resource.h"

#include "lib/math/math.h"

pfloat_t wexp2(pfloat_t x) {
  assert_param(0.0f <= x && x <= 11.0f);

  pfloat_t semi = x * 48.0f;
  uint16_t index = (uint16_t)semi;
  pfloat_t p = (semi - (pfloat_t)index);  // NOLINT(readability/casting)

  return (1 - p) * exp2Table[index] + p * exp2Table[index+1];
}
