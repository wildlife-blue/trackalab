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

using ::testing::_;
using ::testing::ElementsAre;
using ::testing::Return;
using b = std::byte;

TEST(BurstMode, ArticConfigRead) {
  // ARTIC R3 Datasheet, Section: Read Example - 3.4.1

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
                .Read<BurstRegisterID::ARGOS_CONFIG>(argos_config_buffer)
                .status(),
            pw::OkStatus());
}

TEST(BurstMode, RxTimeoutWrite) {
  MockSpi spi;
  // TODO(trbug/4): Uncomment the line below when there is an upstream fix for
  // ElementsAre matching with spans.
  // EXPECT_CALL(spi, Write(ElementsAre(b{0x00}, b{0x0A}, b{0x1C}, b{0x01},
  // b{0x00}, b{0x00},b{0x00},b{0x0A})));
  MockGpi interrupt1;
  EXPECT_CALL(interrupt1, IsHigh()).WillOnce(Return(true));
  MockGpi interrupt2;
  EXPECT_CALL(interrupt2, IsHigh()).WillOnce(Return(false));

  auto rx_timeout_register = pw::bytes::Array<0x00, 0x00, 0x0A>();
  EXPECT_EQ(BurstMode(spi, interrupt1, interrupt2)
                .Write<BurstRegisterID::RX_TIMEOUT>(rx_timeout_register),
            pw::OkStatus());
}

TEST(BurstMode, ReadBufferToSmall) {
  MockSpi spi;
  MockGpi interrupt1;
  MockGpi interrupt2;
  std::array<std::byte, 2> argos_config_buffer;
  EXPECT_EQ(BurstMode(spi, interrupt1, interrupt2)
                .Read<BurstRegisterID::ARGOS_CONFIG>(argos_config_buffer)
                .status(),
            pw::Status::FailedPrecondition());
}

TEST(BurstMode, WriteBufferToLarge) {
  MockSpi spi;
  MockGpi interrupt1;
  MockGpi interrupt2;
  auto register_buffer_to_big = pw::bytes::Array<0x00, 0x00, 0x00, 0x00>();
  EXPECT_EQ(BurstMode(spi, interrupt1, interrupt2)
                .Write<BurstRegisterID::RX_TIMEOUT>(register_buffer_to_big),
            pw::Status::FailedPrecondition());
}

TEST(BurstMode, ReadFirmwareVersion) {
  using testing::ElementsAreArray;
  auto kVersionRegister = pw::bytes::Array<
      // Version string.
      'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
      'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a', 'a',
      // Version number.
      1, 0, 0>();

  MockSpi spi;
  EXPECT_CALL(spi, Read(_))
      .WillRepeatedly(Return(pw::Result<pw::ConstByteSpan>(kVersionRegister)));
  // TODO(trbug/4): Uncomment the line below when there is an upstream fix for
  // ElementsAre matching with spans.
  // EXPECT_CALL(spi, Write(ElementsAre(b{0x00}, b{0x0B}, b{0x1D}, b{0x52})));

  MockGpi interrupt1;
  EXPECT_CALL(interrupt1, IsHigh()).WillRepeatedly(Return(true));

  MockGpi interrupt2;
  EXPECT_CALL(interrupt2, IsHigh()).WillRepeatedly(Return(false));

  BurstMode burst_mode(spi, interrupt1, interrupt2);
  FirmwareVersion version(burst_mode);

  pw::Result<std::string_view> version_string_result =
      version.VersionAsString();
  ASSERT_EQ(version_string_result.status(), pw::OkStatus());
  std::string_view version_string = version_string_result.value();
  char expected_version_string[] = "aaaaaaaaaaaaaaaaaaaaaaaa";
  ASSERT_THAT(version_string,
              ElementsAreArray(std::string_view(expected_version_string)));

  ASSERT_EQ(version.GetMajorVersion().value(), 1);
  ASSERT_EQ(version.GetMinorVersion().value(), 0);
  ASSERT_EQ(version.GetPatchVersion().value(), 0);
}

TEST(BurstMode, ReadArgosConfiguration) {
  // ARTIC R3 Datasheet, Section: Read Example - 3.3.1
  // Argos register in RxMode: Argos 3 Rx backup mode.
  // Argos register in TxMode: Argos PTT-A4-VLD.
  auto kArgosConfigRegister = pw::bytes::Array<0x00, 0x00, 0x61>();

  MockSpi spi;
  EXPECT_CALL(spi, Read(_))
      .WillRepeatedly([&kArgosConfigRegister](pw::ByteSpan buffer) {
        std::copy(kArgosConfigRegister.begin(), kArgosConfigRegister.end(),
                  buffer.begin());
        return pw::Result<pw::ConstByteSpan>(buffer);
      });

  MockGpi interrupt1;
  EXPECT_CALL(interrupt1, IsHigh()).WillRepeatedly(Return(true));

  MockGpi interrupt2;
  EXPECT_CALL(interrupt2, IsHigh()).WillRepeatedly(Return(false));

  BurstMode burst_mode(spi, interrupt1, interrupt2);
  ArgosConfiguration argos_config(burst_mode);

  EXPECT_EQ(argos_config.GetRxMode().value(), RxModeConfig::ARGOS_3_BACKUP);
  EXPECT_EQ(argos_config.GetTxMode().value(), TxModeConfig::ARGOS_PTT_A4_VLD);
}

}  // namespace tr::artic::internal