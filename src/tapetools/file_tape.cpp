#include "tapetools/file_tape.h"

#include <filesystem>
#include <thread>

namespace tapetools {

FileTape::FileTape(const char* tape_path) {
  if (!std::filesystem::exists(tape_path)) {
    std::ofstream{tape_path};
  }
  tape_stream_ = std::fstream{tape_path, std::fstream::in | std::fstream::out | std::fstream::binary};
}

bool FileTape::read(int& value) {
  std::this_thread::sleep_for(std::chrono::milliseconds(io_delay_.read_ms));
  tape_stream_.read(reinterpret_cast<char*>(&value), sizeof(value));
  if (tape_stream_.eof()) {
    return false;
  }
  tape_stream_.seekg(get_p_);
  return true;
}

void FileTape::write(int value) {
  std::this_thread::sleep_for(std::chrono::milliseconds(io_delay_.write_ms));
  tape_stream_.write(reinterpret_cast<char*>(&value), sizeof(value));
  tape_stream_.seekp(put_p_);
}

void FileTape::moveForward() {
  std::this_thread::sleep_for(std::chrono::milliseconds(io_delay_.move_ms));
  int tmp_value;
  if (!read(tmp_value)) {
    throw std::runtime_error("Error. Move forward out of bounds.");
  }
  updateStreamPos(get_p_ + std::streamoff(sizeof(int)), put_p_ + std::streamoff(sizeof(int)));
}

void FileTape::moveBackward() {
  std::this_thread::sleep_for(std::chrono::milliseconds(io_delay_.move_ms));
  if (get_p_ == std::fstream::beg || put_p_ == std::fstream::beg) {
    throw std::runtime_error("Error. Move backward out of bounds.");
  }
  updateStreamPos(get_p_ - std::streamoff(sizeof(int)), put_p_ - std::streamoff(sizeof(int)));
}

void FileTape::moveBegin() {
  std::this_thread::sleep_for(std::chrono::milliseconds(io_delay_.rewind_ms));
  updateStreamPos(std::fstream::beg, std::fstream::beg);
}

void FileTape::moveEnd() {
  std::this_thread::sleep_for(std::chrono::milliseconds(io_delay_.rewind_ms));
  updateStreamPos(std::fstream::end, std::fstream::end);
}

void FileTape::updateStreamPos(std::streampos get_p, std::streampos put_p) {
  get_p_ = get_p;
  put_p_ = put_p;
  tape_stream_.seekg(get_p_);
  tape_stream_.seekp(put_p);
}

size_t FileTape::readBlock(int* block_ptr, size_t block_size) {
  size_t values_read_count = 0;
  bool read_code;
  int value;
  while ((read_code = read(value)) && values_read_count < block_size) {
    block_ptr[values_read_count++] = value;
    moveForward();
  }
  return values_read_count;
}

void FileTape::writeBlock(const int* block_ptr, size_t block_size) {
  for (size_t i = 0; i < block_size; ++i) {
    write(block_ptr[i]);
    moveForward();
  }
}

void FileTape::setIODelay(const char* config_path) {
  std::ifstream config_file(config_path);
  if (!config_file) {
    throw std::runtime_error("Error. Couldn't read config delay file.");
  }
  std::string line;
  while (std::getline(config_file, line)) {
    if (line[0] == '[' || line[0] == '#') {
      continue;
    }
    size_t delim_pos = line.find('=');
    if (delim_pos == std::string::npos) {
      std::cerr << "Warning. Invalid line format, missing '='." << std::endl;
      continue;
    }

    std::string param = line.substr(0, delim_pos);
    size_t delay_ms = std::stoul(line.substr(delim_pos + 1));

    if (param == "read") {
      io_delay_.read_ms = delay_ms;
    } else if (param == "write") {
      io_delay_.write_ms = delay_ms;
    } else if (param == "move") {
      io_delay_.move_ms = delay_ms;
    } else if (param == "rewind") {
      io_delay_.rewind_ms = delay_ms;
    } else {
      std::cerr << "Warning. Invalid parameter name '" + param + "'. Skipping line." << std::endl;
    }
  }
  config_file.close();
}
}  // namespace tapetools