#pragma once

#include <concepts>
#include <vector>

template<std::floating_point T>
class Spectrum
{
public:
  using value_type = std::remove_cv_t<T>;

  struct Item {
    value_type frequency;
    value_type amplitude;
  };

  using underlying_container = std::vector<Item>;

  using iterator = typename underlying_container::iterator;
  using const_iterator = typename underlying_container::const_iterator;

  Spectrum(std::size_t samples_count, std::size_t sample_rate)
    : _data(samples_count / 2)
  {
    for (std::size_t i = 0; i < _data.size(); i++)
      _data[i].frequency = static_cast<T>(sample_rate) * (i+1) / samples_count;
  }

  std::size_t size() const noexcept { return _data.size(); }

  iterator begin() noexcept { return _data.begin(); }
  iterator end() noexcept { return _data.end(); }

  const_iterator begin() const noexcept { return _data.begin(); }
  const_iterator end() const noexcept { return _data.end(); }

  Item& operator [](std::size_t i) { return _data[i]; }
  const Item& operator [](std::size_t i) const { return _data[i]; }

private:
  underlying_container _data;
};
