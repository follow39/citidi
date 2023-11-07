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
  d.data = 5;
  const auto& e = disp.Get<char, int>();
  const auto& f = disp.Get<int, char>();

  EXPECT_EQ(0, a.data);
  EXPECT_EQ(0.0, b.data);
  EXPECT_EQ(5, d.data);
  EXPECT_EQ(5, e.data);
  EXPECT_EQ(5, f.data);
}

TEST(MergeTuples, SimpleTest)
{
  using T = typename MergeTuples<std::tuple<int, float>,
                                 std::tuple<std::size_t, double>,
                                 std::tuple<std::vector<int>>>::R;

  // TODO refactor with std::tuple_element_t
  EXPECT_EQ(typeid(int).name(), typeid(decltype(std::get<0>(T {}))).name());
  EXPECT_EQ(typeid(float).name(), typeid(decltype(std::get<1>(T {}))).name());
  EXPECT_EQ(typeid(std::size_t).name(),
            typeid(decltype(std::get<2>(T {}))).name());
  EXPECT_EQ(typeid(double).name(), typeid(decltype(std::get<3>(T {}))).name());
  EXPECT_EQ(typeid(std::vector<int>).name(),
            typeid(decltype(std::get<4>(T {}))).name());
}

TEST(SortTupleTypes, SimpleTest)
{
  using T = std::tuple<int, double, std::vector<int>, char>;
  using R = typename SortTuple<T, DefaultComparator>::R;

  EXPECT_EQ(typeid(char).name(), typeid(std::tuple_element_t<0, R>).name());
  EXPECT_EQ(typeid(int).name(), typeid(std::tuple_element_t<1, R>).name());
  EXPECT_EQ(typeid(double).name(), typeid(std::tuple_element_t<2, R>).name());
  EXPECT_EQ(typeid(std::vector<int>).name(),
            typeid(std::tuple_element_t<3, R>).name());
}
