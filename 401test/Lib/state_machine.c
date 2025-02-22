#include "state_machine.h"

static const struct sm_transition* transition(struct state_machine* sm)
{
  return sm->transitions + (sm->curr_state - sm->all_states);
}

static void sm_switch_to_state(struct state_machine* sm,
                               const struct sm_state* st)
{
  if (sm->curr_state->on_state_leave)
    (*sm->curr_state->on_state_leave)(sm->state_data);

  sm->curr_state = st;
  sm->state_data->state_machine = sm;

  if (sm->curr_state->on_state_enter)
    (*sm->curr_state->on_state_enter)(sm->state_data);
}

void sm_switch_to_prev_state(struct state_machine* sm)
{
  const struct sm_state* st;
  if ((st = transition(sm)->prev_state))
    sm_switch_to_state(sm, st);
}

void sm_switch_to_next_state(struct state_machine* sm)
{
  const struct sm_state* st;
  if ((st = transition(sm)->next_state))
    sm_switch_to_state(sm, st);
}
