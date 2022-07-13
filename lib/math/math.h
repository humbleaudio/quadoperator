// Copyright 2016 Will Stockwell

#ifndef LIB_MATH_MATH_H_
#define LIB_MATH_MATH_H_

#include <math.h>

#include "lib/math/approx_equal.h"
#include "lib/math/exp.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const pfloat_t kPi;

pfloat_t bound(pfloat_t min, pfloat_t max, pfloat_t x) __attribute__((const));
pfloat_t wabsf(pfloat_t x) __attribute__((const));
pfloat_t wminf(pfloat_t x, pfloat_t y) __attribute__((const));
pfloat_t wmaxf(pfloat_t x, pfloat_t y) __attribute__((const));
pfloat_t wrap(pfloat_t x) __attribute__((const));
pfloat_t wrap_down(pfloat_t x) __attribute__((const));
pfloat_t wrap_up(pfloat_t x) __attribute__((const));
pfloat_t sign(pfloat_t x) __attribute__((const));
pfloat_t phase_to_unit(pfloat_t x) __attribute__((const));  // (-1,1) to (0,1)
uint8_t non_neg(pfloat_t x) __attribute__((const));

#ifdef __cplusplus
}
#endif

#endif  // LIB_MATH_MATH_H_
