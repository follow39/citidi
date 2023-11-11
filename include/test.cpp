#include <iostream>

#include "gtest/gtest.h"
#include "include/lib.h"

TEST(Dispatcher, SimpleTest)
{
  using MG = MarkerGroup<char, int>;
  using MG2 = MarkerGroup<double>;

  using DD =
      Dispatcher<Element<int, MG>, Element<int, int>, Element<double, double>>;

  DD disp {};

  auto& a = disp.Get<int>();
  a.data = 123;
  auto& b = disp.Get<double>();
  b.data = 12.34;
  // const auto& c = disp.Get<>();
  auto& d = disp.Get<MG>();
  d.data = 1;
  const auto& e = disp.Get<char, int>();
  d.data = 3;
  const auto& f = disp.Get<int, char>();
  d.data = 5;
  const auto& g = disp.Get<MG2>();

  EXPECT_EQ(123, a.data);
  EXPECT_EQ(12.34, b.data);
  EXPECT_EQ(5, d.data);
  EXPECT_EQ(5, e.data);
  EXPECT_EQ(5, f.data);
  EXPECT_EQ(12.34, g.data);
}

TEST(MergeTuples, SimpleTest)
{
  using A = typename MergeTuples<std::tuple<int, float>,
                                 std::tuple<std::size_t, double>,
                                 std::tuple<std::vector<int>>>::R;
  using E = std::tuple<int, float, std::size_t, double, std::vector<int>>;

  static_assert(std::is_same<E, A>::value, "");
}

TEST(MatchTuples, SimpleTest)
{
  using T1 = std::tuple<int, char>;
  using T2 = std::tuple<int, double, std::vector<int>, char>;
  using R = typename match::MatchUnorderedTuplesExactly<T1, T2>;

  const auto number = R::number;
  const auto match = R::match;

  EXPECT_EQ(number, 2);
  EXPECT_FALSE(match);
}

TEST(MatchTuples, SimpleTest2)
{
  using T1 = std::tuple<double, std::vector<int>, int, char>;
  using T2 = std::tuple<int, double, std::vector<int>, char>;
  using R = typename match::MatchUnorderedTuplesExactly<T1, T2>;

  const auto number = R::number;
  const auto match = R::match;

  EXPECT_EQ(number, 4);
  EXPECT_TRUE(match);
}
