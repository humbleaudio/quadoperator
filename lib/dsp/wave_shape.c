// Copyright 2016 Will Stockwell
//
// PolyBLEP-based wave shape synthesis derived from
// www.martin-finke.de/blog/articles/audio-plugins-018-polyblep-oscillator/

#include "lib/dsp/wave_shape.h"

#include "lib/dsp/poly_blep.h"
#include "lib/dsp/wave_table.h"
#include "lib/math/math.h"

pfloat_t wave_shape_saw(pfloat_t phase,
  pfloat_t phase_inc,
  pfloat_t inv_phase_inc) {
  return -1.0f * phase + poly_blep(phase, phase_inc, inv_phase_inc);
}

// TODO(will): find a more efficieint direct, band-limited triangle impl.
pfloat_t wave_shape_triangle(pfloat_t phase,
  pfloat_t phase_inc,
  pfloat_t inv_phase_inc,
  pfloat_t last_level) {
  // do in the square in -1 to 1 closer to PolyBLEP article
  // referenced at the top of file, then scale for approximate
  // -1 to 1 result for the triangle shape
  pfloat_t level = wave_shape_pulse(phase, phase_inc, inv_phase_inc, 0.0f);

  // Integration below causes wave shape to lose magnitude.
  // Scale the square to compensate, achieve approximately -1 to 1 range
  level *= 2.16f;

  // Integrate to achieve triangle shape
  level = phase_inc * level + (1.0f - phase_inc) * last_level;

  // early renders exceed 1.0 slightly until accumulator fills
  // in properly. transient issue, so just clip it
  // return clip(level);
  return level;
}

pfloat_t wave_shape_pulse(pfloat_t phase,
  pfloat_t phase_inc,
  pfloat_t inv_phase_inc,
  pfloat_t duty_phase) {
  pfloat_t level = phase <= duty_phase ? 1.0f : -1.0f;

  level += poly_blep(phase, phase_inc, inv_phase_inc);
  level -= poly_blep(wrap(phase - 1.0f - duty_phase), phase_inc, inv_phase_inc);

  // TODO(will): slight ripple causes some clipping
  return level;
}

const wave_table_shape_t var_shapes[4][2] = {
  // {left, right}
  {wave_table_shape_sin, wave_table_shape_tri},
  {wave_table_shape_tri, wave_table_shape_sqr},
  {wave_table_shape_sqr, wave_table_shape_saw},
  // redundant entry for var = 1.0 case:
  {wave_table_shape_sqr, wave_table_shape_saw}
};

const pfloat_t var_offset[4] = {
  0.0f,
  -1.0f,
  -2.0f,
  -2.0f  // redundant entry for var = 1.0 case
};

pfloat_t wave_shape_variable(const pfloat_t phase,
  const pfloat_t phase_inc,
  const pfloat_t inv_phase_inc,
  const pfloat_t freq,
  const pfloat_t var) {
  pfloat_t amt = var * 3.0f;
  uint16_t pair_i = amt;
  amt += var_offset[pair_i];

  uint16_t i = wave_table_index(freq);

  pfloat_t p;
  uint16_t sub_i = wave_table_pos(phase, &p);

  return (1.0f - amt) * wave_table_lookup(var_shapes[pair_i][0], i, sub_i, p)
         + amt * wave_table_lookup(var_shapes[pair_i][1], i, sub_i, p);
}
