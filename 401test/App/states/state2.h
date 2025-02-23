#ifndef __STATE_STATE2_H__
#define __STATE_STATE2_H__

#include "display.h"
#include "state_machine.h"

void state_state2_on_enter(state_data_t* data);
void state_state2_on_leave(state_data_t* data);

void state_state2_render(state_data_t* data, const display_t* disp);

void state_state2_button_m(state_data_t* data);
void state_state2_button_1(state_data_t* data);
void state_state2_button_2(state_data_t* data);

#endif /* __STATE_STATE2_H__ */
