#include "internal/standard_mode.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "pw_bytes/array.h"

namespace tr::artic {

TEST(StandardMode, WriteBuilder) {
  constexpr auto kAddress = StandardModeRegisterAddress<0x00>();
  constexpr auto kRegister = pw::bytes::Array<1, 2, 3>();
  const StandardModeRegisterCommand got_command =
      StandardModeWriteCommand(kAddress, kRegister);
  constexpr auto kExpectedCommand = pw::bytes::Array<0b00000000, 1, 2, 3>();
  EXPECT_TRUE(std::equal(got_command.begin(), got_command.end(),
                         kExpectedCommand.begin(), kExpectedCommand.end()));
}

TEST(StandardMode, BurstModeRegister) {
  // This is a valid address for all memory types.
  constexpr uint16_t kSatelliteDetectionTimeoutRegisterAddress = 0x1C02;
  constexpr uint16_t kBurstModeStartAddress =
      kSatelliteDetectionTimeoutRegisterAddress;

  const StandardModeRegisterCommand got_command =
      SwitchToBurstModeCommand<BurstModeMemory::kProgram,
                               TransactionMode::kWrite>(kBurstModeStartAddress);

  constexpr auto kExpectedCommand =
      pw::bytes::Array<0x00, 0b00001000, 0x02, 0x1c>();
  EXPECT_TRUE(std::equal(got_command.begin(), got_command.end(),
                         kExpectedCommand.begin(), kExpectedCommand.end()));
}

}  // namespace tr::artic