#include "tr_ms5837/ms5837.h"

#include "pw_bytes/byte_builder.h"

namespace tr::ms5837 {

pw::Result<Ms5837> MakeMs5837(pw::i2c::Initiator& i2c) {
  pw::Result<internal::CalibrationData> calibration_data =
      internal::PromRead(i2c);
  if (!calibration_data.ok()) {
    return calibration_data.status();
  }
  return Ms5837(i2c, calibration_data.value());
}

pw::Status Ms5837::Reset() {
  constexpr auto kResetCommand = pw::bytes::Array<0x1E>();
  return i2c_.WriteFor(kAddress, kResetCommand, kReadWriteTimeout);
}

pw::Result<uint32_t> Ms5837::GetRaw(std::byte conversion_command) {
  pw::Status status = i2c_.WriteFor(
      kAddress, pw::ConstByteSpan(&conversion_command, 1), kReadWriteTimeout);
  if (!status.ok()) {
    return status;
  }

  std::array<std::byte, sizeof(uint32_t)> adc_buffer = {std::byte{0}};
  constexpr size_t kSizeofUint24 = 3;
  constexpr auto kReadAdcCommand = pw::bytes::Array<0x00>();
  status = i2c_.WriteReadFor(kAddress, kReadAdcCommand,
                             std::span(adc_buffer).last(kSizeofUint24),
                             kReadWriteTimeout);
  if (!status.ok()) {
    return status;
  }
  auto a =
      static_cast<uint32_t>(pw::ByteBuilder(adc_buffer).begin().ReadUint32()) >>
      8U;
  return a;
}

}  // namespace tr::ms5837