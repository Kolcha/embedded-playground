#ifndef __FAST_FFT_H__
#define __FAST_FFT_H__

// fill out with N *pairs* of twiddle factors
// N - FFTs count, output buffer size must be 2*(N+1)
void fft_twiddle_factors(float* out, unsigned int N);

// does in-place FFT transform
// output format is the same as in KISS FFT C++
// data is array of (re,im) pairs
// tw - twiddle factors (see above)
// N - count of *pairs*, not size
void fft_cplx(float* data, const float* tw, unsigned int N);

// does in-place FFT transform
// output format is the same as in KISS FFT C++
// data is array of real values, 2*N in total
// tw - twiddle factors (see above)
// N - FFTs count, must be data array size / 2
void fft_real(float* data, const float* tw, unsigned int N);

#endif  // __FAST_FFT_H__
