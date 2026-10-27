#include "dht.h"

#include <math.h>
#include <stddef.h>
#include <stdio.h>

static void gpio_pin_input(GPIO_TypeDef* port, uint16_t pin)
{
  GPIO_InitTypeDef cfg = {
    .Pin = pin,
    .Mode = GPIO_MODE_INPUT,
    .Pull = GPIO_PULLUP,
  };
  HAL_GPIO_DeInit(port, pin);
  HAL_GPIO_Init(port, &cfg);
}

static void gpio_pin_output(GPIO_TypeDef* port, uint16_t pin)
{
  GPIO_InitTypeDef cfg = {
    .Pin = pin,
    .Mode = GPIO_MODE_OUTPUT_PP,
    .Pull = GPIO_NOPULL,
    .Speed = GPIO_SPEED_FREQ_LOW,
  };
  HAL_GPIO_DeInit(port, pin);
  HAL_GPIO_Init(port, &cfg);
}

#define US_TO_SYSTICK(x)    ((x) * (SystemCoreClock/1000000U))
#define MS_TO_SYSTICK(x)    ((x) * (SystemCoreClock/1000U))

#define SYSTICK_TO_US(x)    (uint32_t)((x) * (1000000.f/SystemCoreClock))
#define SYSTICK_TO_MS(x)    (uint32_t)((x) * (1000.f/SystemCoreClock))

static uint32_t gpio_wait_for_level(GPIO_TypeDef* port, uint16_t pin,
                                    GPIO_PinState level, uint32_t timeout)
{
  uint32_t cnt = 0;
  while (HAL_GPIO_ReadPin(port, pin) != level && cnt++ < timeout) {}
  return cnt;
}

#define SYSTICK_LOAD (SystemCoreClock/1000000U)
#define SYSTICK_DELAY_CALIB (SYSTICK_LOAD >> 1)

static void delay_microseconds(uint32_t us)
{
  uint32_t start = SysTick->VAL;
  uint32_t ticks = (us * SYSTICK_LOAD)-SYSTICK_DELAY_CALIB;
  while((start - SysTick->VAL) < ticks);
}

#define BIT_TIMEOUT         US_TO_SYSTICK(100)

static float decode_temperature(DHT_Type type, const uint8_t* data, size_t sz)
{
  float f = NAN;
  if (sz < 4) return f;
  switch (type) {
      case DHT11:
        f = data[2];
        if (data[3] & 0x80) {
          f = -1 - f;
        }
        f += (data[3] & 0x0f) * 0.1;
        break;
      case DHT12:
        f = data[2];
        f += (data[3] & 0x0f) * 0.1;
        if (data[2] & 0x80) {
          f *= -1;
        }
        break;
      case DHT22:
      case DHT21:
        f = ((uint32_t)(data[2] & 0x7F)) << 8 | data[3];
        f *= 0.1;
        if (data[2] & 0x80) {
          f *= -1;
        }
        break;
  }
  return f;
}

static float decode_humidity(DHT_Type type, const uint8_t* data, size_t sz)
{
  float f = NAN;
  if (sz < 2) return f;
  switch (type) {
      case DHT11:
      case DHT12:
        f = data[0] + data[1] * 0.1;
        break;
      case DHT22:
      case DHT21:
        f = ((uint32_t)data[0]) << 8 | data[1];
        f *= 0.1;
        break;
  }
  return f;
}


bool dht_read(GPIO_TypeDef* port, uint16_t pin, DHT_Type type,
              float* temp, float* hum)
{
  uint8_t data[5];
  uint32_t cycles[80];

  // Go into high impedance state to let pull-up raise data line level and
  // start the reading process.
  gpio_pin_input(port, pin);
  HAL_Delay(1);

  // First set data line low for a period according to sensor type
  gpio_pin_output(port, pin);
  HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
  switch (type) {
    case DHT22:
    case DHT21:
      delay_microseconds(1100); // data sheet says "at least 1ms"
      break;
    case DHT11:
    default:
      HAL_Delay(20); // data sheet says at least 18ms, 20ms just to be safe
      break;
  }

  // End the start signal by setting data line high for 40 microseconds.
  gpio_pin_input(port, pin);

  // Delay a moment to let sensor pull data line low.
//  delay_microseconds(55);

  // Wait for a moment to let sensor pull data line low.
  if (gpio_wait_for_level(port, pin, GPIO_PIN_RESET, BIT_TIMEOUT) >= BIT_TIMEOUT) {
    printf("DHT timeout waiting for start signal low pulse.\n");
    return false;
  }

  // Now start reading the data line to get the value from the DHT sensor.

  // First expect a low signal for ~80 microseconds followed by a high signal
  // for ~80 microseconds again.
  if (gpio_wait_for_level(port, pin, GPIO_PIN_SET, BIT_TIMEOUT) >= BIT_TIMEOUT) {
    printf("DHT timeout waiting for start signal high pulse.\n");
    return false;
  }
  if (gpio_wait_for_level(port, pin, GPIO_PIN_RESET, BIT_TIMEOUT) >= BIT_TIMEOUT) {
    printf("DHT timeout waiting for first bit low level\n");
    return false;
  }

  // Now read the 40 bits sent by the sensor.  Each bit is sent as a 50
  // microsecond low pulse followed by a variable length high pulse.  If the
  // high pulse is ~28 microseconds then it's a 0 and if it's ~70 microseconds
  // then it's a 1.  We measure the cycle count of the initial 50us low pulse
  // and use that to compare to the cycle count of the high pulse to determine
  // if the bit is a 0 (high state cycle count < low state cycle count), or a
  // 1 (high state cycle count > low state cycle count). Note that for speed
  // all the pulses are read into a array and then examined in a later step.
  for (int i = 0; i < 80; i += 2) {
    cycles[i] = gpio_wait_for_level(port, pin, GPIO_PIN_SET, BIT_TIMEOUT);
    cycles[i + 1] = gpio_wait_for_level(port, pin, GPIO_PIN_RESET, BIT_TIMEOUT);
  }

  // Inspect pulses and determine which ones are 0 (high state cycle count < low
  // state cycle count), or 1 (high state cycle count > low state cycle count).
  for (int i = 0; i < 40; ++i) {
    uint32_t lowCycles = cycles[2 * i];
    uint32_t highCycles = cycles[2 * i + 1];
    if ((lowCycles >= BIT_TIMEOUT) || (highCycles >= BIT_TIMEOUT)) {
      printf("DHT timeout waiting for pulse.\n");
      return false;
    }
    data[i / 8] <<= 1;
    // Now compare the low and high cycle times to see if the bit is a 0 or 1.
    if (highCycles > lowCycles) {
      // High cycles are greater than 50us low cycle count, must be a 1.
      data[i / 8] |= 1;
    }
    // Else high cycles are less than (or equal to, a weird case) the 50us low
    // cycle count so this must be a zero.  Nothing needs to be changed in the
    // stored data.
  }

  // Check we read 40 bits and that the checksum matches.
  if (data[4] != ((data[0] + data[1] + data[2] + data[3]) & 0xFF)) {
    printf("DHT checksum failure!\n");
    return false;
  }

  printf("DHT raw data: [ ");
  for (int i = 0; i < 5; i++) printf("%02lx ", (uint32_t)(data[i]));
  printf("]\n");

  // Decode temperature and humidity values
  if (temp)
    *temp = decode_temperature(type, data, sizeof(data));
  if (hum)
    *hum = decode_humidity(type, data, sizeof(data));

  return true;
}
