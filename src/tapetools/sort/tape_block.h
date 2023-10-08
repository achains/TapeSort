#pragma once

#include <memory>
#include <vector>

#include "tapetools/tape.h"

namespace tapetools {
class TapeBlock {
 public:
  TapeBlock(std::unique_ptr<Tape> tape, size_t block_size);

  bool updateBlock();

  size_t getSize() const;

  int getValue(size_t i) const;

 private:
  size_t current_size_ = 0;
  size_t max_block_size_;
  std::vector<int> data_;
  std::unique_ptr<Tape> tape_;
};
}  // namespace tapetools