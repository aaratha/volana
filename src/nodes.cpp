#include "nodes.h"
#include "utils.h"

std::unique_ptr<Oscillator> Oscillator::init(float amp_, float freq_,
                                             OscType type_) {
  auto osc = std::make_unique<Oscillator>();
  osc->amp.store(amp_);
  osc->freq.store(freq_);
  osc->type = type_;
  osc->audioOut = true;
  std::cout << "new Oscillator: amp=" << amp_ << " freq=" << freq_ << std::endl;
  return osc;
}

void Oscillator::update() {
  float freqIncrement = 2.0f * M_PI * freq.load() / DEVICE_SAMPLE_RATE;
  float newPhase = phase.load() + freqIncrement;

  if (newPhase >= 2.0f * M_PI)
    newPhase -= 2.0f * M_PI;
  phase.store(newPhase);

  float value = 0.0f;
  float p = phase.load() / (2.0f * M_PI); // normalized 0..1

  switch (type) {
  case OscType::Sine:
    value = sinf(phase.load());
    break;
  case OscType::Saw:
    value = 2.0f * p - 1.0f; // ramps from -1 to 1
    break;
  case OscType::Square:
    value = (p < 0.5f) ? 1.0f : -1.0f;
    break;
  case OscType::Triangle:
    value = 4.0f * fabs(p - 0.5f) - 1.0f;
    break;
  }

  out.store(amp.load() * value);
}

std::unique_ptr<LFO> LFO::init(float base_, float amp_, float freq_) {
  auto lfo = std::make_unique<LFO>();
  lfo->base.store(base_);
  lfo->amp.store(amp_);
  lfo->freq.store(freq_);
  lfo->audioOut = false;
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
    t->store(out.load(std::memory_order_relaxed), std::memory_order_relaxed);
  }
}

std::unique_ptr<Filter> Filter::init(float cutoff_, float q_) {
  auto filter = std::make_unique<Filter>();
  filter->cutoff.store(cutoff_);
  filter->q.store(q_);
  filter->audioOut = true;

  return filter;
}

void Filter::update() {
  if (inputs.empty()) {
    out.store(0.0f, std::memory_order_relaxed);
    return;
  }

  // Mix all upstream audio inputs
  float acc = 0.0f;
  for (auto *in : inputs) {
    acc += in->load(std::memory_order_relaxed);
  }
  float input = acc / inputs.size();

  // Read parameters (atomic-safe)
  float fc = cutoff.load(std::memory_order_relaxed);
  float resonance = q.load(std::memory_order_relaxed);

  // Constrain parameters to sensible ranges
  fc = std::clamp(fc, 10.0f, DEVICE_SAMPLE_RATE * 0.45f);
  float Q = std::max(0.1f, resonance);

  // RBJ biquad low-pass coefficients
  float w0 = 2.0f * M_PI * fc / DEVICE_SAMPLE_RATE;
  float cosw0 = cosf(w0);
  float sinw0 = sinf(w0);
  float alpha = sinw0 / (2.0f * Q);

  float b0 = (1.0f - cosw0) * 0.5f;
  float b1 = 1.0f - cosw0;
  float b2 = (1.0f - cosw0) * 0.5f;
  float a0 = 1.0f + alpha;
  float a1 = -2.0f * cosw0;
  float a2 = 1.0f - alpha;

  // Direct Form I biquad
  float y = (b0 / a0) * input + (b1 / a0) * x1 + (b2 / a0) * x2 -
            (a1 / a0) * y1 - (a2 / a0) * y2;

  x2 = x1;
  x1 = input;
  y2 = y1;
  y1 = y;

  out.store(y, std::memory_order_relaxed);
}
