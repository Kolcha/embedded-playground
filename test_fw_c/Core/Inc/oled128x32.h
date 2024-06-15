#ifndef __OLED128X32_H__
#define __OLED128X32_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void oled128x32_init();
void oled128x32_set_row(uint8_t row);
void oled128x32_set_col(uint8_t col);
void oled128x32_clear();

#ifdef __cplusplus
}
#endif

#endif /* __OLED128X32_H__ */
