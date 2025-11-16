#pragma once
#include "utils.h"

struct Oscillator : Node {
  std::atomic<float> amp{1.0f};
  std::atomic<float> freq{440.0f};
  std::atomic<float> phase{0.0f};
  OscType type{OscType::Sine};

  void update() override;

  static std::unique_ptr<Oscillator>
  init(float amp_ = 1.0f, float freq_ = 440.0f, OscType type_ = OscType::Sine);
};

struct LFO : Node {
  std::atomic<float> amp{1.0f};
  std::atomic<float> freq{5.0f};
  std::atomic<float> phase{0.0f};
  std::atomic<float> base{0.0f};

  std::vector<std::atomic<float> *> targets; // parameters it modulates

  void update() override;

  static std::unique_ptr<LFO> init(float base_ = 0.0f, float amp_ = 1.0f,
                                   float freq_ = 5.0f);
};

struct Filter : Node {
  std::atomic<float> cutoff{500.0f};
  std::atomic<float> q{1.0f};

  std::vector<std::atomic<float> *> inputs; // upstream audio sources
  float x1 = 0.0f;
  float x2 = 0.0f;
  float y1 = 0.0f;
  float y2 = 0.0f;

  void update() override;

  static std::unique_ptr<Filter> init(float cutoff_ = 500.0f, float q_ = 1.0f);
};
