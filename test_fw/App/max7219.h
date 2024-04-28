#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void max7219_write_reg(uint8_t reg, uint8_t value);
void max7219_set_row(uint8_t row_index);
void max7219_set_col(uint8_t col_index);
void max7219_clear(void);

void max7219_init();

#ifdef __cplusplus
}
#endif
