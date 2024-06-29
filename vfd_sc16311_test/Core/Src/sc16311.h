#pragma once

#include <stdint.h>

#define SC16311_SEG1                            1
#define SC16311_SEG2                            2
#define SC16311_SEG3                            3
#define SC16311_SEG4                            4
#define SC16311_SEG5                            5
#define SC16311_SEG6                            6
#define SC16311_SEG7                            7
#define SC16311_SEG8                            8
#define SC16311_SEG9                            9
#define SC16311_SEG10                           10
#define SC16311_SEG11                           11
#define SC16311_SEG12                           12
#define SC16311_SEG13                           13
#define SC16311_SEG14                           14
#define SC16311_SEG15                           15
#define SC16311_SEG16                           16
#define SC16311_SEG17                           17
#define SC16311_SEG18                           18
#define SC16311_SEG19                           19
#define SC16311_SEG20                           20

#define SC16311_DIG1                            1
#define SC16311_DIG2                            2
#define SC16311_DIG3                            3
#define SC16311_DIG4                            4
#define SC16311_DIG5                            5
#define SC16311_DIG6                            6
#define SC16311_DIG7                            7
#define SC16311_DIG8                            8
#define SC16311_DIG9                            9
#define SC16311_DIG10                           10
#define SC16311_DIG11                           11
#define SC16311_DIG12                           12
#define SC16311_DIG13                           13
#define SC16311_DIG14                           14
#define SC16311_DIG15                           15
#define SC16311_DIG16                           16

#define SC16311_RAM_SIZE                        48

void sc16311_enable_segment(uint8_t buf[], uint8_t dig, uint8_t seg);
void sc16311_disable_segment(uint8_t buf[], uint8_t dig, uint8_t seg);

// command 1: display mode setting command
#define SC16311_CMD_DISPLAY_MODE                0x00
#define SC16311_DISPLAY_MODE_8_DIG_20SEG        0X07
#define SC16311_DISPLAY_MODE_9_DIG_19SEG        0X08
#define SC16311_DISPLAY_MODE_10DIG_18SEG        0X09
#define SC16311_DISPLAY_MODE_11DIG_17SEG        0X0A
#define SC16311_DISPLAY_MODE_12DIG_16SEG        0X0B
#define SC16311_DISPLAY_MODE_13DIG_15SEG        0X0C
#define SC16311_DISPLAY_MODE_14DIG_14SEG        0X0D
#define SC16311_DISPLAY_MODE_15DIG_13SEG        0X0E
#define SC16311_DISPLAY_MODE_16DIG_12SEG        0X0F

// command 2: data setting command
#define SC16311_CMD_DATA_SETTING                0x40
#define SC16311_DATA_NORMAL_MODE                (0 << 3)
#define SC16311_DATA_TEST_MODE                  (1 << 3)
#define SC16311_DATA_INC_ADDR                   (0 << 2)
#define SC16311_DATA_FIX_ADDR                   (1 << 2)
#define SC16311_DATA_WRITE_MEM                  0x00
#define SC16311_DATA_WRITE_LED                  0x01
#define SC16311_DATA_READ_KEY                   0x02
#define SC16311_DATA_READ_SW                    0x03

// command 3: address setting command
#define SC16311_CMD_ADDR_SETTING                0xC0
// the rest 6 bits are address [00h-2Fh]

// command 4: display control command
#define SC16311_CMD_DISPLAY_CONTROL             0x80
#define SC16311_DISPLAY_OFF                     (0 << 3)
#define SC16311_DISPLAY_ON                      (1 << 3)
#define SC16311_DISPLAY_PW_01_16                0x00
#define SC16311_DISPLAY_PW_02_16                0x01
#define SC16311_DISPLAY_PW_04_16                0x02
#define SC16311_DISPLAY_PW_10_16                0x03
#define SC16311_DISPLAY_PW_11_16                0x04
#define SC16311_DISPLAY_PW_12_16                0x05
#define SC16311_DISPLAY_PW_13_16                0x06
#define SC16311_DISPLAY_PW_14_16                0x07

void sc16311_write_cmd(uint8_t cmd);

void sc16311_write_byte(uint8_t addr, uint8_t data);
void sc16311_write_data(uint8_t addr, const uint8_t* data, uint8_t size);
