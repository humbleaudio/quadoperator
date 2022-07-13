// Copyright 2016 Will Stockwell

#ifndef LIB_DSP_WAVE_SHAPE_H_
#define LIB_DSP_WAVE_SHAPE_H_

#include <conf/arch_conf.h>

#ifdef __cplusplus
extern "C" {
#endif

pfloat_t wave_shape_saw(pfloat_t phase,
  pfloat_t phase_inc,
  pfloat_t inv_phase_inc) __attribute__((const));
pfloat_t wave_shape_triangle(pfloat_t phase,
  pfloat_t phase_inc,
  pfloat_t inv_phase_inc,
  pfloat_t last_level) __attribute__((const));
pfloat_t wave_shape_pulse(pfloat_t phase,
  pfloat_t phase_inc,
  pfloat_t inv_phase_inc,
  pfloat_t duty_phase) __attribute__((const));
pfloat_t wave_shape_variable(const pfloat_t phase,
  const pfloat_t phase_inc,
  const pfloat_t inv_phase_inc,
  const pfloat_t freq,
  const pfloat_t variable) __attribute__((const));

#ifdef __cplusplus
}
#endif

#endif  // LIB_DSP_WAVE_SHAPE_H_
