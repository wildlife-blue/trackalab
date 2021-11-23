#include "internal/prom.h"

#include "gtest/gtest.h"
#include "pw_bytes/array.h"
#include "pw_i2c/initiator_gmock.h"
#include "pw_result/result.h"

namespace tr::ms5837::internal {

TEST(PROM, PromCommand) {
  EXPECT_EQ(PromCommand(0), 0xA0);
  EXPECT_EQ(PromCommand(7), 0xAE);
}

using ::testing::_;

TEST(PROM, ReadCalibrationData) {
  pw::i2c::GmockInitiator i2c;
  constexpr auto kCrcAndFactorySettings = pw::bytes::Array<0b01000000, 0xE0>();
  constexpr auto kC1 = pw::bytes::Array<0x1, 0x2>();
  constexpr auto kC2 = pw::bytes::Array<0x3, 0x4>();
  constexpr auto kC3 = pw::bytes::Array<0x5, 0x6>();
  constexpr auto kC4 = pw::bytes::Array<0x7, 0x8>();
  constexpr auto kC5 = pw::bytes::Array<0x9, 0x0A>();
  constexpr auto kC6 = pw::bytes::Array<0x0B, 0x0C>();

  EXPECT_CALL(i2c, DoWriteReadFor(_, _, _, _))
      .WillOnce([&]([[maybe_unused]] auto address, [[maybe_unused]] auto write,
                    auto read, [[maybe_unused]] auto for_at_least) {
        std::copy(kCrcAndFactorySettings.begin(), kCrcAndFactorySettings.end(),
                  read.begin());
        return pw::OkStatus();
      })
      .WillOnce([&]([[maybe_unused]] auto address, [[maybe_unused]] auto write,
                    auto read, [[maybe_unused]] auto for_at_least) {
        std::copy(kC1.begin(), kC1.end(), read.begin());
        return pw::OkStatus();
      })
      .WillOnce([&]([[maybe_unused]] auto address, [[maybe_unused]] auto write,
                    auto read, [[maybe_unused]] auto for_at_least) {
        std::copy(kC2.begin(), kC2.end(), read.begin());
        return pw::OkStatus();
      })
      .WillOnce([&]([[maybe_unused]] auto address, [[maybe_unused]] auto write,
                    auto read, [[maybe_unused]] auto for_at_least) {
        std::copy(kC3.begin(), kC3.end(), read.begin());
        return pw::OkStatus();
      })
      .WillOnce([&]([[maybe_unused]] auto address, [[maybe_unused]] auto write,
                    auto read, [[maybe_unused]] auto for_at_least) {
        std::copy(kC4.begin(), kC4.end(), read.begin());
        return pw::OkStatus();
      })
      .WillOnce([&]([[maybe_unused]] auto address, [[maybe_unused]] auto write,
                    auto read, [[maybe_unused]] auto for_at_least) {
        std::copy(kC5.begin(), kC5.end(), read.begin());
        return pw::OkStatus();
      })
      .WillOnce([&]([[maybe_unused]] auto address, [[maybe_unused]] auto write,
                    auto read, [[maybe_unused]] auto for_at_least) {
        std::copy(kC6.begin(), kC6.end(), read.begin());
        return pw::OkStatus();
      });

  pw::Result<CalibrationData> got_calibration = PromRead(i2c);
  EXPECT_EQ(got_calibration.status(), pw::OkStatus());

  CalibrationData expected_calibration = {
      .pressure_sensitivity = 0x0201,
      .pressure_offset = 0x0403,
      .temperature_coefficient_of_pressure_sensitivity = 0x0605,
      .temperature_coefficient_of_pressure_offset = 0x0807,
      .reference_temperature = 0x0A09,
      .temperature_coefficient_of_temperature = 0x0C0B};

  EXPECT_EQ(got_calibration.value().pressure_sensitivity,
            expected_calibration.pressure_sensitivity);
  EXPECT_EQ(got_calibration.value().pressure_offset,
            expected_calibration.pressure_offset);
  EXPECT_EQ(
      got_calibration.value().temperature_coefficient_of_pressure_sensitivity,
      expected_calibration.temperature_coefficient_of_pressure_sensitivity);
  EXPECT_EQ(got_calibration.value().temperature_coefficient_of_pressure_offset,
            expected_calibration.temperature_coefficient_of_pressure_offset);
  EXPECT_EQ(got_calibration.value().reference_temperature,
            expected_calibration.reference_temperature);
  EXPECT_EQ(got_calibration.value().temperature_coefficient_of_temperature,
            expected_calibration.temperature_coefficient_of_temperature);
}

}  // namespace tr::ms5837::internal