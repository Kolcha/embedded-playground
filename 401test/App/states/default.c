#include "default.h"

#include <string.h>

#include "fonts/lat1-08.h"

static const bmp_font_t lat1_08 = {
  .w = FONT_CHAR_W_LAT1_08,
  .h = FONT_CHAR_H_LAT1_08,
  .n = FONT_LENGTH_LAT1_08,
  .csz = FONT_CHARSZ_LAT1_08,
  .data = font_data_lat1_08,
};

void state_default_on_enter(state_data_t* data)
{
}

void state_default_on_leave(state_data_t* data)
{
}

void state_default_render(state_data_t* data, const display_t* disp)
{
  const render_context_t rctx = {
    .disp = disp,
    .font = &lat1_08,
  };

  static bool bw = true;
  memset(disp->buf, bw ? 0x00 : 0xFF, disp->buf_sz);
  disp->buf[0] = bw ? 0x01 : 0xFE;
  disp->buf[3] = bw ? 0x80 : 0x7F;
  disp->buf[508] = disp->buf[0];
  disp->buf[511] = disp->buf[3];
  bw = !bw;

  draw_text(&rctx, 48, 4, "Test\nText\nLine");
}

void state_default_button(state_data_t* data)
{
}
