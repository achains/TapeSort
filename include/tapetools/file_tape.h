#pragma once

#include "tape.h"

#include <fstream>

namespace tapetools {
class FileTape : public Tape {
 public:
  FileTape();

  void readBlock(int* block_ptr, size_t block_size) override;

  void open(const char* tape_path);

 private:
  std::fstream tape_stream_;
};
}  // namespace tapetools