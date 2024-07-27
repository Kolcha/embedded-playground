#include "esp32-hal-rgb-led.h"

#define WS2812_OUT_PIN      13

static const uint8_t colors[] = {
  0xFF, 0x00, 0x00,
  0xFF, 0xFF, 0x00,
  0x00, 0xFF, 0x00,
  0x00, 0xFF, 0xFF,
  0x00, 0x00, 0xFF,
  0xFF, 0x00, 0xFF,
};

static const size_t colors_count = sizeof(colors) / 3;

static const size_t max_leds = 5;

size_t color_idx = 0;
size_t active_led_idx = 0;

void setup() {
  pinMode(WS2812_OUT_PIN, OUTPUT);
}

void loop() {
  for (size_t i = 0; i < max_leds; i++) {
    if (i == active_led_idx) {
      uint8_t r = colors[3*color_idx + 0];
      uint8_t g = colors[3*color_idx + 1];
      uint8_t b = colors[3*color_idx + 2];
      neopixelWrite(WS2812_OUT_PIN, r, g, b);
    } else {
      neopixelWrite(WS2812_OUT_PIN, 0, 0, 0);
    }
  }

  delay(250);

  if (++active_led_idx == max_leds) {
    if (++color_idx == colors_count) {
      color_idx = 0;
    }
    active_led_idx = 0;
  }
}
