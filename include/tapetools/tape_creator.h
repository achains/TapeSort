#pragma once

#include "tape.h"

namespace tapetools {
class TapeCreator {
 public:
  virtual Tape* createTape(const char* tape_source) = 0;
};
}  // namespace tapetools