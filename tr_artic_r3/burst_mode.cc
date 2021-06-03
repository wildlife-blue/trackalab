#include "internal/burst_mode.h"

namespace tr::artic::internal {
pw::Result<pw::ConstByteSpan> BurstMode::Read(
    const BurstRegister &burst_register, pw::ByteSpan register_buffer) {
  if (register_buffer.size() < BurstRegister::word_size * burst_register.size) {
    return pw::Status::FailedPrecondition();
  }
  StandardModeRegisterCommand command =
      SwitchToBurstModeCommand<BurstModeMemory::PROGRAM,
                               TransactionMode::WRITE>(burst_register.address);
  pw::Status status = spi_.Write(command);
  if (status != pw::OkStatus()) {
    return status;
  }
  pw::Result<pw::ConstByteSpan> result = spi_.Read(
      register_buffer.first(BurstRegister::word_size * burst_register.size));

  if (!interrupt1_.IsHigh() || interrupt2_.IsHigh()) {
    return pw::Status::Unknown();
  }
  return result;
}

pw::Status BurstMode::Write(const BurstRegister &burst_register,
                            pw::ConstByteSpan register_buffer) {
  if (register_buffer.size() > BurstRegister::word_size * burst_register.size) {
    return pw::Status::FailedPrecondition();
  }
  StandardModeRegisterCommand command =
      SwitchToBurstModeCommand<BurstModeMemory::PROGRAM,
                               TransactionMode::WRITE>(burst_register.address);
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