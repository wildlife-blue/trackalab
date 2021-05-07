#pragma once

namespace tr {
namespace gpio {

class GpiInterface {
 public:
  virtual bool IsHigh() = 0;
};

class GpoInterface {
 public:
  virtual void Write(bool) = 0;
};

}  // namespace gpio
}  // namespace tr