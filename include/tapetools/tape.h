#pragma once

#include <cstddef>
#include <iostream>

namespace tapetools {
class Tape {
 public:
  virtual ~Tape() {}

  virtual bool read(int& value) = 0;

  virtual void write(int value) = 0;

  virtual void moveForward() = 0;

  virtual void moveBackward() = 0;

  virtual void moveBegin() = 0;

  virtual void moveEnd() = 0;

  virtual size_t readBlock(int* block_ptr, size_t block_size) = 0;

  virtual void writeBlock(const int* block_ptr, size_t block_size) = 0;
};

}  // namespace tapetools