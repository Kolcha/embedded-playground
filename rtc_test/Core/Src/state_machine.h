#ifndef __STATE_MACHINE_H__
#define __STATE_MACHINE_H__

typedef struct _ClockState {
  void(*enter_state)();
  void(*leave_state)();
  void(*timer_event)(const struct _ClockState**);
  void(*short_press)(const struct _ClockState**);
  void(*long_press)(const struct _ClockState**);
  void(*double_click)(const struct _ClockState**);
} ClockState;

extern const ClockState state_machine_initial_state;

#endif /* __STATE_MACHINE_H__ */
