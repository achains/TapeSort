#include "tapetools/file_tape_creator.h"

namespace tapetools {
FileTape* FileTapeCreator::createTape(const char* tape_source) { return new FileTape(tape_source); }
}  // namespace tapetools