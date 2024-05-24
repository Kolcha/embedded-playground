#include "analyzer.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <complex>
#include <numbers>
#include <vector>

#include "kissfft.hh"

class SpectrumAnalyzer::Impl
{
  using InputType = SpectrumAnalyzer::InputType;

public:
  Impl(std::size_t samples_count)
    : _samples_count(samples_count)
    , _window(samples_count)
    , _fft_in(samples_count)
    , _fft_out(samples_count/2)
    , _kissfft_obj(samples_count/2, false)
  {
    constexpr auto pi = std::numbers::pi_v<InputType>;
    for (std::size_t i = 0; i < _window.size(); i++)
      _window[i] = 0.5 * (1 - std::cos((2*pi*i)/(_samples_count - 1)));
  }

  void analyze(std::span<const InputType> data, Spectrum<InputType>& spectrum)
  {
    assert(data.size() == _fft_in.size());
    assert(data.size() == 2*spectrum.size());

    assert(_window.size() == data.size());
    std::transform(data.begin(), data.end(),
                   _window.begin(),
                   _fft_in.begin(),
                   std::multiplies{});

    _kissfft_obj.transform_real(_fft_in.data(), _fft_out.data());

    assert(_fft_out.size() == spectrum.size());
    const auto N = _fft_out.size();
    for (std::size_t i = 1; i <= N; i++) {
      auto m = i == N ? _fft_out[0].imag() : std::abs(_fft_out[i]);
      auto a = 0.15f * std::log(m);
      spectrum[i-1].amplitude = std::clamp(a, 0.f, 1.f);
    }
  }

private:
  std::size_t _samples_count;
  std::vector<InputType> _window;
  std::vector<InputType> _fft_in;
  std::vector<std::complex<InputType>> _fft_out;
  kissfft<InputType> _kissfft_obj;
};


SpectrumAnalyzer::SpectrumAnalyzer(std::size_t samples_count)
  : _impl(std::make_unique<Impl>(samples_count))
{
}

SpectrumAnalyzer::~SpectrumAnalyzer() = default;

void SpectrumAnalyzer::analyze(std::span<const InputType> data,
                               Spectrum<InputType>& spectrum) const
{
  _impl->analyze(data, spectrum);
}
