// Copyright 2016 Will Stockwell

#include <conf/arch_conf.h>

#include "lib/math/math.h"

const pfloat_t kPi = 3.14159265358979f;

pfloat_t bound(pfloat_t min, pfloat_t max, pfloat_t x) {
  return wmaxf(min, wminf(max, x));
}

pfloat_t wabsf(pfloat_t x) {
  return (x >= 0.0f) ? x : -1.0f * x;
}

pfloat_t wminf(pfloat_t x, pfloat_t y) {
  return (x < y) ? x : y;
}

pfloat_t wmaxf(pfloat_t x, pfloat_t y) {
  return (x > y) ? x : y;
}

pfloat_t wrap(pfloat_t x) {
  return wrap_down(wrap_up(x));
}

pfloat_t wrap_down(pfloat_t x) {
  while (x > 1.0f) { x -= 2.0f; }
  return x;
}

pfloat_t wrap_up(pfloat_t x) {
  while (x < -1.0f) { x += 2.0f; }
  return x;
}

pfloat_t sign(pfloat_t x) {
  return (x >= 0.0f) ? 1.0f: -1.0f;
}

pfloat_t phase_to_unit(pfloat_t x) {
  return 0.5f * (x + 1.0f);
}

uint8_t non_neg(pfloat_t x) {
  return x >= 0.0f;
}
