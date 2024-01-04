#include <iostream>

#include "gtest/gtest.h"
#include "include/condition.hpp"
#include "include/lib.hpp"

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

TEST(ShrinkTo, SimpleTest)
{
  using MG = MarkerGroup<char, int>;

  using DD =
      Dispatcher<Element<int, MG>, Element<int, int>, Element<double, double>>;

  DD disp {};

  disp.Get<MG>().data = 7;
  disp.Get<int>().data = 13;

  auto t = disp.ShrinkTo<int>();

  EXPECT_EQ(t.Get<MG>().data, 7);
  EXPECT_EQ(t.Get<int>().data, 13);

  disp.Get<MG>().data = 11;
  disp.Get<int>().data = 14;

  EXPECT_EQ(t.Get<MG>().data, 11);
  EXPECT_EQ(t.Get<int>().data, 14);

  auto t2 = t.ShrinkTo<char>();

  EXPECT_EQ(t2.Get<MG>().data, 11);
}

namespace
{

using MG = MarkerGroup<char, int>;

using DD =
    Dispatcher<Element<int, MG>, Element<int, int>, Element<double, double>>;

template<typename T>
void TestCoercionInt(Slice<typename T::DType, int> s)
{
  EXPECT_EQ(s.template Get<MG>().data, 7);
  EXPECT_EQ(s.template Get<int>().data, 13);
}

template<typename T>
void TestCoercionChar(Slice<typename T::DType, char> s)
{
  EXPECT_EQ(s.template Get<MG>().data, 7);
}

TEST(Slice, ImplicitCoercionSliceTest)
{
  DD disp {};

  disp.Get<MG>().data = 7;
  disp.Get<int>().data = 13;

  TestCoercionInt<DD>(disp);

  auto t = disp.ShrinkTo<int>();
  TestCoercionChar<decltype(t)>(t);

  auto s = t.ShrinkTo<char>();
  TestCoercionChar<decltype(s)>(s);
}

}  // namespace

namespace
{

template<bool Result>
struct TestOperation
{
  template<typename T>
  constexpr static bool Evaluate()
  {
    return Result;
  }
};

TEST(Slice, OrConditionTest)
{
  {
    using Condition = Or<TestOperation<true>, TestOperation<true>>;
    EXPECT_TRUE(Condition::Evaluate<int>());
  }
  {
    using Condition = Or<TestOperation<true>, TestOperation<false>>;
    EXPECT_TRUE(Condition::Evaluate<int>());
  }
  {
    using Condition = Or<TestOperation<false>, TestOperation<true>>;
    EXPECT_TRUE(Condition::Evaluate<int>());
  }
  {
    using Condition = Or<TestOperation<false>, TestOperation<false>>;
    EXPECT_FALSE(Condition::Evaluate<int>());
  }
  {
    using Condition = Or<TestOperation<false>,
                         TestOperation<false>,
                         TestOperation<true>,
                         TestOperation<false>>;
    EXPECT_TRUE(Condition::Evaluate<int>());
  }
}

TEST(Slice, AndConditionTest)
{
  {
    using Condition = And<TestOperation<true>, TestOperation<true>>;
    EXPECT_TRUE(Condition::Evaluate<int>());
  }
  {
    using Condition = And<TestOperation<true>, TestOperation<false>>;
    EXPECT_FALSE(Condition::Evaluate<int>());
  }
  {
    using Condition = And<TestOperation<false>, TestOperation<true>>;
    EXPECT_FALSE(Condition::Evaluate<int>());
  }
  {
    using Condition = And<TestOperation<false>, TestOperation<false>>;
    EXPECT_FALSE(Condition::Evaluate<int>());
  }
  {
    using Condition = And<TestOperation<true>,
                          TestOperation<true>,
                          TestOperation<false>,
                          TestOperation<true>>;
    EXPECT_FALSE(Condition::Evaluate<int>());
  }
}

TEST(Slice, NotConditionTest)
{
  {
    using Condition = Not<TestOperation<true>>;
    EXPECT_FALSE(Condition::Evaluate<void>());
  }
  {
    using Condition = Not<TestOperation<false>>;
    EXPECT_TRUE(Condition::Evaluate<void>());
  }
}

TEST(Slice, WithConditionTest)
{
  using TestTuple = std::tuple<int, float>;
  {
    using Condition = With<int>;
    const bool r = Condition::Evaluate<TestTuple>();
    EXPECT_TRUE(r);
  }
  {
    using Condition = With<float>;
    const bool r = Condition::Evaluate<TestTuple>();
    EXPECT_TRUE(r);
  }
  {
    using Condition = With<float, int>;
    const bool r = Condition::Evaluate<TestTuple>();
    EXPECT_TRUE(r);
  }
  {
    using Condition = With<int, float>;
    const bool r = Condition::Evaluate<TestTuple>();
    EXPECT_TRUE(r);
  }
  {
    using Condition = With<char>;
    const bool r = Condition::Evaluate<TestTuple>();
    EXPECT_FALSE(r);
  }
}

TEST(Slice, WithoutConditionTest)
{
  using TestTuple = std::tuple<int, float>;
  {
    using Condition = Without<int>;
    const bool r = Condition::Evaluate<TestTuple>();
    EXPECT_FALSE(r);
  }
  {
    using Condition = Without<float>;
    const bool r = Condition::Evaluate<TestTuple>();
    EXPECT_FALSE(r);
  }
  {
    using Condition = Without<float, int>;
    const bool r = Condition::Evaluate<TestTuple>();
    EXPECT_FALSE(r);
  }
  {
    using Condition = Without<int, float>;
    const bool r = Condition::Evaluate<TestTuple>();
    EXPECT_FALSE(r);
  }
  {
    using Condition = Without<char>;
    const bool r = Condition::Evaluate<TestTuple>();
    EXPECT_TRUE(r);
  }
}

TEST(Slice, ComplexConditionTest)
{
  using ConditionEvaluatorType = ConditionEvaluator<
      Or<With<int>, With<float>, And<With<bool>, Without<char, double>>>>;

  {
    const bool r = ConditionEvaluatorType::Evaluate<std::tuple<int, float>>();
    EXPECT_TRUE(r);
  }
  {
    const bool r = ConditionEvaluatorType::Evaluate<std::tuple<bool, float>>();
    EXPECT_TRUE(r);
  }
  {
    const bool r = ConditionEvaluatorType::Evaluate<std::tuple<bool, double>>();
    EXPECT_TRUE(r);
  }
  {
    const bool r =
        ConditionEvaluatorType::Evaluate<std::tuple<bool, double, char>>();
    EXPECT_FALSE(r);
  }
}

}  // namespace
