// Copyright 2016 Will Stockwell

#ifndef LIB_DSP_WAVE_TABLE_H_
#define LIB_DSP_WAVE_TABLE_H_

#include <conf/arch_conf.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  wave_table_shape_sin,
  wave_table_shape_tri,
  wave_table_shape_sqr,
  wave_table_shape_saw
} wave_table_shape_t;

// should be called once per change to oscillator frequency
uint16_t wave_table_index(const pfloat_t f) __attribute__((pure));

// should be called once per change to phase
uint16_t wave_table_pos(
  const pfloat_t phase,
  volatile pfloat_t *p) __attribute__((pure));

pfloat_t wave_table_lookup(
  const wave_table_shape_t shape,
  const uint16_t tab_index,
  const uint16_t tab_pos,
  const pfloat_t p) __attribute__((const));

#ifdef __cplusplus
}
#endif

#endif  // LIB_DSP_WAVE_TABLE_H_
