#include "internal/command_mode.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "internal/mock_io.h"
#include "pw_status/status.h"
#include "tr_gpio/gpio.h"
#include "tr_spi/spi.h"

namespace tr::artic::internal {

TEST(CommandMode, SetRxMode) {
  using ::testing::Return;
  MockSpi spi;
  MockGpi interrupt2;
  EXPECT_CALL(interrupt2, IsHigh()).WillOnce(Return(false));
  ArticConfigWriter config_writer(spi, interrupt2);
  EXPECT_EQ(pw::OkStatus(), config_writer.SetRxMode(RxMode::ARGOS4));
}

TEST(CommandMode, SetRxModeFailed) {
  using ::testing::Return;
  MockSpi spi;
  MockGpi interrupt2;
  EXPECT_CALL(interrupt2, IsHigh()).WillOnce(Return(true));
  ArticConfigWriter config_writer(spi, interrupt2);
  EXPECT_EQ(pw::Status::Aborted(), config_writer.SetRxMode(RxMode::ARGOS4));
}

TEST(CommandMode, SetTxMode) {
  using ::testing::Return;
  MockSpi spi;
  MockGpi interrupt2;
  EXPECT_CALL(interrupt2, IsHigh()).WillOnce(Return(false));
  ArticConfigWriter config_writer(spi, interrupt2);
  EXPECT_EQ(pw::OkStatus(), config_writer.SetTxMode(TxMode::ARGOS4_HD));
}

}  // namespace tr::artic::internal