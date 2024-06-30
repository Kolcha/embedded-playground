#include "sc16311.h"

static inline uint8_t byte_offset(uint8_t dig, uint8_t seg)
{
  return (dig-1)*2 + (seg-1)/8;
}

static inline uint8_t segment_mask(uint8_t seg)
{
  return 1 << ((seg-1) % 8);
}

void sc16311_enable_segment(uint8_t buf[], uint8_t dig, uint8_t seg)
{
  buf[byte_offset(dig, seg)] |= segment_mask(seg);
}

void sc16311_disable_segment(uint8_t buf[], uint8_t dig, uint8_t seg)
{
  buf[byte_offset(dig, seg)] &= ~segment_mask(seg);
}

__attribute__((weak)) void vfd_serial_prepare_write()
{
  // set STB to LOW
}

__attribute__((weak)) void vfd_serial_end_write()
{
  // set STB to HIGH
}

__attribute__((weak)) void vfd_serial_write_byte(uint8_t byte)
{
  (void)byte;
  // transmit single byte
}

static void vfd_serial_write(const uint8_t* data, uint8_t size)
{
  vfd_serial_prepare_write();
  for (uint8_t i = 0; i < size; i++)
    vfd_serial_write_byte(data[i]);
  vfd_serial_end_write();
}

void sc16311_write_cmd(uint8_t cmd)
{
  vfd_serial_write(&cmd, sizeof(cmd));
}

void sc16311_write_byte(uint8_t addr, uint8_t byte)
{
  uint8_t data[] = { 0xC0 | (addr & 0x3F), byte };
  vfd_serial_write(data, sizeof(data));
}

void sc16311_write_data(uint8_t addr, const uint8_t* data, uint8_t size)
{
  vfd_serial_prepare_write();
  uint8_t cmd3 = 0xC0 | (addr & 0x3F);
  vfd_serial_write_byte(cmd3);
  for (uint8_t i = 0; i < size; i++)
    vfd_serial_write_byte(data[i]);
  vfd_serial_end_write();
}
