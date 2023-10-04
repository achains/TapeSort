#include "tapetools/sort/tape_sort.h"

#include <algorithm>

namespace tapetools {

TapeSort::TapeSort(Tape* input_tape, Tape* output_tape, size_t block_size, size_t max_buffer_size)
    : input_tape_(input_tape), output_tape_(output_tape), block_size_(block_size), max_buffer_size_(max_buffer_size) {}

void TapeSort::sort() {
  // Load K sorted blocks of size B into buffer
  fillBuffer();

  std::vector<int> output_block;
  output_block.reserve(block_size_);

  while (!buffer_queue_.empty()) {
    if (buffer_queue_.top()->hasNext()) {
      output_block.push_back(buffer_queue_.top()->getNext());
      if (output_block.size() == block_size_) {
        writeSortedBlock(output_block);
        output_block.clear();
      }
    } else {
      // Update buffer with new block from Tape
      buffer_queue_.pop();
      fillBuffer();
    }
  }
}

void TapeSort::fillBuffer() {
  while (buffer_queue_.size() < max_buffer_size_) {
    std::vector<int> tmp_block_data(block_size_);
    size_t values_count = input_tape_->readBlock(tmp_block_data.data(), block_size_);
    if (values_count == 0) {
      return;
    }

    // TODO: Create buffer queue values sort invariant
    std::sort(tmp_block_data.begin(), tmp_block_data.end());
    buffer_queue_.emplace(std::make_unique<BufferBlock>(tmp_block_data));
  }
}

void TapeSort::writeSortedBlock(std::vector<int> const& sorted_block) {
  output_tape_->writeBlock(sorted_block.data(), sorted_block.size());
}
}  // namespace tapetools