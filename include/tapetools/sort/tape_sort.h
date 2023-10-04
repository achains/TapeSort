#pragma once

#include <memory>
#include <queue>
#include <vector>

#include "tapetools/tape.h"

namespace tapetools {
class TapeSort {
 public:
  TapeSort(const Tape* input_tape);

  void sort();

 private:
  struct SortedBlock {
   public:
    SortedBlock(std::vector<int> const& raw_data);

    bool hasNext() const;

    int getNext();

   private:
    size_t current_element_id = 0;
    std::vector<int> data_;
  };

  size_t block_size_;
  size_t max_buffer_size_;
  std::priority_queue<std::unique_ptr<SortedBlock>> buffer_queue_;
  std::shared_ptr<Tape> input_tape_;

  void fillBuffer();

  void writeSortedBlock(std::vector<int> const& sorted_block);
};
}  // namespace tapetools