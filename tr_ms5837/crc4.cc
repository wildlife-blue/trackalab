
#include <iostream>
#include <span>

#include "pw_assert/check.h"

namespace tr::ms5837::internal {

#ifdef __cplusplus
extern "C" {
#endif

// TODO(#12): This is a modified version of the crc4 algorithm from the
// datasheet.
uint8_t crc4(uint16_t n_prom[]) {
  uint16_t n_rem = 0;

  // NOLINTNEXTLINE
  n_prom[7] = 0;

  // NOLINTNEXTLINE
  for (uint8_t i = 0; i < 16; i++) {
    if (i % 2 == 1) {
      // NOLINTNEXTLINE
      n_rem ^= (uint16_t)((n_prom[i >> 1]) & 0x00FF);
    } else {
      // NOLINTNEXTLINE
      n_rem ^= (uint16_t)(n_prom[i >> 1] >> 8);
    }
    // NOLINTNEXTLINE
    for (uint8_t n_bit = 8; n_bit > 0; n_bit--) {
      // NOLINTNEXTLINE
      if (n_rem & 0x8000) {
        // NOLINTNEXTLINE
        n_rem = (n_rem << 1) ^ 0x3000;
      } else {
        // NOLINTNEXTLINE
        n_rem = (n_rem << 1);
      }
    }
  }

  // NOLINTNEXTLINE
  n_rem = ((n_rem >> 12) & 0x000F);

  // NOLINTNEXTLINE
  return n_rem ^ 0x00;
}

#ifdef __cplusplus
}
#endif

std::byte Crc4(std::span<const std::byte> data) {
  constexpr size_t kSize = 8;
  // NOLINTNEXTLINE(hicpp-no-array-decay,hicpp-vararg)
  PW_CHECK(data.size() >= kSize);
  std::array<uint16_t, kSize> n_prom = {0};
  std::span<std::byte> prom_data = std::as_writable_bytes(std::span(n_prom));
  std::copy(data.begin(), data.end(), prom_data.begin());
  return std::byte{crc4(n_prom.data())};
}
}  // namespace tr::ms5837::internal