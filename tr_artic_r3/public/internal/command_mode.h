#pragma once
#include <type_traits>

#include "pw_assert/check.h"
#include "pw_status/status.h"
#include "tr_gpio/gpio.h"
#include "tr_spi/spi.h"

namespace tr::artic::internal {

enum class RxMode {
  ARGOS4 = 0b00000001,
};

enum class TxMode {
  ARGOS4_HD = 0b00001000,
};

enum class Instructions {
  START_RX = 0b10000001,
  START_RX_1_MESSAGE = 0b10000010,
  START_RX_2_MESSAGE = 0b10000011,
  START_RX_FOR_FIXED_TIME = 0b10000100,
};

class ArticConfigWriter {
 public:
  ArticConfigWriter(spi::SpiInterface &spi, gpio::GpiInterface &int2)
      : spi_(spi), interrupt2_(int2) {}

  // Sets the receive mode of the radio.
  pw::Status SetRxMode(RxMode mode) {
    // TODO(tracka/1): Verify ARGOS configuration register.
    return WriteCommand(mode);
  }

  // Sets the transmit mode of the radio.
  pw::Status SetTxMode(TxMode mode) {
    // TODO(tracka/1): Verify ARGOS configuration register.
    return WriteCommand(mode);
  }

 private:
  // Checks state of the
  bool Ok() { return !interrupt2_.IsHigh(); }

  // Takes any enum type and writes a single byte value over spi.
  template <class T>
  pw::Status WriteCommand(T command) {
    static_assert(std::is_enum<T>::value);
    const std::byte command_byte = static_cast<std::byte>(command);
    std::span<const std::byte> buffer(&command_byte, 1);
    PW_CHECK_OK(spi_.Write(buffer));
    if (!Ok()) {
      return pw::Status::Aborted();
    }
    return pw::OkStatus();
  }

  spi::SpiInterface &spi_;
  gpio::GpiInterface &interrupt2_;
};
}  // namespace tr::artic::internal