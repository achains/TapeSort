#include <gtest/gtest.h>

#include <filesystem>
#include <numeric>

#include "tapetools/file_tape_creator.h"

using namespace tapetools;

class TemporaryTape : public ::testing::Test {
 protected:
  void SetUp() override {
    tape_.reset(FileTapeCreator(tmp_dir_.c_str()).createTape(tape_name_));
  }

  void TearDown() override { std::filesystem::remove_all(tmp_dir_); }

  std::vector<int> getTapeFileContent() {
    std::ifstream f(tmp_dir_ / tape_name_, std::fstream::binary);
    std::vector<int> file_content;

    int value;
    while (!f.read(reinterpret_cast<char*>(&value), sizeof(value)).eof()) {
      file_content.push_back(value);
    }
    return file_content;
  }

  std::unique_ptr<tapetools::FileTape> tape_;

 private:
  std::filesystem::path tmp_dir_ = "tests_tmp";
  const char* tape_name_ = "0";
};

TEST_F(TemporaryTape, SingleWrite) {
  int value_to_write = 42;
  tape_->write(value_to_write);
  ASSERT_EQ(getTapeFileContent(), std::vector<int>{value_to_write});
}

TEST_F(TemporaryTape, LastWriteValue) {
  int value_to_write;
  for (value_to_write = 0; value_to_write < 3; ++value_to_write) {
    tape_->write(value_to_write);
  }
  ASSERT_EQ(getTapeFileContent().size(), 1);
  ASSERT_EQ(getTapeFileContent(), std::vector<int>{value_to_write - 1});
}

TEST_F(TemporaryTape, WriteRead) {
  int value_to_write = 42;
  tape_->write(value_to_write);
  int value;
  tape_->read(value);
  ASSERT_EQ(value, value_to_write);
}

TEST_F(TemporaryTape, WriteMultiple) {
  std::vector<int> values_to_write(10);
  std::iota(values_to_write.begin(), values_to_write.end(), 0);

  for (int value : values_to_write) {
    tape_->write(value);
    tape_->moveForward();
  }
  ASSERT_EQ(getTapeFileContent(), values_to_write);
}

TEST_F(TemporaryTape, MoveForwardException) { ASSERT_THROW(tape_->moveForward(), std::runtime_error); }

TEST_F(TemporaryTape, MoveBackwardException) { ASSERT_THROW(tape_->moveBackward(), std::runtime_error); }

TEST_F(TemporaryTape, WriteMoveBackwardRead) {
  std::vector<int> values_to_write(10);
  std::iota(values_to_write.begin(), values_to_write.end(), 0);

  for (int value : values_to_write) {
    tape_->write(value);
    tape_->moveForward();
  }

  size_t back_shift = 5;
  for (size_t i = 0; i < back_shift; ++i) {
    tape_->moveBackward();
  }
  int value;
  tape_->read(value);
  ASSERT_EQ(value, 5);
}

TEST_F(TemporaryTape, WriteBlockMovesToEnd) {
  std::vector<int> values_to_write(10);
  std::iota(values_to_write.begin(), values_to_write.end(), 0);

  tape_->writeBlock(values_to_write.data(), values_to_write.size());

  ASSERT_THROW(tape_->moveForward(), std::runtime_error);
}

TEST_F(TemporaryTape, WriteBlock) {
  std::vector<int> values_to_write(10);
  std::iota(values_to_write.begin(), values_to_write.end(), 0);

  tape_->writeBlock(values_to_write.data(), values_to_write.size());

  ASSERT_EQ(getTapeFileContent(), values_to_write);
}

TEST_F(TemporaryTape, WriteMultipleBlocks) {
  std::vector<int> values_to_write(10);
  std::iota(values_to_write.begin(), values_to_write.end(), 0);

  for (size_t i = 0; i < 5; ++i) {
    tape_->writeBlock(values_to_write.data(), values_to_write.size());
  }

  std::vector<int> expected_vector;
  for (size_t i = 0; i < 5; ++i) {
    expected_vector.insert(expected_vector.end(), values_to_write.begin(), values_to_write.end());
  }
  ASSERT_EQ(getTapeFileContent(), expected_vector);
}

TEST_F(TemporaryTape, WriteBlockReadBlock) {
  std::vector<int> values_to_write(10);
  std::iota(values_to_write.begin(), values_to_write.end(), 0);

  tape_->writeBlock(values_to_write.data(), values_to_write.size());
  tape_->moveBegin();
  std::vector<int> actual_vector(values_to_write.size());
  size_t values_read_count = tape_->readBlock(actual_vector.data(), actual_vector.size());

  ASSERT_EQ(values_read_count, values_to_write.size());
  ASSERT_EQ(getTapeFileContent(), actual_vector);
}

TEST_F(TemporaryTape, ExcessiveRead) {
  std::vector<int> values_to_write(10);
  std::iota(values_to_write.begin(), values_to_write.end(), 0);

  tape_->writeBlock(values_to_write.data(), values_to_write.size());
  tape_->moveBegin();

  std::vector<int> actual_vector(values_to_write.size());
  size_t values_read_count = tape_->readBlock(actual_vector.data(), 1e5);

  ASSERT_EQ(values_read_count, values_to_write.size());
  ASSERT_EQ(getTapeFileContent(), actual_vector);
}

TEST_F(TemporaryTape, ReadBlockSingleValue) {
  int value_to_write = 42;
  tape_->write(value_to_write);
  std::vector<int> single_value_vector(1);
  tape_->readBlock(single_value_vector.data(), 1);
  ASSERT_EQ(getTapeFileContent(), single_value_vector);
}

TEST_F(TemporaryTape, ReadBlockEmptyFile) {
  std::vector<int> empty_vector;
  size_t values_read_count = tape_->readBlock(empty_vector.data(), 10);

  ASSERT_EQ(values_read_count, 0);
}