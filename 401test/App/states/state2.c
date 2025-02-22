#include "state2.h"

#include <string.h>

#include "fonts.h"

void state_state2_on_enter(state_data_t* data)
{
}

void state_state2_on_leave(state_data_t* data)
{
}

void state_state2_render(state_data_t* data, const display_t* disp)
{
  const render_context_t rctx = {
    .disp = disp,
    .font = &lat2_terminus16,
  };

  memset(disp->buf, 0x00, disp->buf_sz);

  draw_text(&rctx, 8, 8, "Test Text Line");
}

void state_state2_button(state_data_t* data)
{
  sm_switch_to_prev_state(data->state_machine);
}
