#include <iostream>
#include <memory>

#include "tapetools/file_tape_creator.h"
#include "tapetools/sort/tape_sort.h"

int main(int argc, char** argv) {
  if (argc < 5) {
    std::cerr << "Error. Not enough arguments. Expected format: input_tape input_config output_tape output_config "
                 "[buffer_size] [block_size]"
              << std::endl;
    return 1;
  }

  const char* input_tape_path = argv[1];
  const char* input_tape_config = argv[2];
  const char* output_tape_path = argv[3];
  const char* output_tape_config = argv[4];

  auto tape_creator = std::make_shared<tapetools::FileTapeCreator>();

  std::shared_ptr<tapetools::Tape> input_tape(tape_creator->createTape(input_tape_path));
  dynamic_cast<tapetools::FileTape*>(input_tape.get())->setIODelay(input_tape_config);

  std::shared_ptr<tapetools::Tape> output_tape(tape_creator->createTape(output_tape_path));
  dynamic_cast<tapetools::FileTape*>(output_tape.get())->setIODelay(output_tape_config);

  size_t buffer_size = 100;
  size_t block_size = 20;

  if (argc > 6) {
    buffer_size = std::stoul(argv[5]);
    block_size = std::stoul(argv[6]);
  }

  try {
    tapetools::TapeSort(input_tape, output_tape, tape_creator, block_size, buffer_size).sort();
  } catch (std::runtime_error const& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}