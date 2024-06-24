#include "gpio_button.h"

#define CALL_HANDLER(handler)   if (handler) handler();

GPIO_ButtonState GPIO_ButtonCheckState(GPIO_Button* btn)
{
  GPIO_ButtonState button_state = GPIO_BUTTON_NOT_PRESSED;

  if (HAL_GPIO_ReadPin(btn->GPIOx, btn->GPIO_pin) == btn->pressed_state) {
    uint32_t now = HAL_GetTick();
    if (btn->_press_started == 0) {
      btn->_press_started = now;
    }
    uint32_t time_passed_ms = now - btn->_press_started;

    if (btn->long_press) {
      // avoid action repeat after button release
      if (btn->_long_press_handled) {
        btn->_press_started = 0;
      }
      // long press
      if (!btn->_long_press_handled && time_passed_ms >= btn->long_press_ms) {
        btn->_long_press_handled = 1;
        button_state = GPIO_BUTTON_LONG_PRESS;
        CALL_HANDLER(btn->long_press_handler);
      }
    }
    if (btn->repeatable) {
      // delay before starting repeat
      if (!btn->_repeating && time_passed_ms >= btn->before_repeat_ms) {
        btn->_repeating = 1;
      }
      // delay between repeats
      if (btn->_repeating && time_passed_ms >= btn->repeat_interval_ms) {
        btn->_press_started = now;
        button_state = GPIO_BUTTON_PRESSED;
        CALL_HANDLER(btn->click_handler);
      }
    }
  } else {
    bool was_pressed = btn->_press_started != 0;
    uint32_t time_passed_ms = HAL_GetTick() - btn->_press_started;
    // single click
    if (was_pressed && !btn->_repeating && time_passed_ms >= btn->debounce_ms) {
      button_state = GPIO_BUTTON_PRESSED;
      CALL_HANDLER(btn->click_handler);
    }
    btn->_long_press_handled = 0;
    btn->_repeating = 0;
    btn->_press_started = 0;
  }

  return button_state;
}
