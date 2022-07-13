// Copyright 2016 Will Stockwell

#include <conf/arch_conf.h>

#include "lib/dsp/wave_table.h"
#include "lib/dsp/wave_table_resource.h"

uint16_t wave_table_index(const pfloat_t f) {
  // Determine sub table by frequency
  // TODO(will): do I want more subtables on the low end?
  if (f < 122.0f) {
    return 0;
  } else if (f < 176.0) {
    return 1;
  } else if (f < 254.0f) {
    return 2;
  } else if (f < 367.0f) {
    return 3;
  } else if (f < 531.0f) {
    return 4;
  } else if (f < 766.0f) {
    return 5;
  } else if (f < 1107.0f) {
    return 6;
  } else if (f < 1599.0f) {
    return 7;
  } else if (f < 2309.0f) {
    return 8;
  } else if (f < 3336.0f) {
    return 9;
  } else if (f < 4818.0f) {
    return 10;
  } else if (f < 6960.0f) {
    return 11;
  } else if (f < 10053.0f) {
    return 12;
  } else {
    return 13;
  }
}

uint16_t wave_table_pos(const pfloat_t phase, volatile pfloat_t *p) {
  pfloat_t tab_phase = (phase + 1.0f) * 0.5f;
  pfloat_t tab_pos = tab_phase * kSubTableLen;
  uint16_t index = (uint16_t)tab_pos;

  *p = tab_pos - (pfloat_t)index;  // NOLINT(readability/casting)
  return index;
}

pfloat_t wave_table_lookup(
  const wave_table_shape_t shape,
  const uint16_t tab_index,
  const uint16_t tab_pos,
  const pfloat_t p
  ) {
  const pfloat_t *sub_table = wave_table[shape][tab_index];

  return (1.0f - p) * sub_table[tab_pos] + p * sub_table[tab_pos+1];
}
