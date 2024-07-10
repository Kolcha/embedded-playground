#ifndef __FV935ND_H__
#define __FV935ND_H__

#include <stdint.h>
#include <stdbool.h>

#include "fv935nd_wiring.h"
#include "vfd_display.h"

static const uint8_t fv935nd_seg_pins[] = {
  FV935ND_SEG_A,
  FV935ND_SEG_B,
  FV935ND_SEG_C,
  FV935ND_SEG_D,
  FV935ND_SEG_E,
  FV935ND_SEG_F,
  FV935ND_SEG_G1,
  FV935ND_SEG_G2,
  FV935ND_SEG_H,
  FV935ND_SEG_IL,
  FV935ND_SEG_J,
  FV935ND_SEG_K,
  FV935ND_SEG_M,
  FV935ND_SEG_DOTS,
};

static const vfd_pinout fv935nd_pinout = {
  .grids_count = 8,
  .segments_count = 14,
  .seg_pins = fv935nd_seg_pins,
};

static const char fv935nd_supported_chars[] = {
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
  'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
  'U', 'V', 'W', 'X', 'Y', 'Z',
  ' ', '$', '%', '&', '(', ')', '*', '-', '/', '\\',
  '=', '^', '_', '`', '|',
};

static const uint16_t fv935nd_font_data[] = {
  0x0C3F, 0x0006, 0x00DB, 0x00CF, 0x00E6,
  0x00ED, 0x00FD, 0x0027, 0x00FF, 0x00EF,
  0x00F7, 0x028F, 0x0039, 0x020F, 0x00F9,
  0x00F1, 0x00BD, 0x00F6, 0x0209, 0x001E,
  0x1470, 0x0038, 0x0237, 0x1136, 0x003F,
  0x00F3, 0x103F, 0x10F3, 0x11ED, 0x0201,
  0x003E, 0x0C30, 0x023E, 0x1D00, 0x00EE,
  0x0C09,
  0x0000, 0x13ED, 0x1DE4, 0x1D0D, 0x1400,
  0x0900, 0x1FC0, 0x00C0, 0x0C00, 0x1100,
  0x00C8, 0x0120, 0x0008, 0x0100, 0x0200,
};

static const vfd_font fv935nd_font = {
  .chars_count = sizeof(fv935nd_supported_chars),
  .data = fv935nd_font_data,
  .supported_chars = fv935nd_supported_chars,
};

// display-specific stuff
// degree sign after the second digit
void set_degree_sign_state(const vfd_display* disp, bool on,
                           uint8_t* buf, uint8_t sz);
// decimal point after the 6th digit
void set_decimal_point_state(const vfd_display* disp, bool on,
                             uint8_t* buf, uint8_t sz);
// colons before the 3rd, 5th, and 7th digits
// index starts from 0, available indexes are 0, 1, 2
void set_colon_state(const vfd_display* disp, int idx, bool on,
                     uint8_t* buf, uint8_t sz);

#endif /* __FV935ND_H__ */
