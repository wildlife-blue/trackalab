#include "internal/burst_mode.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "internal/burst_mode.h"
#include "internal/mock_io.h"
#include "internal/standard_mode.h"
#include "pw_bytes/span.h"
#include "pw_result/result.h"
#include "pw_status/status.h"
#include "tr_gpio/gpio.h"
#include "tr_spi/spi.h"

namespace tr::artic::internal {

TEST(BurstMode, ArticConfigRead) {
  // ARTIC R3 Datasheet, Section: Read Example - 3.4.1
  using ::testing::_;
  using ::testing::ElementsAre;
  using ::testing::Return;
  using b = std::byte;
  MockSpi spi;
  // TODO(trbug/4): Uncomment the line below when there is an upstream fix for
  // ElementsAre matching with spans.
  // EXPECT_CALL(spi, Write(ElementsAre(b{0x00}, b{0x0B}, b{0x1C}, b{0x00})));
  constexpr auto kConfigRegister = pw::bytes::Array<0, 0, 0>();
  EXPECT_CALL(spi, Read(_))
      .WillOnce(Return(pw::Result<pw::ConstByteSpan>(kConfigRegister)));
  MockGpi interrupt1;
  EXPECT_CALL(interrupt1, IsHigh()).WillOnce(Return(true));
  MockGpi interrupt2;
  EXPECT_CALL(interrupt2, IsHigh()).WillOnce(Return(false));

  std::array<std::byte, 3> argos_config_buffer;
  EXPECT_EQ(BurstMode(spi, interrupt1, interrupt2)
                .Read(kArgosConfigRegister, argos_config_buffer)
                .status(),
            pw::OkStatus());
}

}  // namespace tr::artic::internal