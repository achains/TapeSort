#include "tapetools/sort/tape_sort.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <utility>

#include "block_buffer.h"

namespace tapetools {

TapeSort::TapeSort(std::shared_ptr<Tape> input_tape, std::shared_ptr<Tape> output_tape,
                   std::shared_ptr<TapeCreator> tape_generator, size_t block_size, size_t max_buffer_size)
    : input_tape_(std::move(input_tape)),
      output_tape_(std::move(output_tape)),
      tape_generator_(std::move(tape_generator)),
      block_size_(block_size),
      max_buffer_size_(max_buffer_size),
      max_number_merge_candidates_(max_buffer_size_ / block_size_) {
  if (max_number_merge_candidates_ < 2) {
    throw std::runtime_error("Error. max_buffer_size should allow to store at least two blocks of size block_size");
  }
  std::filesystem::remove_all(tmp_data_dir_);
  std::filesystem::create_directory(tmp_data_dir_);
}

TapeSort::~TapeSort() { std::filesystem::remove_all(tmp_data_dir_); }

void TapeSort::sort() {
  // Divide input array on blocks of size max_buffer_size, sort them and store on temporary tapes
  size_t number_of_tapes_generated = generateSortedTemporaryTapes();

  // Merge candidates queue
  std::queue<size_t> tape_candidate_id_queue;
  for (size_t i = 0; i < number_of_tapes_generated; ++i) {
    tape_candidate_id_queue.push(i);
  }

  // Merge tapes while there are at least two unmerged tapes
  size_t merge_tape_id = number_of_tapes_generated;
  while (tape_candidate_id_queue.size() > 1) {
    std::vector<size_t> merge_candidates;
    while (merge_candidates.size() < max_number_merge_candidates_ && !tape_candidate_id_queue.empty()) {
      merge_candidates.push_back(tape_candidate_id_queue.front());
      tape_candidate_id_queue.pop();
    }
    merge(merge_candidates, merge_tape_id);
    tape_candidate_id_queue.push(merge_tape_id);
    ++merge_tape_id;
  }

  // Write to output
  size_t sorted_tape_id = tape_candidate_id_queue.front();
  auto sorted_tape = openTape(sorted_tape_id);

  std::vector<int> buffer(max_buffer_size_);
  size_t read_values_count;
  do {
    read_values_count = sorted_tape->readBlock(buffer.data(), max_buffer_size_);
    output_tape_->writeBlock(buffer.data(), read_values_count);
  } while (read_values_count != 0);
}

bool TapeSort::merge(std::vector<size_t> const& merge_candidates_id, size_t merge_tape_id) {
  // Tape pointers for corresponding buffer blocks
  std::vector<std::unique_ptr<Tape>> tapes_to_merge;
  for (size_t id : merge_candidates_id) {
    tapes_to_merge.emplace_back(openTape(id));
  }

  // Output tape for merge result
  std::unique_ptr<Tape> merged_tape = openTape(merge_tape_id);
  std::vector<int> merged_block;

  BlockBuffer block_buffer(std::move(tapes_to_merge), block_size_);
  while (!block_buffer.isEmpty()) {
    merged_block.push_back(block_buffer.getTopValue());
    block_buffer.popTopValue();

    if (merged_block.size() == block_size_) {
      merged_tape->writeBlock(merged_block.data(), block_size_);
      merged_block.clear();
    }
  }

  if (!merged_block.empty()) {
    merged_tape->writeBlock(merged_block.data(), merged_block.size());
  }

  return true;
}

size_t TapeSort::generateSortedTemporaryTapes() {
  size_t number_of_tapes_generated = 0;

  std::vector<int> buffer(max_buffer_size_);
  size_t values_read_count = 0;
  while ((values_read_count = input_tape_->readBlock(buffer.data(), max_buffer_size_))) {
    std::sort(buffer.begin(), buffer.begin() + static_cast<long>(values_read_count));
    openTape(number_of_tapes_generated++)->writeBlock(buffer.data(), values_read_count);
  }

  return number_of_tapes_generated;
}

std::unique_ptr<Tape> TapeSort::openTape(size_t tape_id) const {
  std::string tmp_tape_name = tmp_data_dir_ + "/" + std::to_string(tape_id);
  if (!std::filesystem::exists(tmp_tape_name)) {
    std::ofstream{tmp_tape_name};
  }

  return std::unique_ptr<Tape>(tape_generator_->createTape(tmp_tape_name.c_str()));
}
}  // namespace tapetools