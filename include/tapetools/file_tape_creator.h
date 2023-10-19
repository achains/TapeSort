#pragma once

#include "file_tape.h"
#include "tape_creator.h"

namespace tapetools {
class FileTapeCreator : public TapeCreator {
 public:
  FileTapeCreator(const char* tape_dir = "");

  FileTape* createTape(const char* tape_source) override;

 private:
  const char* tape_dir_ = "";
};
}  // namespace tapetools