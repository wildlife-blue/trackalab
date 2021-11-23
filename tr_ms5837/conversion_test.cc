#include "internal/conversion.h"

#include "gtest/gtest.h"

namespace tr::ms5837::internal {

TEST(ExpBase2, Basic) {
  EXPECT_EQ(256, ExpBase2(8));
  EXPECT_EQ(8388608, ExpBase2(23));
}

TEST(ConversionTest, TestConvert) {
  // Examples values taken from datasheet.
  CalibrationData calibration_data = {
      .pressure_sensitivity = 34982,
      .pressure_offset = 36352,
      .temperature_coefficient_of_pressure_sensitivity = 20328,
      .temperature_coefficient_of_pressure_offset = 22354,
      .reference_temperature = 26646,
      .temperature_coefficient_of_temperature = 26146,
  };

  NormalisedOutput normalised_output =
      Conversion(calibration_data).Convert(4958179, 6815414);
  EXPECT_EQ(normalised_output.pressure, 39998);
  EXPECT_EQ(normalised_output.temperature, 2493);
}

}  // namespace tr::ms5837::internal