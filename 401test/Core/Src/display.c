#include "display.h"

// ------------------------------------
// protected interface
// ------------------------------------

// font glyph definition
typedef struct {
  const bmp_font_t* font;
  const uint8_t* data;
} font_glyph_t;

bool font_get_glyph(const bmp_font_t* font, uint16_t ch, font_glyph_t* g);

// ------------------------------------
// implementation
// ------------------------------------
#include <assert.h>

void display_set_pixel(const display_t* disp, uint8_t x, uint8_t y, bool en)
{
  if (x >= disp->w || y >= disp->h)
    return;

  // assume vertically addressed display (i.e. by column, top to bottom)
  unsigned int i = (unsigned int)x * disp->h / 8 + y / 8;
  assert(i < disp->buf_sz);
  unsigned char m = 1U << (y % 8);

  if (en)
    disp->buf[i] |= m;
  else
    disp->buf[i] &= ~m;
}

bool font_get_glyph(const bmp_font_t* font, uint16_t ch, font_glyph_t* g)
{
  if (ch >= font->n)
    ch = 0;                 // return the first character if out-of-range

  g->font = font;
  g->data = font->data + ch * font->csz;

  return true;
}

static void draw_glyph(const display_t* disp,
                       int x, int y,
                       const font_glyph_t* glyph)
{
  assert(glyph->data);
  // fonts addressed horizontally
  unsigned int bpr = glyph->font->csz / glyph->font->h;

  for (uint8_t r = 0; r < glyph->font->h; r++) {
    int ty = y + r;
    if (ty < 0)
      continue;
    if (ty >= (int)disp->h)
      break;

    for (uint8_t c = 0; c < glyph->font->w; c++) {
      int tx = x + c;
      if (tx < 0)
        continue;
      if (tx >= (int)disp->w)
        break;

      unsigned int i = (unsigned int)r * bpr + c / 8;
      assert(i < glyph->font->csz);
      assert(0 <= tx && tx < (int)disp->w);
      assert(0 <= ty && ty < (int)disp->h);

      unsigned char m = 1U << (8 - (c % 8));
      bool en = (glyph->data[i] & m) != 0;
      display_set_pixel(disp, tx, ty, en);
    }
  }
}

static void draw_char(const render_context_t* ctx,
                      int x, int y, uint16_t c)
{
  font_glyph_t g;
  font_get_glyph(ctx->font, c, &g);
  draw_glyph(ctx->disp, x, y, &g);
}

void draw_text(const render_context_t* ctx, int x, int y, const char* text)
{
  int cx = x;
  int cy = y;

  const char* c = text;
  while (*c != '\0') {
    if (*c == '\n') {
      cx = x;
      cy += ctx->font->h;
    } else {
      draw_char(ctx, cx, cy, *c);
      cx += ctx->font->w;
    }
    c++;
  }
}
