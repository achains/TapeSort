#include "tapetools/file_tape.h"
#include <filesystem>

namespace tapetools {

FileTape::FileTape(const char* tape_path)
    : tape_stream_(tape_path, std::fstream::in | std::fstream::out | std::fstream::binary) {}

bool FileTape::read(int& value) {
  tape_stream_.read(reinterpret_cast<char*>(&value), sizeof(value));
  if (tape_stream_.eof()) {
    return false;
  }
  tape_stream_.seekg(get_p_);
  return true;
}

void FileTape::write(int value) {
  tape_stream_.write(reinterpret_cast<char*>(&value), sizeof(value));
  tape_stream_.seekp(put_p_);
}

void FileTape::moveForward() {
  if (get_p_ == std::fstream::end || put_p_ == std::fstream::end) {
    throw std::runtime_error("Error. Move forward out of bounds.");
  }
  updateStreamPos(get_p_ + std::streamoff(sizeof(int)), put_p_ + std::streamoff(sizeof(int)));
}

void FileTape::moveBackward() {
  if (get_p_ == std::fstream::beg || put_p_ == std::fstream::beg) {
    throw std::runtime_error("Error. Move backward out of bounds.");
  }
  updateStreamPos(get_p_ - std::streamoff(sizeof(int)), put_p_ - std::streamoff(sizeof(int)));
}

void FileTape::moveBegin() { updateStreamPos(std::fstream::beg, std::fstream::beg); }

void FileTape::moveEnd() { updateStreamPos(std::fstream::end, std::fstream::end); }

void FileTape::updateStreamPos(std::streampos get_p, std::streampos put_p) {
  get_p_ = get_p;
  put_p_ = put_p;
  tape_stream_.seekg(get_p_);
  tape_stream_.seekp(put_p);
}

size_t FileTape::readBlock(int* block_ptr, size_t block_size) {
  size_t values_read_count = 0;
  bool read_code;
  int value;
  while ((read_code = read(value)) && values_read_count < block_size) {
    block_ptr[values_read_count++] = value;
    moveForward();
  }
  return values_read_count;
}

void FileTape::writeBlock(const int* block_ptr, size_t block_size) {
  for (size_t i = 0; i < block_size; ++i) {
    write(block_ptr[i]);
    moveForward();
  }
}
}  // namespace tapetools