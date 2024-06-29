#ifndef __ROTARY_ENCODER_H__
#define __ROTARY_ENCODER_H__

#include <stdint.h>

// timer definitions
#include "main.h"

typedef struct {
  // handlers
  void(*value_changed_handler)(uint32_t);
  // configuration
  TIM_TypeDef* timer;
  uint32_t delta;
  // internal state
  uint32_t _last_value;
} RotaryEncoder;

// should be called each time timer parameters change
// configuration adjustments should be done after this call
void RotaryEncoderInit(RotaryEncoder* enc, TIM_TypeDef* timer);

// should be called periodically, often as possible
uint32_t RotaryEncoderGetValue(RotaryEncoder* enc);

#endif /* __ROTARY_ENCODER_H__ */
