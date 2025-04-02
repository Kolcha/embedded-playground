#ifndef __STATE_STATE3_H__
#define __STATE_STATE3_H__

#include "display.h"
#include "state_machine.h"

void state_state3_on_enter(state_data_t* data);
void state_state3_on_leave(state_data_t* data);

void state_state3_render(state_data_t* data, const display_t* disp);

void state_state3_button_m(state_data_t* data);
void state_state3_button_1(state_data_t* data);
void state_state3_button_2(state_data_t* data);

void state_state3_encoder_value(state_data_t* data, uint32_t val);

#endif /* __STATE_STATE3_H__ */
