#include "nodes.h"

std::unique_ptr<Oscillator> Oscillator::init(float amp_, float freq_) {
  auto osc = std::make_unique<Oscillator>();
  osc->amp.store(amp_);
  osc->freq.store(freq_);
  std::cout << "new Oscillator: amp=" << amp_ << " freq=" << freq_ << std::endl;
  return osc;
}

void Oscillator::update() {
  float freqIncrement = 2.0f * M_PI * freq.load() / DEVICE_SAMPLE_RATE;
  float newPhase = phase.load() + freqIncrement;

  if (newPhase >= 2.0f * M_PI) {
    newPhase -= 2.0f * M_PI;
  }
  phase.store(newPhase);

  out.store(amp.load() * sinf(phase.load()));
}

std::unique_ptr<LFO> LFO::init(float base_, float amp_, float freq_) {
  auto lfo = std::make_unique<LFO>();
  lfo->base.store(base_);
  lfo->amp.store(amp_);
  lfo->freq.store(freq_);
  return lfo;
}

void LFO::update() {
  float freqIncrement = 2.0f * M_PI * freq.load() / DEVICE_SAMPLE_RATE;
  float newPhase = phase.load() + freqIncrement;

  if (newPhase >= 2.0f * M_PI) {
    newPhase -= 2.0f * M_PI;
  }
  phase.store(newPhase);

  out.store(base.load() + amp.load() * sinf(phase.load()));
  for (auto *t : targets) {
    t->store(out);
  }
}
