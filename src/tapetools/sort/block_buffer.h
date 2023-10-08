#pragma once

#include <memory>
#include <queue>
#include <vector>

#include "tape_block.h"
#include "tapetools/tape.h"

namespace tapetools {
class BlockBuffer {
 public:
  BlockBuffer(std::vector<std::unique_ptr<Tape>> block_tapes, size_t max_block_size);

  bool isEmpty() const;

  int getTopValue() const;

  void popTopValue();

 private:
  using BlockPtrBlockPos = std::pair<TapeBlock*, size_t>;

  struct BlockPtrComp {
    bool operator()(BlockPtrBlockPos const& lhs, BlockPtrBlockPos const& rhs);
  };

  std::vector<TapeBlock> block_buffer_;
  size_t max_block_size_;
  std::priority_queue<BlockPtrBlockPos, std::vector<BlockPtrBlockPos>, BlockPtrComp> pq_;
};
}  // namespace tapetools