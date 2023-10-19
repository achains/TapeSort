#pragma once

#include "file_tape.h"
#include "tape_creator.h"

namespace tapetools {
class FileTapeCreator : public TapeCreator {
 public:
  FileTapeCreator(const char* tape_dir = "");

  std::unique_ptr<Tape> createTape(const char* tape_source) override;

 private:
  const char* tape_dir_ = "";
};
}  // namespace tapetools