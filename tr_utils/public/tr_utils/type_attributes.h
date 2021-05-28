#pragma once

namespace tr::utils {

class NonCopyable {
 public:
  NonCopyable() = default;
  ~NonCopyable() = default;
  NonCopyable(const NonCopyable&) = delete;
  NonCopyable& operator=(const NonCopyable&) = delete;
};

}  // namespace tr::utils