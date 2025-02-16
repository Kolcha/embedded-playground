#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <stdbool.h>
#include <stdint.h>

// monochome display definition
// w & h must be dividable by 8
// vertical addressing assumed
typedef struct {
  uint8_t w;            // display width
  uint8_t h;            // display height
  uint16_t buf_sz;      // framebuffer size
  uint8_t* buf;         // framebuffer data
} display_t;

// set single pixel at (x,y) on or off
void display_set_pixel(const display_t* disp, uint8_t x, uint8_t y, bool on);

// bitmap font definition
// line-by-line MSB-first
typedef struct {
  uint8_t w;            // character width
  uint8_t h;            // character height
  uint16_t n;           // characters count
  uint16_t csz;         // character size
  const uint8_t* data;  // characters data
} bmp_font_t;

// rendering context
typedef struct {
  const display_t* disp;
  const bmp_font_t* font;
} render_context_t;

// draw given char at (x,y)
// char's (0,0) is top-left
void draw_char(const render_context_t* ctx, int x, int y, uint16_t c);
// draw given text at (x,y)
// text's (0,0) is top-left
// text must be null-terminated
// new line in text is supported
void draw_text(const render_context_t* ctx, int x, int y, const char* text);

#endif /* __DISPLAY_H__ */
