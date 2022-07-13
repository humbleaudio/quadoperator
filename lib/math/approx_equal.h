// Copyright 2016 Will Stockwell

#ifndef LIB_MATH_APPROX_EQUAL_H_
#define LIB_MATH_APPROX_EQUAL_H_

#include <conf/arch_conf.h>

#ifdef __cplusplus
extern "C" {
#endif

// TODO(will): this isn't really 'approx', maybe do based on percentage diff
uint8_t approx_equal(float x, float y, uint32_t places) __attribute__((pure));

#ifdef __cplusplus
}
#endif

#endif  // LIB_MATH_APPROX_EQUAL_H_
