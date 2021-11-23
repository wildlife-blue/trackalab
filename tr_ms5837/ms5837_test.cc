#include "tr_ms5837/ms5837.h"

#include "gtest/gtest.h"
#include "pw_bytes/array.h"
#include "pw_i2c/address.h"
#include "pw_i2c/initiator_gmock.h"

namespace tr::ms5837 {

using ::testing::_;
using ::testing::ElementsAreArray;
using ::testing::Property;
using ::testing::Return;

TEST(Ms5837, Reset) {
  pw::i2c::GmockInitiator i2c;
  auto reset_command = pw::bytes::Array<0x1E>();
  constexpr pw::i2c::Address kAddress = pw::i2c::Address::SevenBit<0x76>();
  // NOLINTNEXTLINE
  constexpr auto command = pw::bytes::Array<0x1E>();
  // Not testing for PROM getter.
  constexpr int kPromCalls = 7;
  EXPECT_CALL(i2c, DoWriteReadFor(_, _, _, _)).Times(kPromCalls);

  // Testing reset.
  EXPECT_CALL(i2c,
              DoWriteReadFor(Property(&pw::i2c::Address::GetSevenBit, 0x76),
                             ElementsAreArray(command), _, _))
      .WillOnce(Return(pw::OkStatus()));
  Ms5837 ms5837 = MakeMs5837(i2c).value();
  EXPECT_EQ(ms5837.Reset(), pw::OkStatus());
}

class Ms5837Mock : public Ms5837 {
 public:
  Ms5837Mock(pw::i2c::Initiator& i2c,
             internal::CalibrationData calibration_data)
      : Ms5837(i2c, calibration_data) {}
};

TEST(Ms5837, ReadTemperatureAndPressure) {
  pw::i2c::GmockInitiator i2c;

  // Testing read.
  EXPECT_CALL(i2c, DoWriteReadFor(_, _, _, _))
      .WillOnce(Return(pw::OkStatus()))
      .WillOnce([]([[maybe_unused]] auto address, [[maybe_unused]] auto write,
                   pw::ByteSpan read, [[maybe_unused]] auto for_at_least) {
        // 6815414 as 24bit unsigned little endian. Refer to conversion_test.cc
        // for more info.
        constexpr auto kRawTemperature = pw::bytes::Array<0xB6, 0xFE, 0x67>();
        std::copy(kRawTemperature.begin(), kRawTemperature.end(), read.begin());
        return pw::OkStatus();
      })
      .WillOnce(Return(pw::OkStatus()))
      .WillOnce([]([[maybe_unused]] auto address, [[maybe_unused]] auto write,
                   auto read, [[maybe_unused]] auto for_at_least) {
        // 4958179 as 24bit unsigned little endian. Refer to conversion_test.cc
        // for more info.
        constexpr auto kRawPressure = pw::bytes::Array<0xE3, 0xA7, 0x4B>();
        std::copy(kRawPressure.begin(), kRawPressure.end(), read.begin());
        return pw::OkStatus();
      });

  // Setup ms5837.
  Ms5837Mock ms5837(
      i2c, {
               .pressure_sensitivity = 34982,
               .pressure_offset = 36352,
               .temperature_coefficient_of_pressure_sensitivity = 20328,
               .temperature_coefficient_of_pressure_offset = 22354,
               .reference_temperature = 26646,
               .temperature_coefficient_of_temperature = 26146,
           });
  // Testing read.
  auto [temperature, pressure] = ms5837.ReadTemperatureAndPressure().value();
  EXPECT_EQ(temperature, 2493);
  EXPECT_EQ(pressure, 39998);
}

}  // namespace tr::ms5837