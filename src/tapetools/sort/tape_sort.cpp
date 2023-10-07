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

  // For each of merge candidates load its block
  std::vector<std::pair<std::vector<int>, size_t>> tape_block_buffer(tapes_to_merge.size(),
                                                                     {std::vector<int>(block_size_), 0});
  for (size_t i = 0; i < tape_block_buffer.size(); ++i) {
    tape_block_buffer[i].second = tapes_to_merge[i]->readBlock(tape_block_buffer[i].first.data(), block_size_);
  }

  // Priority queue for block merging
  struct BufferKey {
    size_t buffer_id;
    size_t buffer_block_pos;
  };

  auto comp = [&tape_block_buffer](BufferKey lhs, BufferKey rhs) {
    return tape_block_buffer[lhs.buffer_id].first[lhs.buffer_block_pos] >
           tape_block_buffer[rhs.buffer_id].first[rhs.buffer_block_pos];
  };
  std::priority_queue<BufferKey, std::vector<BufferKey>, decltype(comp)> pq(comp);
  for (size_t i = 0; i < tape_block_buffer.size(); ++i) {
    if (tape_block_buffer[i].second != 0) {
      pq.emplace(BufferKey{i, 0});
    }
  }

  // Create output tape
  std::unique_ptr<Tape> output_tape = openTape(merge_tape_id);
  // Block of output tape stored in RAM
  std::vector<int> out_block;

  // Merge tapes
  while (!pq.empty()) {
    BufferKey min_buffer_key = pq.top();
    pq.pop();

    // Push min value into output block
    out_block.push_back(tape_block_buffer[min_buffer_key.buffer_id].first[min_buffer_key.buffer_block_pos]);
    if (out_block.size() == block_size_) {
      output_tape->writeBlock(out_block.data(), block_size_);
      out_block.clear();
    }

    bool block_is_exhausted =
        (min_buffer_key.buffer_block_pos + 1 == tape_block_buffer[min_buffer_key.buffer_id].second);
    // Update block with next one from corresponding tape if needed
    if (block_is_exhausted) {
      tape_block_buffer[min_buffer_key.buffer_id].second = tapes_to_merge[min_buffer_key.buffer_id]->readBlock(
          tape_block_buffer[min_buffer_key.buffer_id].first.data(), block_size_);
      if (tape_block_buffer[min_buffer_key.buffer_id].second != 0) {
        pq.emplace(BufferKey{min_buffer_key.buffer_id, 0});
      }
    } else {
      pq.emplace(BufferKey{min_buffer_key.buffer_id, min_buffer_key.buffer_block_pos + 1});
    }
  }

  if (!out_block.empty()) {
    output_tape->writeBlock(out_block.data(), out_block.size());
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