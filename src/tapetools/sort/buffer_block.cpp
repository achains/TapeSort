#include "tapetools/sort/buffer_block.h"

namespace tapetools {
BufferBlock::BufferBlock(std::vector<int> const& raw_data) : data_(raw_data) {}

bool BufferBlock::hasNext() const { return current_pos_ < data_.size(); }

int BufferBlock::getNext() { return data_[current_pos_++]; }
}  // namespace tapetools