#pragma once

#include "tape.h"

#include <memory>

namespace tapetools {
class TapeCreator {
 public:
  virtual std::unique_ptr<Tape> createTape(const char* tape_source) = 0;
};
}  // namespace tapetools