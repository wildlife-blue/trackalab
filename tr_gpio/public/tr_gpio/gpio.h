#pragma once
#include "tr_utils/type_attributes.h"

namespace tr {
namespace gpio {

class GpiInterface : public tr::utils::NonCopyable {
 public:
  virtual bool IsHigh() = 0;
};

class GpoInterface : public tr::utils::NonCopyable {
 public:
  virtual void Write(bool) = 0;
};

}  // namespace gpio
}  // namespace tr