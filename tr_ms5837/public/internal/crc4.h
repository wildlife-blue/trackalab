#pragma once
#include <cstdint>
#include <span>

namespace tr::ms5837::internal {

uint8_t crc4(uint16_t n_prom[]);
std::byte Crc4(std::span<const std::byte> data);
}  // namespace tr::ms5837::internal