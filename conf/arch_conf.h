// Copyright 2016 Will Stockwell

#ifndef CONF_ARCH_CONF_H_
#define CONF_ARCH_CONF_H_

#ifndef PI
  #ifndef X86
    #ifdef STM32F7XX
      #include <stm32f7xx_hal_conf.h>
    #else
      #include <stm32f37x_conf.h>
    #endif
  #else
    #include <conf/x86_conf.h>
  #endif
#else 
typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;
#endif

#ifdef MATH_USE_DOUBLE
  typedef double pfloat_t;
#else
  typedef float pfloat_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

// User-defined handler available to drivers to indicate some kind of
// unrecoverable issue.
void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif  // CONF_ARCH_CONF_H_
