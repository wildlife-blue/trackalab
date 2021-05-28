#pragma once
#include "tr_gpio/gpio.h"
#include "tr_spi/spi.h"

namespace tr::artic::internal {
class MockGpi : public gpio::GpiInterface {
 public:
  MOCK_METHOD(bool, IsHigh, (), (override));
};

class MockGpo : public gpio::GpoInterface {
 public:
  MOCK_METHOD(void, Write, (bool state), (override));
};

class MockSpi : public spi::SpiInterface {
 public:
  MOCK_METHOD(pw::Status, Write, (pw::ConstByteSpan), (override));
  MOCK_METHOD(pw::Result<pw::ConstByteSpan>, Read, (pw::ByteSpan), (override));
};
}  // namespace tr::artic::internal