#pragma once

#include <array>
#include <memory>

class LedMatrix
{
public:
  LedMatrix();
  ~LedMatrix();

  // expects data in range [0,1]
  void display(const std::array<float, 8>& data) const;

private:
  class Impl;
  std::unique_ptr<Impl> _impl;
};
