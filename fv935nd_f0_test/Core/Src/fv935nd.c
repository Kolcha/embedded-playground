#include "fv935nd.h"

void set_degree_sign_state(const vfd_display* disp, bool on,
                           uint8_t* buf, uint8_t sz)
{
  if (on) {
    vfd_enable_segment(disp->controller, buf, sz, 2, FV935ND_SEG_DOTS);
  } else {
    vfd_disable_segment(disp->controller, buf, sz, 2, FV935ND_SEG_DOTS);
  }
}

void set_decimal_point_state(const vfd_display* disp, bool on,
                             uint8_t* buf, uint8_t sz)
{
  if (on) {
    vfd_enable_segment(disp->controller, buf, sz, 6, FV935ND_SEG_DOTS);
  } else {
    vfd_disable_segment(disp->controller, buf, sz, 6, FV935ND_SEG_DOTS);
  }
}

void set_colon_state(const vfd_display* disp, int idx, bool on,
                     uint8_t* buf, uint8_t sz)
{
  int d = 2*idx + 3;
  if (on) {
    vfd_enable_segment(disp->controller, buf, sz, d, FV935ND_SEG_DOTS);
  } else {
    vfd_disable_segment(disp->controller, buf, sz, d, FV935ND_SEG_DOTS);
  }
}
