#include "internal/prom.h"

#include "tr_ms5837/ms5837.h"

namespace tr::ms5837::internal {
pw::Result<CalibrationData> PromRead(pw::i2c::Initiator &initiator) {
  using namespace std::chrono_literals;
  constexpr pw::i2c::Address kAddress = pw::i2c::Address::SevenBit<0x76>();
  constexpr size_t kPromSize = 7;
  std::array<uint16_t, kPromSize> prom_buffer = {0};
  pw::i2c::RegisterDevice prom(initiator, kAddress, std::endian::little,
                               pw::i2c::RegisterAddressSize::k1Byte);

  for (uint32_t i = 0; i < prom_buffer.size(); ++i) {
    pw::Result<uint16_t> reg =
        prom.ReadRegister16(PromCommand(i), kReadWriteTimeout);
    if (!reg.ok()) {
      return reg.status();
    }
    prom_buffer[i] = reg.value();
  }

  constexpr uint16_t kCrcBitMask = 0xF000;
  constexpr uint8_t kCrcBitShift = 12;
  auto crc4 = static_cast<std::byte>(
      static_cast<uint32_t>(prom_buffer[0U] & kCrcBitMask) >> kCrcBitShift);

  constexpr uint16_t kValueMask = 0x0FFF;
  prom_buffer[0] &= kValueMask;
  if (crc4 != Crc4(std::as_bytes(std::span(prom_buffer)))) {
    return pw::Status::DataLoss();
  }

  constexpr size_t kCalibrationDataSize = 6;
  std::span<const std::byte> only_calibration_data =
      std::as_bytes(std::span(prom_buffer).last(kCalibrationDataSize));

  CalibrationData calibration_data = {0};
  std::copy(only_calibration_data.begin(), only_calibration_data.end(),
            std::as_writable_bytes(std::span(&calibration_data, 1)).begin());
  return calibration_data;
}
}  // namespace tr::ms5837::internal