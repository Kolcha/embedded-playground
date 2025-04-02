#include "rotary_encoder.h"

uint32_t RotaryEncoderGetValue(RotaryEncoder* enc)
{
  uint32_t curr_value = enc->timer->CNT;

  if (curr_value == enc->last_value) {
    return curr_value;
  }

  uint32_t max_value = enc->timer->ARR;
  uint32_t delta = enc->timer->ARR / 3;

  if (enc->last_value < delta && max_value - curr_value < delta) {
    enc->timer->CNT = 0;
    curr_value = 0;
  }

  if (max_value - enc->last_value < delta && curr_value < delta) {
    enc->timer->CNT = max_value;
    curr_value = max_value;
  }

  if (enc->last_value != curr_value) {
    enc->last_value = curr_value;
    if (enc->value_changed_handler) enc->value_changed_handler(curr_value);
  }

  return curr_value;
}
