/*
 * Copyright (c) 2024 Nick Korotysh
 * SPDX-License-Identifier: MIT
 */

#include <stdint.h>
#include <libusb.h>
#include <unistd.h>

static uint8_t reverse_byte(uint8_t x)
{
  x = ((x >> 1) & 0x55) | ((x << 1) & 0xaa);
  x = ((x >> 2) & 0x33) | ((x << 2) & 0xcc);
  x = ((x >> 4) & 0x0f) | ((x << 4) & 0xf0);
  return x;
}

static void ch341a_spi_init(libusb_device_handle* dev)
{
  uint8_t data[] = {0xAB, 0x40 | 0x3F, 0x20};
  int btr = 0;
  libusb_bulk_transfer(dev, 0x02, data, sizeof(data), &btr, 1);
}

static void ch341a_spi_cs_set(libusb_device_handle* dev, int high)
{
  uint8_t data[] = {0xAB, 0x80 | (high ? 0x37 : 0x36), 0x20};
  int btr = 0;
  libusb_bulk_transfer(dev, 0x02, data, sizeof(data), &btr, 1);
}

static void max7219_write(libusb_device_handle* dev, uint8_t addr, uint8_t data)
{
  uint8_t usb_data[] = {0xA8, reverse_byte(addr), reverse_byte(data)};
  int btr = 0;
  ch341a_spi_cs_set(dev, 0);
  libusb_bulk_transfer(dev, 0x02, usb_data, sizeof(usb_data), &btr, 1);
  libusb_bulk_transfer(dev, 0x82, usb_data, sizeof(usb_data), &btr, 1);
  ch341a_spi_cs_set(dev, 1);
}

static void max7219_init(libusb_device_handle* dev)
{
  for (uint8_t i = 0; i < 8; i++)     // zero all rows
    max7219_write(dev, 0x01 + i, 0x00);

  max7219_write(dev, 0x0A, 0x00);     // brightness
  max7219_write(dev, 0x0B, 0x07);     // scan limit (8 digits)
  max7219_write(dev, 0x09, 0x00);     // decode mode (no BCD decoding)
  max7219_write(dev, 0x0F, 0x00);     // disable test mode
  max7219_write(dev, 0x0C, 0x01);     // shutdown mode (normal operation)

  max7219_write(dev, 0x01, 0x01);     // light up one dot (0,0)
}


static void max7219_set_row(libusb_device_handle* dev, uint8_t row_index)
{
  max7219_write(dev, row_index + 1, 0xFF);
}

static void max7219_set_col(libusb_device_handle* dev, uint8_t col_index)
{
  for (uint8_t i = 0; i < 8; i++)
    max7219_write(dev, i + 1, 0x01 << col_index);
}

static void max7219_clear(libusb_device_handle* dev)
{
  for (uint8_t i = 0; i < 8; i++)
    max7219_write(dev, i + 1, 0x00);
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

  ch341a_spi_init(dev);
  max7219_init(dev);

  while (1) {
    for (int i = 0; i < 8; i++) {
      max7219_clear(dev);
      max7219_set_row(dev, i);
      usleep(250000);
    }
    for (int i = 0; i < 8; i++) {
      max7219_clear(dev);
      max7219_set_col(dev, i);
      usleep(250000);
    }
  }

  libusb_close(dev);
  libusb_exit(ctx);

  return 0;
}
