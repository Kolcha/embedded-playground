#include "max7219.h"

#include "gpio.h"
#include "spi.h"

#define DECODE_MODE_REG     0x09
#define INTENSITY_REG       0x0A
#define SCAN_LIMIT_REG      0x0B
#define SHUTDOWN_REG        0x0C
#define DISPLAY_TEST_REG    0x0F

void max7219_write_reg(uint8_t reg, uint8_t value)
{
    uint8_t tx_data[2] = { reg, value };
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi3, tx_data, 2, 100);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
}

void max7219_set_row(uint8_t row_index)
{
    max7219_write_reg(row_index + 1, 0xFF);
}

void max7219_set_col(uint8_t col_index)
{
    for (int i = 0; i < 8; i++) {
        max7219_write_reg(i + 1, 0x01 << col_index);
    }
}

void max7219_clear(void)
{
    for (int i = 0; i < 8; i++) {
        max7219_write_reg(i + 1, 0x00);
    }
}

void max7219_init()
{
    max7219_write_reg(DISPLAY_TEST_REG, 0);
    max7219_write_reg(SCAN_LIMIT_REG, 7);
    max7219_write_reg(DECODE_MODE_REG, 0);
    max7219_write_reg(SHUTDOWN_REG, 1);
    max7219_clear();
}
