#include "artic_r3/artic_r3.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "internal/mock_io.h"
#include "tr_gpio/gpio.h"
#include "tr_spi/spi.h"

namespace tr::artic {
using internal::MockGpi;
using internal::MockGpo;
using internal::MockSpi;

TEST(ArticR3, Reset) {
  using ::testing::Return;
  MockGpi int1;
  EXPECT_CALL(int1, IsHigh()).WillOnce(Return(true));
  MockGpi int2;
  MockGpo reset;
  EXPECT_CALL(reset, Write(false));
  EXPECT_CALL(reset, Write(true));
  MockGpo device_select;
  MockSpi spi;

  ArticR3 artic(reset, device_select, int1, int2, spi);
  artic.Reset();
}

}  // namespace tr::artic