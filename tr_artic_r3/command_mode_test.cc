#include "internal/command_mode.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "pw_status/status.h"
#include "tr_gpio/gpio.h"
#include "tr_spi/spi.h"

namespace tr::artic::internal {

class MockGpi : public gpio::GpiInterface {
 public:
  MOCK_METHOD(bool, IsHigh, (), (override));
};

class MockSpi : public spi::SpiInterface {
 public:
  MOCK_METHOD(pw::Status, Write, (std::span<const std::byte>), (override));
};

TEST(CommandMode, SetRxMode) {
  using ::testing::Return;
  MockSpi spi;
  MockGpi interrupt2;
  EXPECT_CALL(interrupt2, IsHigh()).WillOnce(Return(false));
  ArticConfigWriter config_writer(spi, interrupt2);
  pw::Status status = config_writer.SetRxMode(RxMode::ARGOS4);
  EXPECT_EQ(pw::OkStatus(), status);
}

TEST(CommandMode, SetRxModeFailed) {
  using ::testing::Return;
  MockSpi spi;
  MockGpi interrupt2;
  EXPECT_CALL(interrupt2, IsHigh()).WillOnce(Return(true));
  ArticConfigWriter config_writer(spi, interrupt2);
  pw::Status status = config_writer.SetRxMode(RxMode::ARGOS4);
  EXPECT_EQ(pw::Status::Aborted(), status);
}

}  // namespace tr::artic::internal