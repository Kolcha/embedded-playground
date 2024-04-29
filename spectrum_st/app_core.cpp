#include "app_core.hpp"

AppCore& AppCore::instance()
{
  static AppCore core;
  return core;
}

void AppCore::setup()
{
  _processor.setLevelMaster(0.565f);
}

void AppCore::process(std::span<const float> data)
{
  _analyzer.analyze(data, _spectrum);
  _processor.process(_spectrum);
  _lm.display(_visualizer.bars8(_spectrum));
}

AppCore::AppCore()
  : _spectrum(N, 48000)
  , _analyzer(N)
  , _processor(_spectrum)
  , _visualizer(&_processor)
{}
