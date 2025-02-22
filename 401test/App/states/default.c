#include "default.h"

#include <string.h>

#include "fonts.h"

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

  static const int ticks_on = 15;
  static const int ticks_off = 15;
  static int ticks_to_wait = ticks_off;
  static int ticks_counter = 0;
  if (++ticks_counter == ticks_to_wait) {
    ticks_counter = 0;
    ticks_to_wait = ticks_to_wait == ticks_off ? ticks_on : ticks_off;
    bw = !bw;
  }

  draw_text(&rctx, 48, 4, "Test\nText\nLine");
}

void state_default_button(state_data_t* data)
{
  sm_switch_to_next_state(data->state_machine);
}
