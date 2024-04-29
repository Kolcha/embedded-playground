#pragma once

#include <algorithm>
#include <array>
#include <cassert>

#include "processor.hpp"

template<std::floating_point T>
constexpr const std::array<T, 9> bands_8 = {
  20.0855369231877,
  47.6555930039437,
  113.069197664102,
  268.271626781473,
  636.509918021887,
  1510.20396976326,
  3583.15866840945,
  8501.51787444345,
  20170.9756273686,
};

template<std::floating_point T>
constexpr const std::array<T, 17> bands_16 = {
  20.0855369231877,
  30.9384578296513,
  47.6555930039437,
  73.4055833378014,
  113.069197664102,
  174.164455605111,
  268.271626781473,
  413.228207132904,
  636.509918021887,
  980.438577877443,
  1510.20396976326,
  2326.22020568209,
  3583.15866840945,
  5519.26512014507,
  8501.51787444345,
  13095.1865141752,
  20170.9756273686,
};

template<std::floating_point T, std::size_t N>
std::array<T, N-1> split_to_bands(const Spectrum<T>& s,
                                  const std::array<T, N>& b)
{
  std::array<T, N-1> bands;

  auto fiter = b.begin();
  auto siter = s.begin();
  auto cval = siter->amplitude;
  // skip everything before min
  while (siter != s.end() && siter->frequency < *fiter)
    ++siter;
  ++fiter;  // move to the end of 1st, beginning of 2nd

  // main loop, process frequencies before the band end
  while (siter != s.end()) {
    auto [f, a] = *siter;
    if (fiter != b.end() && f > *fiter) {
      assert(fiter != b.begin());
      auto idx = std::distance(b.begin(), fiter) - 1;
      assert(0 <= idx && idx < bands.size());
      bands[idx] = cval;
      cval = a;
      ++fiter;
    } else {
      cval = std::max(cval, a);
    }
    ++siter;
  }
  // drop anything after the max frequency
  assert(fiter == b.end());
  assert(bands.size() == b.size()-1);
  return bands;
}

template<std::floating_point T>
T led_value(const Spectrum<T>& s, T lo, T hi)
{
  assert(lo < hi);
  auto by_freq_l = [](auto&& e, T f) { return e.frequency < f; };
  auto l_iter = std::lower_bound(s.begin(), s.end(), lo, by_freq_l);
  auto by_freq_u = [](T f, auto&& e) { return f < e.frequency; };
  auto h_iter = std::upper_bound(s.begin(), s.end(), hi, by_freq_u);
  auto by_value = [](auto&& a, auto&& b) { return a.amplitude < b.amplitude; };
  return std::max_element(l_iter, h_iter, by_value)->amplitude;
}


template<std::floating_point T>
class SpectrumVisualizer
{
public:
  explicit SpectrumVisualizer(const SpectrumProcessor<T>* p) noexcept
    : _proc(p)
  {
    assert(_proc);
  }

  inline std::array<T, 8> bars8(const Spectrum<T>& s) const
  {
    return split_to_bands(s, bands_8<T>);
  }

  inline std::array<T, 16> bars16(const Spectrum<T>& s) const
  {
    return split_to_bands(s, bands_16<T>);
  }

  std::array<T, 3> rgb(const Spectrum<T>& s) const
  {
    return {
          led_value(s, T(20), _proc->thresholdLow()),
          led_value(s, _proc->thresholdMidLow(), _proc->thresholdMidHigh()),
          led_value(s, _proc->thresholdHigh(), T(20000)),
    };
  }

private:
  const SpectrumProcessor<T>* _proc;
};
