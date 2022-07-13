// Copyright 2017 Humble Audio LLC

#ifndef LIB_UNIT_H_
#define LIB_UNIT_H_

#include <conf/arch_conf.h>

#ifdef __cplusplus
extern "C" {
#endif

pfloat_t unit_wide_zero(const pfloat_t unit,
  const pfloat_t width) __attribute__((const));
pfloat_t unit_detent(const pfloat_t unit,
  const pfloat_t width) __attribute__((const));
pfloat_t unit_to_bi(const pfloat_t unit) __attribute__((const));
pfloat_t unit_exp(const pfloat_t unit) __attribute__((const));

#ifdef __cplusplus
}
#endif

#endif  // LIB_UNIT_H_
