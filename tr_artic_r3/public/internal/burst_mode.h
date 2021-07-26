#pragma once
#include <cstdint>

#include "internal/standard_mode.h"
#include "pw_status/status.h"
#include "tr_gpio/gpio.h"
#include "tr_spi/spi.h"

namespace tr::artic::internal {

enum class Permission {
  READ,
  WRITE,
  READ_WRITE,
};

enum class BurstRegisterID {
  CRC_RESULTS,
  ARGOS_CONFIG,
  RX_TIMEOUT,
  SATELLITE_DETECTION_TIMEOUT,
  TCXO_WARMUP_TIME,
  RX_PAYLOAD,
  RX_FILTERING_CONFIG,
  TX_PAYLOAD,
  TCXO_CONTROL,
  TX_FREQUENCY_ARGOS,
  EXCEPTION_INFO,
  FIRMWARE_VERSION,
  ARGOS_ID,
};

static constexpr size_t kBurstRegisterWordSize = 3;  // bytes

template <BurstRegisterID id>
struct BurstRegister {
  // Burst mode start address
  const uint16_t address;
  // Size in 24bit words
  const std::size_t size;
  // Register permissions
  const Permission permission;

  constexpr BurstRegister();
};

// // ARTIC R3 Datasheet, Table: 17
template <>
constexpr BurstRegister<BurstRegisterID::CRC_RESULTS>::BurstRegister()
    : address(0x1DA9), size(8), permission(Permission::READ) {}
template <>
constexpr BurstRegister<BurstRegisterID::ARGOS_CONFIG>::BurstRegister()
    : address(0x1C00), size(1), permission(Permission::READ) {}
template <>
constexpr BurstRegister<BurstRegisterID::RX_TIMEOUT>::BurstRegister()
    : address(0x1C01), size(1), permission(Permission::READ_WRITE) {}
template <>
constexpr BurstRegister<
    BurstRegisterID::SATELLITE_DETECTION_TIMEOUT>::BurstRegister()
    : address(0x1C02), size(1), permission(Permission::READ_WRITE) {}
template <>
constexpr BurstRegister<BurstRegisterID::TCXO_WARMUP_TIME>::BurstRegister()
    : address(0x1C03), size(1), permission(Permission::READ_WRITE) {}
template <>
constexpr BurstRegister<BurstRegisterID::RX_PAYLOAD>::BurstRegister()
    : address(0x1C04), size(9), permission(Permission::READ) {}
template <>
constexpr BurstRegister<BurstRegisterID::RX_FILTERING_CONFIG>::BurstRegister()
    : address(0x1C0D), size(101), permission(Permission::WRITE) {}
template <>
constexpr BurstRegister<BurstRegisterID::TX_PAYLOAD>::BurstRegister()
    : address(0x1C72), size(220), permission(Permission::WRITE) {}
template <>
constexpr BurstRegister<BurstRegisterID::TCXO_CONTROL>::BurstRegister()
    : address(0x1D4E), size(1), permission(Permission::READ_WRITE) {}
template <>
constexpr BurstRegister<BurstRegisterID::TX_FREQUENCY_ARGOS>::BurstRegister()
    : address(0x1D50), size(1), permission(Permission::READ_WRITE) {}
template <>
constexpr BurstRegister<BurstRegisterID::EXCEPTION_INFO>::BurstRegister()
    : address(0x1D51), size(1), permission(Permission::READ_WRITE) {}
template <>
constexpr BurstRegister<BurstRegisterID::FIRMWARE_VERSION>::BurstRegister()
    : address(0x1D52), size(9), permission(Permission::READ) {}
template <>
constexpr BurstRegister<BurstRegisterID::ARGOS_ID>::BurstRegister()
    : address(0x1D5B), size(2), permission(Permission::READ) {}

class BurstMode {
 public:
  BurstMode(spi::SpiInterface &spi, gpio::GpiInterface &interrupt1,
            gpio::GpiInterface &interrupt2)
      : spi_(spi), interrupt1_(interrupt1), interrupt2_(interrupt2) {}

  // Reads a register in burst mode.
  template <BurstRegisterID id>
  pw::Result<pw::ConstByteSpan> Read(pw::ByteSpan register_buffer) {
    constexpr BurstRegister<id> burst_register;
    static_assert(burst_register.permission == Permission::READ ||
                      burst_register.permission == Permission::READ_WRITE,
                  "Read not supported for this register.");
    return Read(burst_register.size, burst_register.address, register_buffer);
  }

  // Writes a register in burst mode.
  template <BurstRegisterID id>
  pw::Status Write(pw::ConstByteSpan register_buffer) {
    constexpr BurstRegister<id> burst_register;
    static_assert(burst_register.permission == Permission::WRITE ||
                      burst_register.permission == Permission::READ_WRITE,
                  "Write not supported for this register.");
    return Write(burst_register.size, burst_register.address, register_buffer);
  }

 private:
  pw::Result<pw::ConstByteSpan> Read(size_t size, uint16_t address,
                                     pw::ByteSpan register_buffer);

  pw::Status Write(size_t size, uint16_t address,
                   pw::ConstByteSpan register_buffer);
  spi::SpiInterface &spi_;
  gpio::GpiInterface &interrupt1_;
  gpio::GpiInterface &interrupt2_;
};

}  // namespace tr::artic::internal