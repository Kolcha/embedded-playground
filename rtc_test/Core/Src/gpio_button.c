#include "gpio_button.h"

#define CALL_HANDLER(handler)   if (handler) handler();

GPIO_ButtonState GPIO_ButtonCheckState(GPIO_Button* btn)
{
  GPIO_ButtonState button_state = GPIO_BUTTON_NOT_PRESSED;

  uint32_t now = HAL_GetTick();
  if (HAL_GPIO_ReadPin(btn->GPIOx, btn->GPIO_pin) == btn->pressed_state) {
    if (btn->_last_pressed == 0) {
      btn->_last_pressed = now;
    }
    uint32_t time_passed_ms = now - btn->_last_pressed;

    if ((btn->mode & GPIO_BUTTON_MODE_MASK) == GPIO_BUTTON_MODE_LONG_PRESS) {
      // avoid action repeat after button release
      if (btn->_long_press_handled) {
        btn->_last_pressed = 0;
      }
      // long press
      if (!btn->_long_press_handled && time_passed_ms >= btn->long_press_ms) {
        btn->_long_press_handled = 1;
        button_state = GPIO_BUTTON_LONG_PRESS;
        CALL_HANDLER(btn->long_press_handler);
      }
    }
    if ((btn->mode & GPIO_BUTTON_MODE_MASK) == GPIO_BUTTON_MODE_REPEAT) {
      // delay before starting repeat
      if (!btn->_repeating && time_passed_ms >= btn->before_repeat_ms) {
        btn->_repeating = 1;
      }
      // delay between repeats
      if (btn->_repeating && time_passed_ms >= btn->repeat_interval_ms) {
        btn->_last_pressed = now;
        button_state = GPIO_BUTTON_PRESSED;
        CALL_HANDLER(btn->click_handler);
      }
    }
  } else {
    bool was_pressed = btn->_last_pressed != 0;
    bool dbl_click_enabled = (btn->mode& GPIO_BUTTON_FLAG_DBL_CLICK) != 0;
    uint32_t time_passed_ms = now - btn->_last_pressed;
    if (was_pressed && !btn->_repeating && time_passed_ms >= btn->debounce_ms) {
      if (dbl_click_enabled) {
        // double click
        btn->_last_released = now;
        btn->_n_clicks++;
        if (btn->_n_clicks == 2) {
          btn->_n_clicks = 0;
          btn->_last_released = 0;
          button_state = GPIO_BUTTON_DOUBLE_CLICK;
          CALL_HANDLER(btn->double_click_handler);
        }
      } else {
        // single click
        button_state = GPIO_BUTTON_PRESSED;
        CALL_HANDLER(btn->click_handler);
      }
    }
    // double click timeout
    if (!was_pressed && dbl_click_enabled && btn->_last_released != 0) {
      uint32_t since_last_release = now - btn->_last_released;
      if (since_last_release >= 250) {
        button_state = GPIO_BUTTON_PRESSED;
        btn->_n_clicks = 0;
        btn->_last_released = 0;
        CALL_HANDLER(btn->click_handler);
      }
    }
    btn->_long_press_handled = 0;
    btn->_repeating = 0;
    btn->_last_pressed = 0;
  }

  return button_state;
}
