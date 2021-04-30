#pragma once
#include <chrono>

#include "gpio/gpio.h"
#include "pw_assert/check.h"
#include "pw_chrono/system_clock.h"
#include "pw_status/status.h"
#include "spi/spi.h"

namespace tr::artic {

namespace internal {
template <class Func_t>
bool poll_true_for(
    Func_t func_condition,
    std::chrono::duration<long double, std::milli> for_at_least) {
  auto start_time = pw::chrono::SystemClock::now();
  while (pw::chrono::SystemClock::now() - start_time < for_at_least) {
    if (func_condition()) {
      return true;
    }
  }
  return false;
};
}  // namespace internal

class ArticR3 {
 public:
  constexpr ArticR3(gpio::GpoInterface &reset,
                    gpio::GpoInterface &device_select, gpio::GpiInterface &int1,
                    gpio::GpiInterface &int2, spi::SpiInterface &spi)
      : reset_(reset),
        device_select_(device_select),
        int1_(int1),
        int2_(int2),
        spi_(spi) {}

  void Reset() {
    // Reset line is active low. This resets and releases.
    reset_.Write(false);
    reset_.Write(true);

    using namespace std::chrono_literals;
    // ARTIC R3 Datasheet - sec: 3.1
    const auto startup_time = 40ms;
    PW_CHECK_OK(PollIdleStateTransitionFor(startup_time));
  }

 private:
  gpio::GpoInterface &reset_;
  gpio::GpoInterface &device_select_;
  gpio::GpiInterface &int1_;
  gpio::GpiInterface &int2_;
  spi::SpiInterface &spi_;

  pw::Status PollIdleStateTransitionFor(
      std::chrono::duration<long double, std::milli> duration) {
    if (internal::poll_true_for([this]() { return int1_.IsHigh(); },
                                duration)) {
      return pw::OkStatus();
    }
    return pw::Status::DeadlineExceeded();
  }
};
}  // namespace tr::artic