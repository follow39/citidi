#include "include/condition.hpp"

#include "gtest/gtest.h"

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
