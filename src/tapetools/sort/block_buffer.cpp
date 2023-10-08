#include "block_buffer.h"

namespace tapetools {
BlockBuffer::BlockBuffer(std::vector<std::unique_ptr<Tape>> block_tapes, size_t max_block_size)
    : max_block_size_(max_block_size) {
  for (auto& tape_ptr : block_tapes) {
    block_buffer_.emplace_back(std::move(tape_ptr), max_block_size_);
  }

  for (auto& block : block_buffer_) {
    pq_.emplace(&block, 0);
  }
}

bool BlockBuffer::BlockPtrComp::operator()(BlockPtrBlockPos const& lhs, BlockPtrBlockPos const& rhs) {
  return lhs.first->getValue(lhs.second) > rhs.first->getValue(rhs.second);
}

bool BlockBuffer::isEmpty() const { return pq_.empty(); }

int BlockBuffer::getTopValue() const {
  if (!isEmpty()) {
    return pq_.top().first->getValue(pq_.top().second);
  }
  throw std::runtime_error("Error. Attempted to get top value of empty queue.");
}

void BlockBuffer::popTopValue() {
  if (isEmpty()) {
    throw std::runtime_error("Error. Attempted to pop value of empty queue.");
  }

  BlockPtrBlockPos top_block = pq_.top();
  pq_.pop();
  if (top_block.second + 1 < top_block.first->getSize()) {
    ++top_block.second;
    pq_.emplace(top_block);
    return;
  }
  bool block_is_valid = top_block.first->updateBlock();
  if (block_is_valid) {
    top_block.second = 0;
    pq_.emplace(top_block);
  }
}

}  // namespace tapetools