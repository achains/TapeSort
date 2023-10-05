#pragma once

#include "tape.h"

#include <fstream>

namespace tapetools {
class FileTape : public Tape {
 public:
  FileTape(const char* tape_path);

  ~FileTape() = default;

  virtual int read() override;

  virtual void write(int value) override;

  virtual void moveForward() override;

  virtual void moveBackward() override;

  virtual void moveBegin() override;

  virtual void moveEnd() override;

  size_t readBlock(int* block_ptr, size_t block_size) override;

  void writeBlock(const int* block_ptr, size_t block_size) override;

 private:
  void updateStreamPos(std::streampos get_p, std::streampos put_p);

  std::fstream tape_stream_;
  std::streampos get_p_ = std::fstream::beg;
  std::streampos put_p_ = std::fstream::beg;
};
}  // namespace tapetools