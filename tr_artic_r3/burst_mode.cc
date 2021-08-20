#include "internal/burst_mode.h"

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

}  // namespace tr::artic::internal