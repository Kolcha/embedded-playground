#ifndef __APP_STATE_MACHINE_H__
#define __APP_STATE_MACHINE_H__

#include "display.h"
#include "state_machine.h"

struct sm_state_api {
  void(*render)(state_data_t*, const display_t*);

  void(*button_m)(state_data_t*);
  void(*button_1)(state_data_t*);
  void(*button_2)(state_data_t*);

  void(*encoder_value)(state_data_t*, uint32_t);
};

extern state_data_t app_state_data;
extern struct state_machine app_sm;

void app_state_machine_init();

#endif /* __APP_STATE_MACHINE_H__ */
