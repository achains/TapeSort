#pragma once

#include <memory>
#include <queue>
#include <vector>

#include "buffer_block.h"
#include "tapetools/tape.h"

namespace tapetools {
class TapeSort {
 public:
  TapeSort(const Tape* input_tape);

  void sort();

 private:
  size_t block_size_;
  size_t max_buffer_size_;
  std::priority_queue<std::unique_ptr<BufferBlock>> buffer_queue_;
  std::shared_ptr<Tape> input_tape_;

  void fillBuffer();

  void writeSortedBlock(std::vector<int> const& sorted_block);
};
}  // namespace tapetools