#include <iostream>
#include <tuple>

#include "include/utils.hpp"

int main() {
  std::ignore = IsSomeTypeIsDuplicatedAssertion<int, int>{};

  return 0;
}
