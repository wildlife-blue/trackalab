#pragma once
#include <cstdint>

#include "internal/standard_mode.h"
#include "pw_status/status.h"
#include "tr_gpio/gpio.h"
#include "tr_spi/spi.h"

namespace tr::artic::internal {

struct BurstRegister {
  static constexpr size_t word_size = 3;  // bytes
  // Burst mode start address
  const uint16_t address;
  // Size in 24bit words
  const std::size_t size;
};

// clang-format off
// // ARTIC R3 Datasheet, Table: 17
constexpr BurstRegister kCrcResultsRegister                = {.address{0x1DA9}, .size{8}};
constexpr BurstRegister kArgosConfigRegister               = {.address{0x1C00}, .size{1}};
constexpr BurstRegister kRxTimeoutRegister                 = {.address{0x1C01}, .size{1}};
constexpr BurstRegister kSatelliteDetectionTimeoutRegister = {.address{0x1C02}, .size{1}};
constexpr BurstRegister kTcxoWarmupTimeRegister            = {.address{0x1C03}, .size{1}};
constexpr BurstRegister kRxPayloadRegister                 = {.address{0x1C04}, .size{9}};
constexpr BurstRegister kRxFilteringConfigRegister         = {.address{0x1C0D}, .size{101}};
constexpr BurstRegister kTxPayloadRegister                 = {.address{0x1C72}, .size{220}};
constexpr BurstRegister kTxcoControlRegister               = {.address{0x1D4E}, .size{1}};
constexpr BurstRegister kTxFrequencyArgosRegister          = {.address{0x1D50}, .size{1}};
constexpr BurstRegister kExceptionInfoRegister             = {.address{0x1D51}, .size{1}};
constexpr BurstRegister kFirmwareVersionRegister           = {.address{0x1D52}, .size{9}};
constexpr BurstRegister kArgosIdRegister                   = {.address{0x1D5B}, .size{2}};
// clang-format on

class BurstMode {
 public:
  BurstMode(spi::SpiInterface &spi, gpio::GpiInterface &interrupt1,
            gpio::GpiInterface &interrupt2)
      : spi_(spi), interrupt1_(interrupt1), interrupt2_(interrupt2) {}

  // Reads a register in burst mode.
  pw::Result<pw::ConstByteSpan> Read(const BurstRegister &burst_register,
                                     pw::ByteSpan register_buffer);
  // Writes a register in burst mode.
  pw::Status Write(const BurstRegister &burst_register,
                   pw::ConstByteSpan register_buffer);

 private:
  spi::SpiInterface &spi_;
  gpio::GpiInterface &interrupt1_;
  gpio::GpiInterface &interrupt2_;
};

}  // namespace tr::artic::internal