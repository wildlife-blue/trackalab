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
  TRANSMIT_1_PACKAGE_AND_GO_IDLE = 0b01001000,
};

enum class HouseKeeping {
  CLEAR_INTERRUPT1 = 0b10000000,
  CLEAR_INTERRUPT2 = 0b11000000,
};

class ArticCommandWriter {
 public:
  ArticCommandWriter(spi::SpiInterface &spi, gpio::GpiInterface &int2)
      : spi_(spi), interrupt2_(int2) {}

  pw::Status WriteCommand(RxMode command) {
    return WriteCommand<RxMode>(command);
  }

  pw::Status WriteCommand(TxMode command) {
    return WriteCommand<TxMode>(command);
  }

  pw::Status WriteCommand(Instructions command) {
    return WriteCommand<Instructions>(command);
  }

  pw::Status WriteCommand(HouseKeeping command) {
    return WriteCommand<HouseKeeping>(command);
  }

 private:
  // Takes any enum type and writes a single byte value over spi.
  template <class T>
  pw::Status WriteCommand(T command) {
    const std::byte command_byte = static_cast<std::byte>(command);
    std::span<const std::byte> buffer(&command_byte, 1);
    PW_CHECK_OK(spi_.Write(buffer));
    if (!Ok()) {
      return pw::Status::Aborted();
    }
    return pw::OkStatus();
  }

  // Checks state of the tranceiver.
  bool Ok() { return !interrupt2_.IsHigh(); }
  spi::SpiInterface &spi_;
  gpio::GpiInterface &interrupt2_;
};

class ArticConfigWriter {
 public:
  ArticConfigWriter(spi::SpiInterface &spi, gpio::GpiInterface &int2)
      : command_writer_(spi, int2) {}

  // Sets the receive mode of the radio.
  pw::Status SetRxMode(RxMode mode) {
    // TODO(#1): Verify ARGOS configuration register.
    return command_writer_.WriteCommand(mode);
  }

  // Sets the transmit mode of the radio.
  pw::Status SetTxMode(TxMode mode) {
    // TODO(#1): Verify ARGOS configuration register.
    return command_writer_.WriteCommand(mode);
  }

 private:
  ArticCommandWriter command_writer_;
};
}  // namespace tr::artic::internal