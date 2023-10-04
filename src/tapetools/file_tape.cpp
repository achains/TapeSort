#include "tapetools/file_tape.h"

namespace tapetools {
FileTape::FileTape() {}

void FileTape::open(const char* tape_path) {
  tape_stream_.open(tape_path, std::fstream::in | std::fstream::out);
  if (!tape_stream_) {
    throw std::runtime_error("Can't open file");
  }
}

size_t FileTape::readBlock(int* block_ptr, size_t block_size) {
  int value = 0;
  int values_count = 0;
  while (values_count < block_size && (tape_stream_ >> value)) {
    block_ptr[values_count++] = value;
  }
  return values_count;
}
}  // namespace tapetools