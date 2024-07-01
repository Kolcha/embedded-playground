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
  // internal state
  uint32_t last_value;
} RotaryEncoder;

// convenient macro for rotary encoder declaration
#define ROTARY_ENCODER(Timer)                     \
{                                                 \
  .timer = Timer,                                 \
  .last_value = 0,                                \
}

// should be called periodically, often as possible
uint32_t RotaryEncoderGetValue(RotaryEncoder* enc);

#endif /* __ROTARY_ENCODER_H__ */
