#pragma once

#include <memory>
#include <queue>
#include <vector>

#include "tapetools/tape.h"
#include "tapetools/tape_creator.h"

namespace tapetools {
class TapeSort {
 public:
  TapeSort(std::shared_ptr<Tape> input_tape, std::shared_ptr<Tape> output_tape,
           std::shared_ptr<TapeCreator> tape_generator, size_t block_size, size_t max_buffer_size);

  void sort();

 private:
  size_t block_size_;
  size_t max_buffer_size_;
  size_t max_number_merge_candidates_;
  std::shared_ptr<TapeCreator> tape_generator_;
  std::shared_ptr<Tape> input_tape_;
  std::shared_ptr<Tape> output_tape_;

  size_t generateSortedTemporaryTapes();

  bool merge(std::vector<size_t> const& merge_candidates_id, Tape* merged_tape);
};
}  // namespace tapetools