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

  ~TapeSort();

  void sort();

 private:
  const std::string tmp_data_dir_ = "tmp";
  size_t block_size_;
  size_t max_buffer_size_;
  size_t max_number_merge_candidates_;
  std::shared_ptr<TapeCreator> tape_generator_;
  std::shared_ptr<Tape> input_tape_;
  std::shared_ptr<Tape> output_tape_;

  size_t generateSortedTemporaryTapes();

  bool merge(std::vector<size_t> const& merge_candidates_id, size_t merge_tape_id);

  std::unique_ptr<Tape> openTape(size_t tape_id) const;
};
}  // namespace tapetools