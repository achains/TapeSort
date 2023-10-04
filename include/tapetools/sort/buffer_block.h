#pragma once

#include <cstddef>
#include <vector>

namespace tapetools {
class BufferBlock {
 public:
  BufferBlock(std::vector<int> const& raw_data);

  bool hasNext() const;

  int getNext();

 private:
  size_t current_pos_ = 0;
  std::vector<int> data_;
};
}  // namespace tapetools