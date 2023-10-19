#include <gtest/gtest.h>

#include <filesystem>
#include <random>

#include "tapetools/file_tape_creator.h"
#include "tapetools/sort/tape_sort.h"

using namespace tapetools;

class SortedTape : public ::testing::Test {
 protected:
  void SetUp() override {
    tape_creator_ = std::make_shared<FileTapeCreator>(tmp_dir_.c_str());
    tape_ = tape_creator_->createTape(input_tape_name_);
    output_tape_ = tape_creator_->createTape(output_tape_name_);
  }

  void fillWithRandom(size_t tape_size) {
    std::random_device rd;
    std::mt19937 engine(rd());
    std::uniform_int_distribution<int> dist(-100, 100);

    for (size_t i = 0; i < tape_size; ++i) {
      tape_->write(dist(engine));
      tape_->moveForward();
    }
    tape_->moveBegin();
  }

  bool isSorted(const char* tape_name) {
    std::vector<int> sorted_tape_values = getTapeFileContent(tape_name);
    return std::is_sorted(sorted_tape_values.begin(), sorted_tape_values.end());
  }

  void TearDown() override { std::filesystem::remove_all(tmp_dir_); }

  const char* getInputName() { return input_tape_name_; }

  const char* getOutputName() { return output_tape_name_; }

  std::vector<int> getTapeFileContent(const char* tape_name) {
    std::ifstream f(tmp_dir_ / tape_name, std::fstream::binary);
    std::vector<int> file_content;

    int value;
    while (!f.read(reinterpret_cast<char*>(&value), sizeof(value)).eof()) {
      file_content.push_back(value);
    }
    return file_content;
  }

  std::shared_ptr<Tape> tape_;
  std::shared_ptr<Tape> output_tape_;
  std::shared_ptr<FileTapeCreator> tape_creator_;

 private:
  std::filesystem::path tmp_dir_ = "tests_tmp";
  const char* input_tape_name_ = "rand_tape";
  const char* output_tape_name_ = "sorted_tape";
};

TEST_F(SortedTape, Sort100) {
  fillWithRandom(100);
  ASSERT_EQ(isSorted(getInputName()), false);

  size_t block_size = 10;
  size_t buffer_size = 40;

  TapeSort(tape_, output_tape_, tape_creator_, block_size, buffer_size).sort();
  ASSERT_EQ(isSorted(getOutputName()), true);
  ASSERT_EQ(getTapeFileContent(getInputName()).size(), getTapeFileContent(getOutputName()).size());
}

TEST_F(SortedTape, Sort10000) {
  fillWithRandom(10000);
  size_t block_size = 100;
  size_t buffer_size = 500;

  TapeSort(tape_, output_tape_, tape_creator_, block_size, buffer_size).sort();
  ASSERT_EQ(isSorted(getOutputName()), true);
  ASSERT_EQ(getTapeFileContent(getInputName()).size(), getTapeFileContent(getOutputName()).size());
}

TEST_F(SortedTape, BigBufferSize) {
  fillWithRandom(100);
  size_t block_size = 5;
  size_t buffer_size = 1000;

  TapeSort(tape_, output_tape_, tape_creator_, block_size, buffer_size).sort();
  ASSERT_EQ(isSorted(getOutputName()), true);
  ASSERT_EQ(getTapeFileContent(getInputName()).size(), getTapeFileContent(getOutputName()).size());
}

TEST_F(SortedTape, PrimeSizes) {
  fillWithRandom(83);
  size_t block_size = 7;
  size_t buffer_size = 23;

  TapeSort(tape_, output_tape_, tape_creator_, block_size, buffer_size).sort();
  ASSERT_EQ(isSorted(getOutputName()), true);
  ASSERT_EQ(getTapeFileContent(getInputName()).size(), getTapeFileContent(getOutputName()).size());
}

TEST_F(SortedTape, SmallBlockSize) {
  fillWithRandom(100);
  size_t block_size = 1;
  size_t buffer_size = 2;

  TapeSort(tape_, output_tape_, tape_creator_, block_size, buffer_size).sort();
  ASSERT_EQ(isSorted(getOutputName()), true);
  ASSERT_EQ(getTapeFileContent(getInputName()).size(), getTapeFileContent(getOutputName()).size());
}

TEST_F(SortedTape, NotEnoughBlocks) {
  fillWithRandom(100);
  size_t block_size = 7;
  size_t buffer_size = 10;

  ASSERT_THROW(TapeSort(tape_, output_tape_, tape_creator_, block_size, buffer_size).sort(),
               std::runtime_error);
}