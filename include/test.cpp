#include <iostream>

#include "gtest/gtest.h"
#include "include/lib.h"

using DD = Dispatcher<Element<int, int>, Element<double, double>>;

TEST(Dispatcher, SimpleTest)
{
  DD d {};

  const auto& a = d.Get<int>();
  const auto& b = d.Get<double>();
  // const auto& c = d.Get<>();

  EXPECT_EQ(0, a.data);
  EXPECT_EQ(0.0, b.data);
}
