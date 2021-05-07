#pragma once
#include "pw_assert/check.h"
#include "pw_status/status.h"
#include "tr_gpio/gpio.h"
#include "tr_spi/spi.h"

namespace tr::artic::internal {

enum class RxMode {
  ARGOS4 = 0b00000001,
};

class ArticConfigWriter {
 public:
  ArticConfigWriter(spi::SpiInterface &spi, gpio::GpiInterface &int2)
      : spi_(spi), interrupt2_(int2) {}

  pw::Status SetRxMode(RxMode mode) {
    const std::byte command = static_cast<std::byte>(mode);
    std::span<const std::byte> buffer(&command, 1);
    PW_CHECK_OK(spi_.Write(buffer));
    if (interrupt2_.IsHigh()) {
      return pw::Status::Aborted();
    }
    return pw::OkStatus();
  }

 public:
  spi::SpiInterface &spi_;
  gpio::GpiInterface &interrupt2_;
};
}  // namespace tr::artic::internal