#include "internal/burst_mode.h"

#include "pw_bytes/endian.h"

namespace tr::artic::internal {
pw::Result<pw::ConstByteSpan> BurstMode::Read(size_t size, uint16_t address,
                                              pw::ByteSpan register_buffer) {
  if (register_buffer.size() < kBurstRegisterWordSize * size) {
    return pw::Status::FailedPrecondition();
  }
  StandardModeRegisterCommand command =
      SwitchToBurstModeCommand<BurstModeMemory::PROGRAM,
                               TransactionMode::WRITE>(address);
  pw::Status status = spi_.Write(command);
  if (status != pw::OkStatus()) {
    return status;
  }
  pw::Result<pw::ConstByteSpan> result =
      spi_.Read(register_buffer.first(kBurstRegisterWordSize * size));

  if (!interrupt1_.IsHigh() || interrupt2_.IsHigh()) {
    return pw::Status::Unknown();
  }
  return result;
}

pw::Status BurstMode::Write(size_t size, uint16_t address,
                            pw::ConstByteSpan register_buffer) {
  if (register_buffer.size() > kBurstRegisterWordSize * size) {
    return pw::Status::FailedPrecondition();
  }
  StandardModeRegisterCommand command =
      SwitchToBurstModeCommand<BurstModeMemory::PROGRAM,
                               TransactionMode::WRITE>(address);
  pw::Status status = spi_.Write(command);
  if (status != pw::OkStatus()) {
    return status;
  }
  status = spi_.Write(register_buffer);
  if (status != pw::OkStatus()) {
    return status;
  }
  if (!interrupt1_.IsHigh() || interrupt2_.IsHigh()) {
    return pw::Status::Unknown();
  }
  return pw::OkStatus();
}

pw::Result<std::string_view> FirmwareVersion::VersionAsString() {
  buffer_t buffer;
  pw::Result<pw::ConstByteSpan> version_buffer = GetVersionBuffer(buffer);
  if (version_buffer.status() != pw::OkStatus()) {
    return version_buffer.status();
  }
  pw::ConstByteSpan version = version_buffer.value();
  constexpr size_t kSizeofAsciiString = 24;
  return std::string_view(reinterpret_cast<const char *>(version.data()),
                          kSizeofAsciiString);
}

pw::Result<uint8_t> FirmwareVersion::GetMajorVersion() {
  buffer_t buffer;
  pw::Result<pw::ConstByteSpan> version_buffer = GetVersionBuffer(buffer);
  if (version_buffer.status() != pw::OkStatus()) {
    return version_buffer.status();
  }
  pw::ConstByteSpan version = version_buffer.value();
  constexpr size_t kMajorVersionIndex = 24;
  return static_cast<uint8_t>(version[kMajorVersionIndex]);
}

pw::Result<uint8_t> FirmwareVersion::GetMinorVersion() {
  buffer_t buffer;
  pw::Result<pw::ConstByteSpan> version_buffer = GetVersionBuffer(buffer);
  if (version_buffer.status() != pw::OkStatus()) {
    return version_buffer.status();
  }
  pw::ConstByteSpan version = version_buffer.value();
  constexpr size_t kMinorVersionIndex = 25;
  return static_cast<uint8_t>(version[kMinorVersionIndex]);
}

pw::Result<uint8_t> FirmwareVersion::GetPatchVersion() {
  buffer_t buffer;
  pw::Result<pw::ConstByteSpan> version_buffer = GetVersionBuffer(buffer);
  if (version_buffer.status() != pw::OkStatus()) {
    return version_buffer.status();
  }
  pw::ConstByteSpan version = version_buffer.value();
  constexpr size_t kPatchIndex = 26;
  return static_cast<uint8_t>(version[kPatchIndex]);
}

pw::Result<pw::ConstByteSpan> FirmwareVersion::GetVersionBuffer(
    std::span<std::byte> buffer) {
  pw::Result<pw::ConstByteSpan> read =
      burst_mode_.Read<BurstRegisterID::FIRMWARE_VERSION>(
          std::as_writable_bytes(buffer));
  if (read.status() != pw::OkStatus()) {
    return read.status();
  }
  return read.value();
}

pw::Result<RxModeConfig> ArgosConfiguration::GetRxMode() {
  ArgosConfigurationRegister config_register = {0};
  pw::Result<pw::ConstByteSpan> read =
      burst_mode_.Read<BurstRegisterID::ARGOS_CONFIG>(
          std::as_writable_bytes(std::span(&config_register, 1)));
  if (read.status() != pw::OkStatus()) {
    return read.status();
  }
  return config_register.rx_configuration;
}

pw::Result<TxModeConfig> ArgosConfiguration::GetTxMode() {
  ArgosConfigurationRegister config_register = {0};
  pw::Result<pw::ConstByteSpan> read =
      burst_mode_.Read<BurstRegisterID::ARGOS_CONFIG>(
          std::as_writable_bytes(std::span(&config_register, 1)));
  if (read.status() != pw::OkStatus()) {
    return read.status();
  }
  return config_register.tx_configuration;
}

pw::Result<pw::ConstByteSpan> BuildArgos2PTTPayload(uint32_t id_number,
                                                    pw::ConstByteSpan payload,
                                                    pw::ByteSpan destination) {
  static constexpr uint32_t kBitsInAByte = 8;
  static constexpr uint32_t kPreambleSize = 4;   // bytes
  static constexpr uint32_t kArticWordSize = 3;  // bytes
  static constexpr uint32_t kArgosWordSize = 4;  // bytes
  static constexpr uint32_t kIdSize = 1;
  static constexpr uint8_t kMaxPayloadArgosWords = 0x08;
  // TODO(#8): Tighten this constraint, this currently checks for max
  // size.
  if (destination.size() <
      kPreambleSize + kMaxPayloadArgosWords * kArgosWordSize) {
    return pw::Status::FailedPrecondition();
  }
  uint32_t encoded_length_bits =
      kBitsInAByte * (payload.size() + kPreambleSize);
  uint32_t message_words =
      RoundUpIntegerDivision(payload.size(), kArgosWordSize);

  if (message_words > kMaxPayloadArgosWords) {
    return pw::Status::FailedPrecondition();
  }

  // A4-SYS-IF-0086-CNES_0107 - SERVICES AND MESSAGE FORMATS sec:4.2.1
  // n = N - 1
  uint8_t parity = Parity(message_words - 1);
  uint8_t encoded_message_length_with_parity =
      ((message_words - 1U) << 1U) | parity;

  pw::ByteBuilder builder(destination);
  auto encoded_message_length =
      pw::bytes::CopyInOrder(std::endian::big, encoded_length_bits);
  constexpr uint8_t kIDBitSize = 28;
  uint32_t message_length_and_id =
      static_cast<uint32_t>(encoded_message_length_with_parity << kIDBitSize) |
      id_number;
  builder.append(std::span(encoded_message_length).last(kArticWordSize))
      .PutUint32(message_length_and_id, std::endian::big)
      .append(payload);
  uint32_t number_of_padding_bytes =
      RoundUpIntegerDivision(builder.size(), kArticWordSize) * kArticWordSize -
      builder.size();
  if (number_of_padding_bytes > 0) {
    builder.append(number_of_padding_bytes, std::byte{0});
  }
  if (builder.status() != pw::OkStatus()) {
    return builder.status();
  }

  return pw::ConstByteSpan(builder);
}

}  // namespace tr::artic::internal