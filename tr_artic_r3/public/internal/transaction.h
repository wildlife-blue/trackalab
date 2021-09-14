#pragma once
#include "command_mode.h"
#include "pw_status/status.h"
#include "tr_gpio/gpio.h"
#include "tr_spi/spi.h"

namespace tr::artic::internal {
class TransactionContext {
 public:
  constexpr TransactionContext(tr::spi::SpiInterface &spi,
                               tr::gpio::GpiInterface &interrupt1,
                               tr::gpio::GpiInterface &interrupt2)
      : spi_(spi), interrupt1_(interrupt1), interrupt2_(interrupt2) {}

  // Begins a transaction clearing and checking interrupts
  pw::Status Begin() {
    if (interrupt2_.IsHigh()) {
      return pw::Status::Aborted();
    }
    pw::Status status = ArticCommandWriter(spi_, interrupt2_)
                            .WriteCommand(HouseKeeping::CLEAR_INTERRUPT1);
    if (status != pw::OkStatus()) {
      return status;
    }
    if (interrupt1_.IsHigh()) {
      return pw::Status::Unknown();
    }
    return pw::OkStatus();
  }

  // Ends a transaction checking interrupts
  pw::Status End() {
    if (interrupt2_.IsHigh()) {
      return pw::Status::Aborted();
    }
    if (interrupt1_.IsHigh()) {
      return pw::OkStatus();
    }
    return pw::Status::Unknown();
  }

 private:
  tr::spi::SpiInterface &spi_;
  tr::gpio::GpiInterface &interrupt1_;
  tr::gpio::GpiInterface &interrupt2_;
};
}  // namespace tr::artic::internal