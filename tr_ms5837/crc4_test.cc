#include "internal/crc4.h"

#include <array>

#include "gtest/gtest.h"
#include "pw_checksum/crc16_ccitt.h"

namespace tr::ms5837::internal {

TEST(CRC4, EquivelenceTest) {
  std::array<uint16_t, 8> input_buffer = {0x1012, 0x1234, 0x2345, 0x3456,
                                          0x4567, 0x5678, 0x6789, 0};

  constexpr std::byte kExpectedCrc{0x0E};
  EXPECT_EQ(kExpectedCrc,
            Crc4(std::as_bytes(std::span<uint16_t>(input_buffer)).first(14)));
}
}  // namespace tr::ms5837::internal