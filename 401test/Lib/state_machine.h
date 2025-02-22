#ifndef __STATE_MACHINE_H__
#define __STATE_MACHINE_H__

typedef struct {
  void* state_machine;
  void* user_data;
} state_data_t;

struct sm_state_api;

struct sm_state {
  void(*on_state_enter)(state_data_t*);
  void(*on_state_leave)(state_data_t*);

  struct sm_state_api const* api;
};


struct sm_transition {
  const struct sm_state* prev_state;
  const struct sm_state* next_state;
};

struct state_machine {
  const struct sm_state* curr_state;
  state_data_t* state_data;

  // mapping "state - transition", sizes must be equal
  const struct sm_state* all_states;
  const struct sm_transition* transitions;
};

void sm_switch_to_prev_state(struct state_machine* sm);
void sm_switch_to_next_state(struct state_machine* sm);

#endif /* __STATE_MACHINE_H__ */
