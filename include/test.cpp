#include <iostream>

#include "gtest/gtest.h"
#include "include/lib.h"
#include "include/tuple_sort.h"

TEST(Dispatcher, SimpleTest)
{
  using MG = MarkerGroup<char, int>;

  using DD =
      Dispatcher<Element<int, int>, Element<double, double>, Element<int, MG>>;

  DD disp {};

  const auto& a = disp.Get<int>();
  const auto& b = disp.Get<double>();
  // const auto& c = d.Get<>();
  auto& d = disp.Get<MG>();
  d.data = 1;
  const auto& e = disp.Get<char, int>();
  d.data = 3;
  const auto& f = disp.Get<int, char>();
  d.data = 5;

  EXPECT_EQ(0, a.data);
  EXPECT_EQ(0.0, b.data);
  EXPECT_EQ(5, d.data);
  EXPECT_EQ(5, e.data);
  EXPECT_EQ(5, f.data);
}

TEST(MergeTuples, SimpleTest)
{
  using A = typename MergeTuples<std::tuple<int, float>,
                                 std::tuple<std::size_t, double>,
                                 std::tuple<std::vector<int>>>::R;
  using E = std::tuple<int, float, std::size_t, double, std::vector<int>>;

  static_assert(std::is_same<E, A>::value, "");
}

TEST(SortTupleTypes, SimpleTest)
{
  using T = std::tuple<int, double, std::vector<int>, char>;
  using A = typename SortTuple<T, DefaultComparator>::R;
  using E = std::tuple<char, int, double, std::vector<int>>;

  static_assert(std::is_same<E, A>::value, "");
}
