#include "led_matrix.hpp"

#include <cstdint>
#include <cmath>

#include <Arduino.h>
#include <SPI.h>

class LedMatrix::Impl {
  SPIClass _spi_1;
  static constexpr const auto CS_PIN = PA4;

public:
  Impl() : _spi_1(PA7, PA6, PA5) { init(); }
  ~Impl() { shutdown(); }

  void display(const std::array<float, 8>& data);

private:
  void init();
  void shutdown();

  void write(uint8_t addr, uint8_t data);
  void write_row(uint8_t row, uint8_t val) { write(0x01 + row, val); }
};

void LedMatrix::Impl::init()
{
  pinMode(CS_PIN, OUTPUT);
  _spi_1.begin();

  // MAX7219 init

  for (uint8_t i = 0; i < 8; i++)   // zero all rows
    write_row(i, 0x00);

  write(0x0F, 0x00);    // disable test mode
  write(0x0B, 0x07);    // scan limit (8 digits)
  write(0x09, 0x00);    // decode mode (no BCD decoding)
  write(0x0A, 0x00);    // brightness
  write(0x0C, 0x01);    // shutdown mode (normal operation)

  write(0x01, 0x01);    // light up one dot (0,0)
}

void LedMatrix::Impl::shutdown()
{
  write(0x0C, 0x00);    // shutdown MAX7219

  _spi_1.end();
}

void LedMatrix::Impl::write(uint8_t addr, uint8_t data)
{
  digitalWrite(CS_PIN, LOW);
  _spi_1.transfer(addr);
  _spi_1.transfer(data);
  // uint8_t cmd[] = {addr, data};
  // _spi_1.transfer(cmd, 2);      // expects non-const!
  digitalWrite(CS_PIN, HIGH);
}

void LedMatrix::Impl::display(const std::array<float, 8>& data)
{
  std::array<uint8_t, 8> bars;
  for (size_t i = 0; i < data.size(); i++) {
    bars[i] = static_cast<uint8_t>(std::round(data[i]*8));
  }

  for (uint8_t i = 0; i < 8; i++) {  // rows
    uint8_t row = 0;
    for (uint8_t b = 0; b < 8; b++)  // bits
      if (bars[b] > i)
        row |= (1 << b);
    write_row(i, row);
  }
}

LedMatrix::LedMatrix()
  : _impl(std::make_unique<Impl>())
{
}

LedMatrix::~LedMatrix() = default;

void LedMatrix::display(const std::array<float, 8>& data) const
{
  _impl->display(data);
}
