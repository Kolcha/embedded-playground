/*
 * Copyright (c) 2024 Nick Korotysh
 * SPDX-License-Identifier: MIT
 */

#include <stdint.h>
#include <string.h>
#include <libusb.h>
#include <unistd.h>

static inline unsigned int min(unsigned int a, unsigned int b)
{
  return a < b ? a : b;
}

static void ch341a_i2c_init(libusb_device_handle* dev)
{
  uint8_t data[] = {0xAA, 0x60 | 0x02, 0x00};     // 0x02 - 400kHz
  int btr = 0;
  libusb_bulk_transfer(dev, 0x02, data, sizeof(data), &btr, 1);
}

static void ssd1306_write_cmd_byte(libusb_device_handle* dev, uint8_t addr,
                                   uint8_t cmd)
{
  uint8_t data[] = {0xAA, 0x74, 0x83, addr << 1, 0x80, cmd, 0x75, 0x00};
  int btr = 0;
  libusb_bulk_transfer(dev, 0x02, data, sizeof(data), &btr, 1);
}

static void ssd1306_write_cmd_bytes(libusb_device_handle* dev, uint8_t addr,
                                    const uint8_t* bytes, unsigned int count)
{
  for (unsigned int i = 0; i < count; i++)
    ssd1306_write_cmd_byte(dev, addr, bytes[i]);
}

static void ssd1306_write_frame(libusb_device_handle* dev, uint8_t addr,
                                const uint8_t* frame, unsigned int size)
{
  const unsigned int CH341_PACKET_LENGTH = 32;
  uint8_t usb_data[CH341_PACKET_LENGTH];
  uint8_t* ptr = usb_data;
  *ptr++ = 0xAA;                    // CH341A I2C stream
  *ptr++ = 0x74;                    // CH341A I2C start
  *ptr++ = 0x80 | 0x1C;             // CH341A I2C data size
  *ptr++ = addr << 1;               // I2C device address
  *ptr++ = 0x40;                    // SSD1306 control byte

  unsigned int write_left = size;
  while (write_left > 0) {
    unsigned int write_now = min(CH341_PACKET_LENGTH - 3, write_left);
    if (ptr != usb_data) {          // the first packet
      write_now = CH341_PACKET_LENGTH - (ptr - usb_data + 1);
    } else {
      *ptr++ = 0xAA;                // CH341A I2C stream
      *ptr++ = 0x80 | write_now;    // CH341A I2C data size
    }
    for (unsigned int i = 0; i < write_now; ++i)
      *ptr++ = *frame++;
    unsigned int usb_size = CH341_PACKET_LENGTH;
    if (write_left == write_now) {  // the last packet
      *ptr++ = 0x75;                // CH341A I2C stop
      usb_size = write_now + 4;     // 4 special bytes
    }
    *ptr++ = 0x00;                  // CH341A I2C packet end
    int btr = 0;
    libusb_bulk_transfer(dev, 0x02, usb_data, usb_size, &btr, 2);
    write_left -= write_now;
    ptr = usb_data;
  }
}

static void ssd1306_init(libusb_device_handle* dev, uint8_t addr)
{
  const uint8_t init_seq[] = {
    0xAE,
    0x20, 0x01,                 // vertical scanning
    0x21, 0x00, 0x7F,           // W - 1
    0x22, 0x00, 0x03,           // H/8 - 1
    0x40,
    // 0xA0 | 0x01,                // column remap
    0xA8, 0x1F,                 // H - 1
    // 0xC0 | 0x08,                // from bottom up
    0xD3, 0x00,
    0xDA, 0x02,                 // depends on wiring and resoultion
    0xD5, 0x80,
    0xD9, 0xF1,
    0xDB, 0x30,
    0x81, 0x1F,
    0xA4,
    0xA6,
    0x8D, 0x14,
    0x2E | 0x00,
    0xAF,
  };
  ssd1306_write_cmd_bytes(dev, addr, init_seq, sizeof(init_seq));
}


static void frame_set_row(uint8_t* frame, int r)
{
  for (int i = 0; i < 128; i++)
    frame[32/8*i+r/8] = 1 << (r % 8);
}

static void frame_set_column(uint8_t* frame, int c)
{
  const int h = 32/8;
  memset(frame + c*h, 0xFF, h);
}

static void frame_clear(uint8_t* frame)
{
  memset(frame, 0x00, 512);
}


int main()
{
  libusb_context* ctx;
  libusb_device_handle* dev;

  if (libusb_init(&ctx) != 0)
    return 1;

  dev = libusb_open_device_with_vid_pid(ctx, 0x1A86, 0x5512);
  if (!dev)
    return 1;

  libusb_detach_kernel_driver(dev, 0);
  libusb_claim_interface(dev, 0);

  ch341a_i2c_init(dev);
  ssd1306_init(dev, 0x3C);

  uint8_t frame_buf[512];

  while (1) {
    for (int i = 0; i < 32; i++) {
      frame_clear(frame_buf);
      frame_set_row(frame_buf, i);
      ssd1306_write_frame(dev, 0x3C, frame_buf, sizeof(frame_buf));
      usleep(32000);
    }
    for (int i = 0; i < 128; i++) {
      frame_clear(frame_buf);
      frame_set_column(frame_buf, i);
      ssd1306_write_frame(dev, 0x3C, frame_buf, sizeof(frame_buf));
      usleep(8000);
    }
  }

  libusb_close(dev);
  libusb_exit(ctx);

  return 0;
}
