#pragma once

#include "analyzer.hpp"
#include "processor.hpp"
#include "visualizer.hpp"
// TODO: hide this
#include "led_matrix.hpp"

class AppCore {
public:
  static AppCore& instance();

  void setup();
  void process(std::span<const float> data);

private:
  AppCore();

private:
  static constexpr const size_t N = 4096;
  using InputType = SpectrumAnalyzer::InputType;
  SpectrumAnalyzer::SpectrumType _spectrum;
  SpectrumAnalyzer _analyzer;
  SpectrumProcessor<InputType> _processor;
  SpectrumVisualizer<InputType> _visualizer;
  // TODO: remove
  LedMatrix _lm;
};
