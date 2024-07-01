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
  GPIO_BUTTON_DOUBLE_CLICK,
} GPIO_ButtonState;

typedef enum {
  GPIO_BUTTON_MODE_SIMPLE               = 0x00,
  GPIO_BUTTON_MODE_LONG_PRESS           = 0x04,
  GPIO_BUTTON_MODE_REPEAT               = 0x08,
} GPIO_ButtonMode;

typedef enum {
  GPIO_BUTTON_FLAG_DBL_CLICK            = 0x10,
} GPIO_ButtonModeFlags;

#define GPIO_BUTTON_MODE_MASK           0x0C
#define GPIO_BUTTON_FLAGS_MASK          0XF0

typedef struct {
  // handlers
  void(*click_handler)();
  void(*long_press_handler)();
  void(*double_click_handler)();
  // configuration
  GPIO_PinState pressed_state;
  GPIO_TypeDef* GPIOx;
  uint16_t GPIO_pin;
  uint8_t mode; // GPIO_ButtonMode | GPIO_ButtonModeFlags
  // timeouts
  uint8_t debounce_ms;
  uint16_t long_press_ms;
  uint16_t before_repeat_ms;
  uint8_t repeat_interval_ms;
  // internal state
  bool _repeating;
  bool _long_press_handled;
  uint8_t _n_clicks;
  uint32_t _last_pressed;
  uint32_t _last_released;
} GPIO_Button;

// convenient macro for buttons declaration
#define GPIO_BUTTON(GPIO_Port, GPIO_Pin, GPIO_State)    \
{                                                       \
  .click_handler = NULL,                                \
  .long_press_handler = NULL,                           \
  .double_click_handler = NULL,                         \
  .GPIOx = GPIO_Port,                                   \
  .GPIO_pin = GPIO_Pin,                                 \
  .pressed_state = GPIO_State,                          \
  .mode = GPIO_BUTTON_MODE_SIMPLE,                      \
  .debounce_ms = 30,                                    \
  .before_repeat_ms = 300,                              \
  .repeat_interval_ms = 80,                             \
  .long_press_ms = 2000,                                \
  ._repeating = false,                                  \
  ._long_press_handled = false,                         \
  ._n_clicks = 0,                                       \
  ._last_pressed = 0,                                   \
  ._last_released = 0,                                  \
}

// should be called periodically, often as possible
GPIO_ButtonState GPIO_ButtonCheckState(GPIO_Button* btn);

#endif /* __GPIO_BUTTON_H__ */
