// SPDX-License-Identifier: MIT
// Copyright (c) 2026 SuperJupiter
#ifndef SUPERJUPITER_INTEGRATION_H
#define SUPERJUPITER_INTEGRATION_H
#if defined(__FAST_MATH__) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
#error "SuperJupiter requires IEEE non-finite checks; disable fast-math"
#endif
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#define SUPERJUPITER_INTEGRATION_VERSION "0.1.0"
namespace superjupiter {
namespace integration {
enum class Method { Trapezoid, Previous };
enum class Status {
  Seeded,
  Integrated,
  GapSkipped,
  InvalidInput,
  InvalidTime,
  InvalidConfig,
  NumericError
};
struct Result {
  Status status;
  double valueSeconds;
  uint64_t coveredMs, skippedMs;
};
class Integration {
  uint32_t gap_, last_;
  Method method_;
  bool seeded_;
  double previous_, total_;
  uint64_t covered_, skipped_;
  Result result(Status s) const { return Result{s, total_, covered_, skipped_}; }

public:
  explicit Integration(uint32_t maxGapMs = 10000, Method method = Method::Trapezoid)
      : gap_(maxGapMs), last_(0), method_(method), seeded_(false), previous_(0), total_(0),
        covered_(0), skipped_(0) {}
  Result push(double value, uint32_t atMs) {
    if (!gap_ || gap_ >= 0x80000000UL)
      return result(Status::InvalidConfig);
    if (!std::isfinite(value)) {
      seeded_ = false;
      return result(Status::InvalidInput);
    }
    if (!seeded_) {
      seeded_ = true;
      previous_ = value;
      last_ = atMs;
      return result(Status::Seeded);
    }
    const uint32_t dt = atMs - last_;
    if (!dt || dt >= 0x80000000UL)
      return result(Status::InvalidTime);
    if (dt > gap_) {
      if (skipped_ > UINT64_MAX - dt)
        return result(Status::NumericError);
      skipped_ += dt;
      previous_ = value;
      last_ = atMs;
      return result(Status::GapSkipped);
    }
    const double average = method_ == Method::Previous ? previous_ : previous_ / 2 + value / 2;
    const double area = average * (static_cast<double>(dt) / 1000);
    const double next = total_ + area;
    if (!std::isfinite(area) || !std::isfinite(next) || covered_ > UINT64_MAX - dt)
      return result(Status::NumericError);
    total_ = next;
    covered_ += dt;
    previous_ = value;
    last_ = atMs;
    return result(Status::Integrated);
  }
  Result summary() const { return result(Status::Integrated); }
  void reset() {
    seeded_ = false;
    previous_ = total_ = 0;
    covered_ = skipped_ = 0;
  }
};
} // namespace integration
} // namespace superjupiter
#endif
