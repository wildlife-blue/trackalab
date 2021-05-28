#pragma once
#include <span>

#include "pw_bytes/span.h"
#include "pw_result/result.h"
#include "pw_status/status.h"
#include "tr_utils/type_attributes.h"

namespace tr {
namespace spi {
class SpiInterface : public tr::utils::NonCopyable {
 public:
  virtual pw::Status Write(pw::ConstByteSpan buffer) = 0;
  virtual pw::Result<pw::ConstByteSpan> Read(pw::ByteSpan buffer) = 0;
};
}  // namespace spi
}  // namespace tr