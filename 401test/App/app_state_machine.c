#include "app_state_machine.h"

#include <stddef.h>

#include "states/default.h"

static struct sm_state_api const state_default_api = {
  .render = &state_default_render,
  .button = &state_default_button,
};

static struct sm_state const sm_all_states[] = {
  {
    .on_state_enter = &state_default_on_enter,
    .on_state_leave = &state_default_on_leave,

    .api = &state_default_api,
  },
};

static struct sm_transition const sm_all_transitions[] = {
  {
    .prev_state = NULL,
    .next_state = NULL,
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
