#include "artic_r3/artic_r3.h"

#include "gmock/gmock.h"
#include "gpio/gpio.h"
#include "gtest/gtest.h"
#include "spi/spi.h"

namespace tr::artic {

class MockGpi : public gpio::GpiInterface {
 public:
  MOCK_METHOD(bool, IsHigh, (), (override));
};

class MockGpo : public gpio::GpoInterface {
 public:
  MOCK_METHOD(void, Write, (bool state), (override));
};

class MockSpi : public spi::SpiInterface {};

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

TEST(ArticR3, todo) {}

}