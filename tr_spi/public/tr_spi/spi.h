#pragma once
#include <span>

#include "pw_status/status.h"

namespace tr {
namespace spi {
class SpiInterface {
 public:
  virtual pw::Status Write(std::span<const std::byte> buffer) = 0;
};
}  // namespace spi
}  // namespace tr