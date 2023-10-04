#pragma once

#include <cstddef>

namespace tapetools {
class Tape {
 public:
  virtual size_t readBlock(int* block_ptr, size_t block_size) = 0;
};
}  // namespace tapetools