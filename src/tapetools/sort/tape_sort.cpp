#include "tapetools/sort/tape_sort.h"

#include <algorithm>
#include <filesystem>
#include <utility>

namespace tapetools {

TapeSort::TapeSort(std::shared_ptr<Tape> input_tape, std::shared_ptr<Tape> output_tape, size_t max_buffer_size)
    : input_tape_(std::move(input_tape)), output_tape_(std::move(output_tape)), max_buffer_size_(max_buffer_size) {}

void TapeSort::sort() {}

void TapeSort::writeSortedBlock(std::vector<int> const& sorted_block) {
  output_tape_->writeBlock(sorted_block.data(), sorted_block.size());
}
}  // namespace tapetools