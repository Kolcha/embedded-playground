#include "app_core.hpp"
#include "wave_data.h"

#include <chrono>

void setup()
{
  AppCore::instance().setup();
  Serial1.begin(115200);
  while (!Serial1);
}

void loop()
{
  auto s = std::chrono::high_resolution_clock::now();
  AppCore::instance().process(wave_data);
  auto e = std::chrono::high_resolution_clock::now();
  auto d = std::chrono::duration_cast<std::chrono::microseconds>(e-s).count();
  Serial1.printf("processing time: %ld us\n", d);
  delay(250);
}
