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
}  // namespace tr::artic::internal