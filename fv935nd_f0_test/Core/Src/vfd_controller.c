#include "vfd_controller.h"

void vfd_serial_write(const vfd_serial* s, const uint8_t* data, uint8_t size)
{
  s->prepare_write();
  for (uint8_t i = 0; i < size; i++)
    s->write_byte(data[i]);
  s->finish_write();
}

static inline uint8_t bytes_per_digit(const vfd_controller* c)
{
  return ((c->max_segments + 7) & ~7) / 8;
}

static inline uint8_t byte_offset(const vfd_controller* c,
                                  uint8_t dig, uint8_t seg)
{
  return (dig-1)*bytes_per_digit(c) + (seg-1)/8;
}

static inline uint8_t segment_mask(uint8_t seg)
{
  return 1 << ((seg-1) % 8);
}

static inline uint8_t ram_size(const vfd_controller* c)
{
  return bytes_per_digit(c) * c->max_digits;
}

void vfd_enable_segment(const vfd_controller* c, uint8_t buf[], uint8_t sz,
                        uint8_t dig, uint8_t seg)
{
  buf[byte_offset(c, dig, seg)] |= segment_mask(seg);
}

void vfd_disable_segment(const vfd_controller* c, uint8_t buf[], uint8_t sz,
                         uint8_t dig, uint8_t seg)
{
  buf[byte_offset(c, dig, seg)] &= ~segment_mask(seg);
}

void vfd_write_cmd(const vfd_controller* c, uint8_t cmd)
{
  vfd_serial_write(c->serial, &cmd, sizeof(cmd));
}

void vfd_write_byte(const vfd_controller* c, uint8_t addr, uint8_t byte)
{
  uint8_t data[] = { 0xC0 | (addr & 0x3F), byte };
  vfd_serial_write(c->serial, data, sizeof(data));
}

void vfd_write_data(const vfd_controller* c, uint8_t addr,
                    const uint8_t* data, uint8_t size)
{
  c->serial->prepare_write();
  uint8_t cmd3 = 0xC0 | (addr & 0x3F);
  c->serial->write_byte(cmd3);
  for (uint8_t i = 0; i < size; i++)
    c->serial->write_byte(data[i]);
  c->serial->finish_write();
}

void vfd_common_init(const vfd_controller* c, uint8_t displ_cfg)
{
  vfd_write_cmd(c, 0x40);
  for (uint8_t i = 0; i < ram_size(c); i++)
    vfd_write_byte(c, i, 0x00);
  vfd_write_cmd(c, displ_cfg & 0x3F);
  vfd_write_cmd(c, 0x8F);       // display on, max brightness
}
