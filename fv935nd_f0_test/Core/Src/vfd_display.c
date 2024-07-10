#include "vfd_display.h"

#include <stddef.h>

const uint16_t* vfd_font_get_char(const vfd_font* fnt, char c)
{
  const char* ci = fnt->supported_chars;
  while (ci != fnt->supported_chars + fnt->chars_count)
    if (*ci == c)
      return fnt->data + (ci - fnt->supported_chars);
  return NULL;
}

void vfd_write_segments(const vfd_display* disp,
                        uint8_t d, uint16_t segs,
                        uint8_t* buf, uint8_t sz)
{
  for (uint8_t b = 0; b < sizeof(segs) * 8; b++) {
    uint8_t s = disp->pinout->seg_pins[b];
    if ((segs & (1 << b)) != 0) {
      vfd_enable_segment(disp->controller, buf, sz, d, s);
    } else {
      vfd_disable_segment(disp->controller, buf, sz, d, s);
    }
  }
}

void vfd_write_char(const vfd_display* disp, uint8_t d, char c,
                    uint8_t* buf, uint8_t sz)
{
  const uint16_t* fcp = vfd_font_get_char(disp->font, c);
  vfd_write_segments(disp, d, fcp ? *fcp : 0, buf, sz);
}
