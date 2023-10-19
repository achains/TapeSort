#include "tapetools/file_tape_creator.h"

#include <filesystem>

namespace tapetools {
FileTapeCreator::FileTapeCreator(const char* tape_dir) : tape_dir_(tape_dir) {
  if (tape_dir_) {
    if (std::filesystem::exists(tape_dir_)) {
      std::filesystem::remove_all(tape_dir_);
    }
    std::filesystem::create_directory(tape_dir_);
  }
}

FileTape* FileTapeCreator::createTape(const char* tape_source) {
  return new FileTape((std::filesystem::path(tape_dir_) / std::filesystem::path(tape_source)).c_str());
}
}  // namespace tapetools