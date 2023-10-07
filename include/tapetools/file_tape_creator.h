#pragma once

#include "file_tape.h"
#include "tape_creator.h"

namespace tapetools {
class FileTapeCreator: public TapeCreator {
 public:
  FileTape* createTape(const char* tape_source) override;
};
}