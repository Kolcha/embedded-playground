#pragma once

#include <memory>
#include <span>

#include "spectrum.hpp"

// output:
// - type (must be defined, float/double)
// - range (fixed, 0..1)
// - frequencies (fixed, depend on sample rate and sampes count)

// frequencies can be calculated only once,
// as sample rate and samples count are constants
// frequencies use the same output type as values

// Spectrum class is responsible for frequencies calculaion
// Analyzer accepts Spectrum and uses it for its setup and output
// such approach allows to avoid a lot of temporary allocations
// just by keeping the only one single instance of Spectrum forever

// Analyzer just does FFT, it knows nothing about frequencies
// Analyzer accepts single-channel input, user must prepare input

class SpectrumAnalyzer
{
public:
  // shortcut to input (and output) data type
  using InputType = float;
  // convenient shortcut to exact spectrum type used in analyzer
  using SpectrumType = Spectrum<InputType>;

  // @a samples_count must be power of 2 (for some FFT implementations)
  explicit SpectrumAnalyzer(std::size_t samples_count);
  ~SpectrumAnalyzer();

  // analyzes the @a data and writes result to @a spectrum
  // expects floating point input data in range [-1...1], doesn't normalize it
  // output data in @a spectrum is normalized to range [0...1]
  // @a spectrum must be initialized with the same samples count as initializer
  void analyze(std::span<const InputType> data, Spectrum<InputType>& spectrum) const;

private:
  class Impl;
  std::unique_ptr<Impl> _impl;
};
