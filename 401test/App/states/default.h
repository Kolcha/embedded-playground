#ifndef __STATE_DEFAULT_H__
#define __STATE_DEFAULT_H__

#include "display.h"
#include "state_machine.h"

void state_default_on_enter(state_data_t* data);
void state_default_on_leave(state_data_t* data);

void state_default_render(state_data_t* data, const display_t* disp);
void state_default_button(state_data_t* data);

#endif /* __STATE_DEFAULT_H__ */
