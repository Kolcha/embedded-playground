#include "state3.h"

#include <stdio.h>
#include <string.h>

#include "fonts.h"

static int number = 0;

void state_state3_on_enter(state_data_t* data)
{
}

void state_state3_on_leave(state_data_t* data)
{
}

void state_state3_render(state_data_t* data, const display_t* disp)
{
  char str[8];

  const render_context_t rctx = {
    .disp = disp,
    .font = &lat2_terminus16,
  };

  snprintf(str, sizeof(str), "%+04d", number);

  memset(disp->buf, 0x00, disp->buf_sz);

  draw_text(&rctx, 48, 8, str);
}

void state_state3_button_m(state_data_t* data)
{
  sm_switch_to_next_state(data->state_machine);
}

void state_state3_button_1(state_data_t* data)
{
  if (--number < -999)
    number = -999;
}

void state_state3_button_2(state_data_t* data)
{
  if (++number > 999)
    number = 999;
}

void state_state3_encoder_value(state_data_t* data, uint32_t val)
{
  number = val;
}
