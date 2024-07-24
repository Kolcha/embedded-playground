extern "C" {
#include "fast_fft.h"
#include "fast_ln.h"
#include "fft_hamming_1024.h"
#include "fft_twiddles_512.h"
#include "wave_data_int16_4096.h"
}

#include "esp32-hal-cpu.h"

#define SAMPLES_COUNT       1024
#define FFT_SIZE        (SAMPLES_COUNT/2)

#define count_of(X)     (sizeof(X)/sizeof(X[0]))

static float fft_io_buffer[SAMPLES_COUNT];

void prepare_fft_input(const int16_t* raw_input, float* input, size_t ns)
{
  const int16_t* raw_input_end = raw_input + 2*ns;        // 2 channels
  const float* wnd_k = fft_hamming_1024;
  for (; raw_input != raw_input_end; raw_input += 2) {
    int16_t ch1 = *(raw_input+0);
    int16_t ch2 = *(raw_input+1);
    *input++ = (ch1 > ch2 ? ch1 : ch2) / 32768.f * *wnd_k++;
  }
}

static float clamp(float val, float l, float h)
{
  val = fmax(val, l);
  val = fmin(val, h);
  return val;
  // fmin(fmax(val, l), h);
}

void calculate_spectrum(float* fft_buffer, size_t nfft)
{
  // first item in the output is special, its real part is not used,
  // imaginary part (the second array element) is the last magnitude
  // so, save it for the later use, as it will be overwritten first
  float last_magnitude = fft_buffer[1];

  float* buf_end = fft_buffer + 2*nfft;

  float* fft_in = fft_buffer + 2;
  float* sp_out = fft_buffer;
  while (sp_out != buf_end) {
    float m = fft_in == buf_end ?
              last_magnitude : hypot(*fft_in, *(fft_in+1));
    fft_in += 2;
    *sp_out++ = 0;          // no frequencies
    *sp_out++ = clamp(0.15f * log(m), 0.f, 1.f);
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {}
}

void loop() {
  auto ts = micros();
  prepare_fft_input(wave_data_int16_4096, fft_io_buffer, SAMPLES_COUNT);
  auto tp = micros();
  fft_real(fft_io_buffer, fft_twiddles_512, FFT_SIZE);
  auto tf = micros();
  calculate_spectrum(fft_io_buffer, FFT_SIZE);
  auto te = micros();
  auto dp = tp-ts;
  auto df = tf-tp;
  auto ds = te-tf;
  auto dt = te-ts;
  auto f = getCpuFrequencyMhz();
  Serial.printf("%luMHz, prepare: %ld, fft: %ld, spectrum: %ld, total: %ld us\n", f, dp, df, ds, dt);
  delay(250);
}
