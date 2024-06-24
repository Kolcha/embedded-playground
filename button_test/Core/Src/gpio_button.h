#ifndef __GPIO_BUTTON_H__
#define __GPIO_BUTTON_H__

#include <stdbool.h>
#include <stdint.h>

// GPIO definitions
#include "main.h"

typedef enum {
  GPIO_BUTTON_NOT_PRESSED,
  GPIO_BUTTON_PRESSED,
  GPIO_BUTTON_LONG_PRESS,
} GPIO_ButtonState;

typedef struct {
  // handlers
  void(*click_handler)();
  void(*long_press_handler)();
  // configuration
  GPIO_TypeDef* GPIOx;
  uint16_t GPIO_pin;
  bool repeatable;
  bool long_press;
  GPIO_PinState pressed_state;
  // timeouts
  uint8_t debounce_ms;
  uint8_t repeat_interval_ms;
  uint16_t before_repeat_ms;
  uint16_t long_press_ms;
  // internal state
  bool _repeating;
  bool _long_press_handled;
  uint32_t _press_started;
} GPIO_Button;

// convenient macro for buttons declaration
#define GPIO_BUTTON(GPIO_Port, GPIO_Pin, GPIO_State)    \
{                                                       \
  .GPIOx = GPIO_Port,                                   \
  .GPIO_pin = GPIO_Pin,                                 \
  .pressed_state = GPIO_State,                          \
  .debounce_ms = 30,                                    \
  .before_repeat_ms = 300,                              \
  .repeat_interval_ms = 80,                             \
  .long_press_ms = 2000,                                \
}

// should be called periodically, often as possible
GPIO_ButtonState GPIO_ButtonCheckState(GPIO_Button* btn);

#endif /* __GPIO_BUTTON_H__ */
