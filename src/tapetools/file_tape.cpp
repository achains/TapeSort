#include "tapetools/file_tape.h"

namespace tapetools {
FileTape::FileTape() {}

void FileTape::open(const char* tape_path) {
  tape_stream_.open(tape_path, std::fstream::in | std::fstream::out);
  if (!tape_stream_) {
    throw std::runtime_error("Can't open file");
  }
}

void FileTape::readBlock(int* block_ptr, size_t block_size) {}
}  // namespace tapetools