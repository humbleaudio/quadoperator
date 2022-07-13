// Copyright 2016 Will Stockwell

#ifndef LIB_DSP_POLY_BLEP_H_
#define LIB_DSP_POLY_BLEP_H_

#include <conf/arch_conf.h>

#ifdef __cplusplus
extern "C" {
#endif

pfloat_t poly_blep(pfloat_t phase,
  pfloat_t phaseInc,
  pfloat_t invPhaseInc) __attribute__((const));

#ifdef __cplusplus
}
#endif

#endif  // LIB_DSP_POLY_BLEP_H_
