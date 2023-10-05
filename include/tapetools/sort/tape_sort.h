#pragma once

#include <memory>
#include <queue>
#include <vector>

#include "buffer_block.h"
#include "tapetools/tape.h"

namespace tapetools {
class TapeSort {
 public:
  TapeSort(std::shared_ptr<Tape> input_tape, std::shared_ptr<Tape> output_tape, size_t max_buffer_size);

  void sort();

 private:
  size_t max_buffer_size_;
  std::priority_queue<std::unique_ptr<BufferBlock>> buffer_queue_;
  std::shared_ptr<Tape> input_tape_;
  std::shared_ptr<Tape> output_tape_;

  void writeSortedBlock(std::vector<int> const& sorted_block);
};
}  // namespace tapetools