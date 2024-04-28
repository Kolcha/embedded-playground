#include <climits>
#include <cstdio>

#include <chrono>
#include <complex>
#include <vector>

#include "kissfft.hh"

#include "main.h"
#include "gpio.h"
#include "spi.h"
#include "usart.h"

#include "max7219.h"
#include "wave_data.h"

extern "C"
void SystemClock_Config(void);

int main()
{
  HAL_Init();
  SystemClock_Config();
  MX_USART2_UART_Init();
  MX_GPIO_Init();
  MX_SPI3_Init();
  HAL_Delay(100);
  max7219_init();

  constexpr const auto N = sizeof(wave_data) / sizeof(float);
  std::vector<std::complex<float>> fft_out(N/2);
  kissfft<float> kissfft_obj(N/2, false);

  auto fft_min = INT_MAX;
  auto fft_max = INT_MIN;
  auto fft_avg = 0;

  auto do_fft = [&]() {
    auto sm = HAL_GetTick();
    kissfft_obj.transform_real(wave_data, fft_out.data());
    auto em = HAL_GetTick();
    int d = em-sm;
    fft_min = d < fft_min ? d : fft_min;
    fft_max = d > fft_max ? d : fft_max;
    fft_avg = (fft_avg + d) / 2;
    printf("min: %d ms, max: %d ms, avg: %d ms, cur: %d ms\n", fft_min, fft_max, fft_avg, d);
  };

  while (true) {
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
      for (int i = 0; i < 8; i++) {
    	  do_fft();
          max7219_clear();
          max7219_set_row(i);
          printf("set_row(%d)\n", i);
          HAL_Delay(250);
      }
      HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
      for (int i = 0; i < 8; i++) {
    	  do_fft();
          max7219_clear();
          max7219_set_col(i);
          printf("set_col(%d)\n", i);
          HAL_Delay(250);
      }
  }
  return 0;
}
