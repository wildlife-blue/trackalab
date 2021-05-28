#pragma once
#include <array>
#include <cstddef>
#include <span>

#include "pw_bytes/byte_builder.h"

namespace tr::artic::internal {

constexpr size_t kStandardModeWriteCommandSize = 4;
typedef std::array<std::byte, kStandardModeWriteCommandSize>
    StandardModeRegisterCommand;

enum class BurstModeMemory {
  PROGRAM = 0b00000000,
  X = 0b00000010,
  Y = 0b00000100,
  IO = 0b00000110,
};

enum class TransactionMode {
  READ = 0x1,
  WRITE = 0x0,
};

template <uint8_t kAddress>
class StandardModeRegisterAddress;

// Generates a standard mode command, according to the Artic-R3 specification.
template <uint8_t kAddress>
StandardModeRegisterCommand StandardModeWriteCommand(
    const StandardModeRegisterAddress<kAddress> &address,
    std::span<const std::byte> artic_register) {
  std::array<std::byte, kStandardModeWriteCommandSize> result;
  // [A6, A5, A4, A3, A2, A1, A0, R/(NW)]
  result[0] = (address.address() << 1) | std::byte{0x00};
  auto result_register = std::as_writable_bytes(std::span(result))
                             .last(kStandardModeWriteCommandSize - 1);
  std::copy(artic_register.begin(), artic_register.end(),
            result_register.begin());
  return result;
}

// Creates a standard mode register address and checks that it is within the
// bounds of the Artic spec.
template <uint8_t kAddress>
class StandardModeRegisterAddress {
 public:
  constexpr StandardModeRegisterAddress() : address_(kAddress) {
    static_assert(kAddress < 0x8F);
  }

 private:
  std::byte address() const { return std::byte{address_}; }
  const uint8_t address_;

  friend StandardModeRegisterCommand StandardModeWriteCommand<kAddress>(
      const StandardModeRegisterAddress<kAddress> &address,
      std::span<const std::byte> artic_register);
};

template <BurstModeMemory kBurstMemoryMode, TransactionMode kTransactionMode>
StandardModeRegisterCommand SwitchToBurstModeCommand(uint16_t start_address) {
  std::array<std::byte, 3> register_contents;
  pw::ByteBuilder builder(register_contents);
  constexpr std::byte kBurstModeOnMask = std::byte{0b00001000};
  std::byte config = static_cast<std::byte>(kBurstMemoryMode) |
                     static_cast<std::byte>(kTransactionMode) |
                     kBurstModeOnMask;
  builder.append(1, config);
  builder.PutUint16(start_address);
  constexpr auto kAddress = StandardModeRegisterAddress<0x00>();
  return StandardModeWriteCommand(kAddress, register_contents);
}

}  // namespace tr::artic::internal
