#ifndef __VFD_DISPLAY_H__
#define __VFD_DISPLAY_H__

#include "vfd_controller.h"

// seg_pins is an array of controller's pins definitions
typedef struct {
  uint8_t grids_count;
  uint8_t segments_count;
  const uint8_t* seg_pins;
} vfd_pinout;

// bits in font data values must follow seg_pins order
// supported_chars and fonts data must be in the same order
typedef struct {
  uint8_t chars_count;
  const uint16_t* data;
  const char* supported_chars;
} vfd_font;

// returns pointer to value in data, NULL if no character found
const uint16_t* vfd_font_get_char(const vfd_font* fnt, char c);

typedef struct {
  const vfd_pinout* pinout;
  const vfd_controller* controller;
  const vfd_font* font;
} vfd_display;

// doesn't send data to controller, does only in-memory change
void vfd_write_segments(const vfd_display* disp,
                        uint8_t d, uint16_t segs,
                        uint8_t* buf, uint8_t sz);
void vfd_write_char(const vfd_display* disp, uint8_t d, char c,
                    uint8_t* buf, uint8_t sz);

#endif /* __VFD_DISPLAY_H__ */
