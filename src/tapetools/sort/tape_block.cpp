#include "tape_block.h"

namespace tapetools {
TapeBlock::TapeBlock(std::unique_ptr<Tape> tape, size_t max_block_size)
    : max_block_size_(max_block_size), data_(max_block_size_), tape_(std::move(tape)) {
  updateBlock();
}

bool TapeBlock::updateBlock() {
  current_size_ = tape_->readBlock(data_.data(), max_block_size_);
  return current_size_ > 0;
}

size_t TapeBlock::getSize() const { return current_size_; }

int TapeBlock::getValue(size_t i) const {
  if (i < getSize()) {
    return data_[i];
  }
  throw std::runtime_error("Error. Out of block bounds.");
}

}  // namespace tapetools