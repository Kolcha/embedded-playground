#include "usart.h"

// to get printf() work
int __io_putchar(int ch)
{
    static uint8_t cr = '\r';
    if (ch == '\n')
        HAL_UART_Transmit(&huart2, &cr, 1, 0xFFFF);
    HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
    return ch;
}
