#pragma once

#include "tape.h"

#include <fstream>

namespace tapetools {
class FileTape : public Tape {
 public:
  FileTape(const char* tape_path);

  ~FileTape() = default;

  virtual bool read(int& value) override;

  virtual void write(int value) override;

  virtual void moveForward() override;

  virtual void moveBackward() override;

  virtual void moveBegin() override;

  virtual void moveEnd() override;

  size_t readBlock(int* block_ptr, size_t block_size) override;

  void writeBlock(const int* block_ptr, size_t block_size) override;

  void setIODelay(const char* config_path);

 private:
  void updateStreamPos(std::streampos get_p, std::streampos put_p);

  struct IODelay {
    size_t read_ms = 0;
    size_t write_ms = 0;
    size_t move_ms = 0;
    size_t rewind_ms = 0;
  } io_delay_;

  std::fstream tape_stream_;
  std::streampos get_p_ = std::fstream::beg;
  std::streampos put_p_ = std::fstream::beg;
};
}  // namespace tapetools