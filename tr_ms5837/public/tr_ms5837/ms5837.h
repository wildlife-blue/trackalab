#pragma once
#include <chrono>

#include "internal/conversion.h"
#include "internal/prom.h"
#include "pw_i2c/initiator.h"
#include "pw_result/result.h"

namespace tr::ms5837 {

constexpr auto kReadWriteTimeout = std::chrono::milliseconds(50);

typedef internal::NormalisedOutput TemperatureAndPressure;

class Ms5837 {
 private:
  static constexpr pw::i2c::Address kAddress =
      pw::i2c::Address::SevenBit<0x76>();

 protected:
  constexpr Ms5837(pw::i2c::Initiator& i2c,
                   internal::CalibrationData calibration_data)
      : i2c_(i2c), calibration_data_(calibration_data) {}

 public:
  // Resets the sensor.
  pw::Status Reset();

  // Reads the temperature and pressure.
  template <uint16_t oversampling_ratio = 256>
  pw::Result<TemperatureAndPressure> ReadTemperatureAndPressure() {
    pw::Result<uint32_t> raw_temperature =
        GetRawTemperature<oversampling_ratio>();
    if (!raw_temperature.ok()) {
      return raw_temperature.status();
    }
    pw::Result<uint32_t> raw_pressure = GetRawPressure<oversampling_ratio>();
    if (!raw_pressure.ok()) {
      return raw_pressure.status();
    }
    return internal::Conversion(calibration_data_)
        .Convert(raw_pressure.value(), raw_temperature.value());
  }

 private:
  // Checks if the oversampling ratio is valid.
  template <uint16_t oversampling_ratio>
  constexpr void static_assert_oversampling_ratio() {
    static_assert(oversampling_ratio == 256 || oversampling_ratio == 512 ||
                      oversampling_ratio == 1024 ||
                      oversampling_ratio == 2048 || oversampling_ratio == 4096,
                  "Supported oversampling ratios are 256, 512, 1024, 2048, "
                  "4096.");
  }
  template <uint16_t oversampling_ratio>
  constexpr std::byte oversampling_ratio_to_register_nibble() {
    static_assert_oversampling_ratio<oversampling_ratio>();
    for (uint8_t i = 0; i < 16; i++) {
      if (oversampling_ratio / 256 & (1U << i)) {
        return std::byte{i * 2};
      }
    }
    return std::byte{0};
  }

  // Reads the raw temperature value.
  template <uint16_t oversampling_ratio>
  pw::Result<uint32_t> GetRawTemperature() {
    return GetRaw(std::byte{0x50} |
                  oversampling_ratio_to_register_nibble<oversampling_ratio>());
  }

  // Reads the raw pressure value.
  template <uint16_t oversampling_ratio>
  pw::Result<uint32_t> GetRawPressure() {
    return GetRaw(std::byte{0x40} |
                  oversampling_ratio_to_register_nibble<oversampling_ratio>());
  }

  // Gets raw data from the ADC.
  pw::Result<uint32_t> GetRaw(std::byte conversion_command);
  friend pw::Result<Ms5837> MakeMs5837(pw::i2c::Initiator& i2c);

  pw::i2c::Initiator& i2c_;
  internal::CalibrationData calibration_data_;
};

pw::Result<Ms5837> MakeMs5837(pw::i2c::Initiator& i2c);
}  // namespace tr::ms5837