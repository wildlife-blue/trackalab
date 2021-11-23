#pragma once
#include <cstddef>
#include <span>

#include "internal/conversion.h"
#include "internal/crc4.h"
#include "pw_i2c/initiator.h"
#include "pw_i2c/register_device.h"
#include "pw_result/result.h"

namespace tr::ms5837::internal {

constexpr uint32_t PromCommand(uint8_t address) {
  return (address << 1) | 0xA0;
}

pw::Result<CalibrationData> PromRead(pw::i2c::Initiator &initiator);

}  // namespace tr::ms5837::internal