#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <vector>

#include "spectrum.hpp"

template<std::floating_point T>
class SpectrumProcessor
{
public:
  // spectrum is used mostly to get frequencies
  explicit SpectrumProcessor(const Spectrum<T>& s);

  // modifies given spectrum
  void process(Spectrum<T>& s) const;

  void setLevelMaster(T lvl) noexcept { _level_master = lvl; }

  void setLevelLow(T lvl) noexcept { _level_low = lvl; }
  void setLevelMid(T lvl) noexcept { _level_mid = lvl; }
  void setLevelHigh(T lvl) noexcept { _level_high = lvl; }

  void setThresholdLow(T thr) noexcept { _thr_low = thr; }
  void setThresholdMidLow(T thr) noexcept { _thr_ml = thr; }
  void setThresholdMidHigh(T thr) noexcept { _thr_mh = thr; }
  void setThresholdHigh(T thr) noexcept { _thr_high = thr; }

  T levelMaster() const noexcept { return _level_master; }

  T levelLow() const noexcept { return _level_low; }
  T levelMid() const noexcept { return _level_mid; }
  T levelHigh() const noexcept { return _level_high; }

  T thresholdLow() const noexcept { return _thr_low; }
  T thresholdMidLow() const noexcept { return _thr_ml; }
  T thresholdMidHigh() const noexcept { return _thr_mh; }
  T thresholdHigh() const noexcept { return _thr_high; }

private:
  std::vector<T> _amp_k;

  T _level_master = 1.0;

  T _level_low   = 1.0;
  T _level_mid   = 1.0;
  T _level_high  = 1.0;

  T _thr_low   = 125;
  T _thr_ml    = 126;
  T _thr_mh    = 899;
  T _thr_high  = 900;
};


template<std::floating_point T>
SpectrumProcessor<T>::SpectrumProcessor(const Spectrum<T>& s)
  : _amp_k(s.size())
{
  auto llf = [](const auto& i) { return std::log(std::log(i.frequency)); };
  std::transform(s.begin(), s.end(), _amp_k.begin(), llf);
}

template<std::floating_point T>
void SpectrumProcessor<T>::process(Spectrum<T>& s) const
{
  assert(s.size() == _amp_k.size());

  for (std::size_t i = 0; i < s.size(); i++) {
    auto& [f, a] = s[i];

    // cut values at very low frequencies
    // they often have very high amplitude,
    // but this looks like some noise or errors
    //
    // also cut values for very high frequencies,
    // even no high amplitudes were noticed there
    if (f < 20 || f > 20000) {
      a = 0;
      continue;
    }

    auto k = _level_master * _amp_k[i];

    if (20 < f && f < _thr_low)
      k *= _level_low;
    if (_thr_ml < f && f < _thr_mh)
      k *= _level_mid;
    if (_thr_high < f && f < 20000)
      k *= _level_high;

    a = std::clamp(k*a, T(0), T(1));
 }
}
