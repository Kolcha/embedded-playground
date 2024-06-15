#include "oled128x32.h"

#include <string.h>

#include "i2c.h"

// 128x32 display is expected, many hardcoded values depend on this

#define SSD1306_DISPLAY_ADDR    0x3C

#define SSD1306_DISPLAY_W       128
#define SSD1306_DISPLAY_H       32

// to avoid unnecessary memory allocation before i2c write,
// frame buffer has to be static and have one extra byte
// (ssd1306 control byte) at the its beginning
static uint8_t ssd1306_frame[SSD1306_DISPLAY_W*SSD1306_DISPLAY_H/8 + 4];
// actual frame data starts from the second byte
static uint8_t* frame_buffer = &ssd1306_frame[4];

static void ssd1306_write_cmd_byte(uint8_t addr, uint8_t cmd)
{
  uint8_t data[] = {0x00, cmd};
  HAL_I2C_Master_Transmit(&hi2c2, addr << 1, data, sizeof(data), 1);
}

static void ssd1306_write_cmd_bytes(uint8_t addr,
                                    const uint8_t* bytes, unsigned int count)
{
  for (unsigned int i = 0; i < count; i++)
    ssd1306_write_cmd_byte(addr, bytes[i]);
}

// frame buffer is static, see the beginning of the file
static void ssd1306_write_frame(uint8_t addr)
{
  ssd1306_frame[3] = 0x40;      // SSD1306 control byte
  HAL_I2C_Master_Transmit_DMA(&hi2c2, addr << 1, ssd1306_frame + 3, 513); // sizeof(ssd1306_frame));
}

static void ssd1306_init(uint8_t addr)
{
  // 128x32 display is assumed, must be adjusted for another one
  const uint8_t init_seq[] = {
    0xAE,
    0x20, 0x01,                 // vertical scanning
    0x21, 0x00, 0x7F,           // W - 1
    0x22, 0x00, 0x03,           // H/8 - 1
    0x40 | 0x00,
    0xA0 | 0x01,                // column remap
    0xA8, 0x1F,                 // H - 1
    0xC0 | 0x08,                // from bottom up
    0xD3, 0x00,
    0xDA, 0x02,                 // depends on wiring and resolution
    0xD5, 0x80,
    0xD9, 0xF1,
    0xDB, 0x40,
    0x81, 0x8F,
    0xA4,
    0xA6,
    0x8D, 0x14,
    0x2E | 0x00,
    0xAF,
  };
  ssd1306_write_cmd_bytes(addr, init_seq, sizeof(init_seq));
}

void oled128x32_init()
{
  ssd1306_init(SSD1306_DISPLAY_ADDR);
  // static data is zero initialized
  ssd1306_write_frame(SSD1306_DISPLAY_ADDR);
}

void oled128x32_set_row(uint8_t r)
{
  for (unsigned int i = 0; i < 128; i++)
    frame_buffer[32/8*i+r/8] = 1 << (r % 8);
  ssd1306_write_frame(SSD1306_DISPLAY_ADDR);
}

void oled128x32_set_col(uint8_t c)
{
  const unsigned int h = 32/8;
  memset(frame_buffer + c*h, 0xFF, h);
  ssd1306_write_frame(SSD1306_DISPLAY_ADDR);
}

void oled128x32_clear()
{
  memset(ssd1306_frame, 0x00, sizeof(ssd1306_frame));
  ssd1306_write_frame(SSD1306_DISPLAY_ADDR);
}
