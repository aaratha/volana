#pragma once
#include "utils.h"

struct Oscillator : Node {
  std::atomic<float> amp{1.0f};
  std::atomic<float> freq{440.0f};
  std::atomic<float> phase{0.0f};
  std::atomic<float> out{0.0f};

  void update() override;

  static std::unique_ptr<Oscillator> init(float amp_ = 1.0f,
                                          float freq_ = 440.0f);
};

struct LFO : Node {
  std::atomic<float> amp{1.0f};
  std::atomic<float> freq{5.0f};
  std::atomic<float> phase{0.0f};
  std::atomic<float> base{0.0f};
  std::atomic<float> out{0.0f};

  std::vector<std::atomic<float> *> targets; // parameters it modulates

  void update() override;

  static std::unique_ptr<LFO> init(float base_ = 0.0f, float amp_ = 1.0f,
                                   float freq_ = 5.0f);
};
