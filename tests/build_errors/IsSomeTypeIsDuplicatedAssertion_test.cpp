#include <tuple>
#include <iostream>

#include "include/utils.hpp"

int main()
{
  std::ignore = IsSomeTypeIsDuplicatedAssertion<int, int>{};

  return 0;
}
