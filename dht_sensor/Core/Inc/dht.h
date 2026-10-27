#ifndef DHT_SENSOR_H
#define DHT_SENSOR_H

#include <stdbool.h>
#include <stdint.h>

#include "gpio.h"

typedef enum {
  DHT11 = 11,
  DHT12 = 12,
  DHT21 = 21,
  DHT22 = 22,
}DHT_Type;

bool dht_read(GPIO_TypeDef* port, uint16_t pin, DHT_Type type,
              float* temp, float* hum);

#endif  /* DHT_SENSOR_H */
