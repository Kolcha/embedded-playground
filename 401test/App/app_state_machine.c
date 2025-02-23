#include "app_state_machine.h"

#include <stddef.h>

#include "states/default.h"
#include "states/state2.h"
#include "states/state3.h"

static struct sm_state_api const state_default_api = {
  .render = &state_default_render,

  .button_m = &state_default_button_m,
  .button_1 = &state_default_button_1,
  .button_2 = &state_default_button_2,
};

static struct sm_state_api const state_state2_api = {
  .render = &state_state2_render,

  .button_m = &state_state2_button_m,
  .button_1 = &state_state2_button_1,
  .button_2 = &state_state2_button_2,
};

static struct sm_state_api const state_state3_api = {
  .render = &state_state3_render,

  .button_m = &state_state3_button_m,
  .button_1 = &state_state3_button_1,
  .button_2 = &state_state3_button_2,
};

static struct sm_state const sm_all_states[] = {
  {
    .on_state_enter = &state_default_on_enter,
    .on_state_leave = &state_default_on_leave,

    .api = &state_default_api,
  },

  {
    .on_state_enter = &state_state2_on_enter,
    .on_state_leave = &state_state2_on_leave,

    .api = &state_state2_api,
  },

  {
    .on_state_enter = &state_state3_on_enter,
    .on_state_leave = &state_state3_on_leave,

    .api = &state_state3_api,
  },
};

static struct sm_transition const sm_all_transitions[] = {
  {
    .prev_state = NULL,
    .next_state = &sm_all_states[1],
  },
  {
    .prev_state = &sm_all_states[0],
    .next_state = &sm_all_states[2],
  },
  {
    .prev_state = &sm_all_states[1],
    .next_state = &sm_all_states[0],
  },
};

state_data_t app_state_data;

struct state_machine app_sm = {
  .curr_state = &sm_all_states[0],
  .state_data = &app_state_data,

  .all_states = sm_all_states,
  .transitions = sm_all_transitions,
};

void app_state_machine_init()
{
  app_state_data.state_machine = &app_sm;
  state_default_on_enter(&app_state_data);
}
