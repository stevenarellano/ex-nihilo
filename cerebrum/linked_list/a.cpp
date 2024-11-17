#include <vector>

int fill(int x) {
  // fill a vector from 0 to x with random numbers
  std::vector<int> vec(x);
  std::generate(vec.begin(), vec.end(), []() { return rand() % 100; });
}