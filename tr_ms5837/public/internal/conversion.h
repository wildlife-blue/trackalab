#pragma once
#include <cstdint>

#include "pw_preprocessor/compiler.h"

namespace tr::ms5837::internal {

PW_PACKED(struct) CalibrationData {
  uint16_t pressure_sensitivity;
  uint16_t pressure_offset;
  uint16_t temperature_coefficient_of_pressure_sensitivity;
  uint16_t temperature_coefficient_of_pressure_offset;
  uint16_t reference_temperature;
  uint16_t temperature_coefficient_of_temperature;
};

struct NormalisedOutput {
  int32_t temperature;
  int32_t pressure;
};

constexpr uint32_t ExpBase2(uint32_t exponent) {
  uint32_t result = 1;
  for (uint32_t i = 0; i < exponent; ++i) {
    result *= 2;
  }
  return result;
}

class Conversion {
 public:
  Conversion(const CalibrationData& calibration_data)
      : calibration_data_(calibration_data) {}

  NormalisedOutput Convert(uint32_t raw_pressure,
                           uint32_t raw_temperature) const {
    int32_t dT =
        raw_temperature - calibration_data_.reference_temperature * ExpBase2(8);

    int32_t temp =
        2000 + dT * calibration_data_.temperature_coefficient_of_temperature /
                   ExpBase2(23);
    int64_t offset =
        (int64_t)calibration_data_.pressure_offset * (int64_t)ExpBase2(16) +
        (int64_t)calibration_data_.temperature_coefficient_of_pressure_offset *
            (int64_t)dT / (int64_t)ExpBase2(7);
    int64_t sens = calibration_data_.pressure_sensitivity * ExpBase2(15) +
                   (int64_t)calibration_data_
                           .temperature_coefficient_of_pressure_sensitivity *
                       dT / ExpBase2(8);
    int32_t pressure =
        (raw_pressure * sens / ExpBase2(21) - offset) / ExpBase2(13);
    // TODO(#12): Consider using second order calculation as described in the
    // datasheet. Reference: TE CONNECTIVITY SENSORS /// ENG_DS_MS5837-30BA_C2,
    // pg: 12.
    return {temp, pressure};
  }

 private:
  const CalibrationData& calibration_data_;
};

}  // namespace tr::ms5837::internal