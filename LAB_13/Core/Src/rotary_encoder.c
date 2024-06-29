#include "rotary_encoder.h"

void RotaryEncoderInit(RotaryEncoder* enc, TIM_TypeDef* timer)
{
  enc->value_changed_handler = NULL;
  enc->timer = timer;
  enc->delta = timer->ARR / 3;
  enc->_last_value = timer->CNT;
}

uint32_t RotaryEncoderGetValue(RotaryEncoder* enc)
{
  uint32_t curr_value = enc->timer->CNT;
  uint32_t max_value = enc->timer->ARR;

  if (enc->_last_value < enc->delta && max_value - curr_value < enc->delta) {
    enc->timer->CNT = 0;
    curr_value = 0;
  }

  if (max_value - enc->_last_value < enc->delta && curr_value < enc->delta) {
    enc->timer->CNT = max_value;
    curr_value = max_value;
  }

  if (enc->_last_value != curr_value) {
    enc->_last_value = curr_value;
    if (enc->value_changed_handler) enc->value_changed_handler(curr_value);
  }

  return curr_value;
}
