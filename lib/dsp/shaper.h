// Copyright 2016 Will Stockwell

#ifndef LIB_DSP_SHAPER_H_
#define LIB_DSP_SHAPER_H_

#ifdef __cplusplus
extern "C" {
#endif

float clip(float x) __attribute__((const));
float soft_clip(float x) __attribute__((const));
float cubic_shape(float x) __attribute__((const));

#ifdef __cplusplus
}
#endif

#endif  // LIB_DSP_SHAPER_H_
