#include "include/condition.hpp"

#include "gtest/gtest.h"

namespace {

TEST(BoolConst, EvaluateTest) {
  {
    using Condition = BoolConst<true>;
    EXPECT_TRUE(Condition::Evaluate<void>());
  }
  {
    using Condition = BoolConst<false>;
    EXPECT_FALSE(Condition::Evaluate<void>());
  }
}

TEST(Or, EvaluateTest) {
  {
    using Condition = Or<BoolConst<true>, BoolConst<true>>;
    EXPECT_TRUE(Condition::Evaluate<int>());
  }
  {
    using Condition = Or<BoolConst<true>, BoolConst<false>>;
    EXPECT_TRUE(Condition::Evaluate<int>());
  }
  {
    using Condition = Or<BoolConst<false>, BoolConst<true>>;
    EXPECT_TRUE(Condition::Evaluate<int>());
  }
  {
    using Condition = Or<BoolConst<false>, BoolConst<false>>;
    EXPECT_FALSE(Condition::Evaluate<int>());
  }
  {
    using Condition = Or<BoolConst<false>, BoolConst<false>, BoolConst<true>,
                         BoolConst<false>>;
    EXPECT_TRUE(Condition::Evaluate<int>());
  }
}

TEST(And, EvaluateTest) {
  {
    using Condition = And<BoolConst<true>, BoolConst<true>>;
    EXPECT_TRUE(Condition::Evaluate<int>());
  }
  {
    using Condition = And<BoolConst<true>, BoolConst<false>>;
    EXPECT_FALSE(Condition::Evaluate<int>());
  }
  {
    using Condition = And<BoolConst<false>, BoolConst<true>>;
    EXPECT_FALSE(Condition::Evaluate<int>());
  }
  {
    using Condition = And<BoolConst<false>, BoolConst<false>>;
    EXPECT_FALSE(Condition::Evaluate<int>());
  }
  {
    using Condition = And<BoolConst<true>, BoolConst<true>, BoolConst<false>,
                          BoolConst<true>>;
    EXPECT_FALSE(Condition::Evaluate<int>());
  }
}

TEST(Not, EvaluateTest) {
  {
    using Condition = Not<BoolConst<true>>;
    EXPECT_FALSE(Condition::Evaluate<void>());
  }
  {
    using Condition = Not<BoolConst<false>>;
    EXPECT_TRUE(Condition::Evaluate<void>());
  }
}

TEST(With, EvaluateTest) {
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

TEST(WithExactly, EvaluateTest) {
  using TestTuple = std::tuple<int, float>;
  {
    using Condition = WithExactly<int>;
    const bool r = Condition::Evaluate<TestTuple>();
    EXPECT_FALSE(r);
  }
  {
    using Condition = WithExactly<float>;
    const bool r = Condition::Evaluate<TestTuple>();
    EXPECT_FALSE(r);
  }
  {
    using Condition = WithExactly<float, int>;
    const bool r = Condition::Evaluate<TestTuple>();
    EXPECT_TRUE(r);
  }
  {
    using Condition = WithExactly<int, float>;
    const bool r = Condition::Evaluate<TestTuple>();
    EXPECT_TRUE(r);
  }
  {
    using Condition = WithExactly<char>;
    const bool r = Condition::Evaluate<TestTuple>();
    EXPECT_FALSE(r);
  }
}

TEST(Without, EvaluateTest) {
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

TEST(ComplexCondition, Test) {
  using ConditionType =
      Or<With<int>, With<float>, And<With<bool>, Without<char, double>>>;

  {
    const bool r =
        ConditionEvaluator<std::tuple<int, float>, ConditionType>::value;
    EXPECT_TRUE(r);
  }
  {
    const bool r =
        ConditionEvaluator<std::tuple<bool, float>, ConditionType>::value;
    EXPECT_TRUE(r);
  }
  {
    const bool r =
        ConditionEvaluator<std::tuple<bool, double>, ConditionType>::value;
    EXPECT_TRUE(r);
  }
  {
    const bool r = ConditionEvaluator<std::tuple<bool, double, char>,
                                      ConditionType>::value;
    EXPECT_FALSE(r);
  }
}

}  // namespace
