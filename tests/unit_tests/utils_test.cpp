#include "include/utils.hpp"

#include "gtest/gtest.h"

namespace
{

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

TEST(MatchTuples, DISABLED_ShouldNotCompile)
{
  using T1 =
      std::tuple<double, std::vector<int>, int, int>;  // int considered 2 times
  using T2 = std::tuple<int, double, std::vector<int>, char>;
  using R = typename match::MatchUnorderedTuplesExactly<T1, T2>;

  const auto number = R::number;
  const auto match = R::match;

  EXPECT_EQ(number, 3);
  EXPECT_FALSE(match);
}

TEST(FindFirstTupleInsideSecond, SimpleTest)
{
  using T1 = std::tuple<int, char>;
  using T2 = std::tuple<int, double, std::vector<int>, char>;
  using R = typename match::FindFirstTupleInsideSecond<T1, T2>;

  const auto number = R::number;
  const auto match = R::value;

  EXPECT_EQ(number, 2);
  EXPECT_TRUE(match);
}

TEST(FindFirstTupleInsideSecond, DISABLED_ShouldNotCompile)
{
  using T1 = std::tuple<int, int>;  // int considered 2 times
  using T2 = std::tuple<int, double, std::vector<int>, char>;
  using R = typename match::FindFirstTupleInsideSecond<T1, T2>;

  const auto number = R::number;
  const auto match = R::value;

  EXPECT_EQ(number, 1);
  EXPECT_FALSE(match);
}

}  // namespace
