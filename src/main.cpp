#include <iostream>
#include <vector>

#include "tapetools/file_tape.h"

int main(){
  const char* test_data = "/home/ach/CLionProjects/TapeSort/tests/data/random_50.txt";

  tapetools::FileTape tape;
  tape.open(test_data);

  std::vector<int> a(100);
  size_t values_count = tape.readBlock(a.data(), 100);
  for (int i = 0; i < 100; ++i) {
    std::cout << a[i] << ' ';
  }
  std::cout << '\n' << values_count << '\n';

  return 0;
}