#include "include/utils.hpp"

#include "gtest/gtest.h"
#include "include/condition.hpp"

namespace
{

TEST(IsTypeInsidePack, SimpleTest)
{
  {
    const bool actual_value = IsTypeInsidePack<int, int>::value;
    EXPECT_TRUE(actual_value);
  }
  {
    const bool actual_value = IsTypeInsidePack<int, double, int, char>::value;
    EXPECT_TRUE(actual_value);
  }
  {
    const bool actual_value = IsTypeInsidePack<int, double, char>::value;
    EXPECT_FALSE(actual_value);
  }
}

TEST(IsTypeInsideTuple, SimpleTest)
{
  {
    const bool actual_value = IsTypeInsideTuple<int, std::tuple<int>>::value;
    EXPECT_TRUE(actual_value);
  }
  {
    const bool actual_value =
        IsTypeInsideTuple<int, std::tuple<double, int, char>>::value;
    EXPECT_TRUE(actual_value);
  }
  {
    const bool actual_value =
        IsTypeInsideTuple<int, std::tuple<double, char>>::value;
    EXPECT_FALSE(actual_value);
  }
}

TEST(CountTheSameTypesInUnorderedTuples, SimpleTest)
{
  {
    using Tuple1Type = std::tuple<double, int, char>;
    using Tuple2Type = std::tuple<double, int, char>;
    const std::size_t expected_value = 3;
    const std::size_t actual_value =
        CountTheSameTypesInUnorderedTuples<Tuple1Type, Tuple2Type>::value;
    EXPECT_EQ(expected_value, actual_value);
  }
  {
    using Tuple1Type = std::tuple<double, int, char>;
    using Tuple2Type = std::tuple<char, double, int>;
    const std::size_t expected_value = 3;
    const std::size_t actual_value =
        CountTheSameTypesInUnorderedTuples<Tuple1Type, Tuple2Type>::value;
    EXPECT_EQ(expected_value, actual_value);
  }
  {
    using Tuple1Type = std::tuple<double, int, char>;
    using Tuple2Type = std::tuple<char, int>;
    const std::size_t expected_value = 2;
    const std::size_t actual_value =
        CountTheSameTypesInUnorderedTuples<Tuple1Type, Tuple2Type>::value;
    EXPECT_EQ(expected_value, actual_value);
  }
  {
    using Tuple1Type = std::tuple<double, int>;
    using Tuple2Type = std::tuple<char, double, int>;
    const std::size_t expected_value = 2;
    const std::size_t actual_value =
        CountTheSameTypesInUnorderedTuples<Tuple1Type, Tuple2Type>::value;
    EXPECT_EQ(expected_value, actual_value);
  }
  {
    using Tuple1Type = std::tuple<double, int, char>;
    using Tuple2Type = std::tuple<char, float, int>;
    const std::size_t expected_value = 2;
    const std::size_t actual_value =
        CountTheSameTypesInUnorderedTuples<Tuple1Type, Tuple2Type>::value;
    EXPECT_EQ(expected_value, actual_value);
  }
  {
    using Tuple1Type = std::tuple<double, int, float>;
    using Tuple2Type = std::tuple<char, double, int>;
    const std::size_t expected_value = 2;
    const std::size_t actual_value =
        CountTheSameTypesInUnorderedTuples<Tuple1Type, Tuple2Type>::value;
    EXPECT_EQ(expected_value, actual_value);
  }
}

TEST(FindRepeatedTypes, SimpleTest)
{
  {
    const bool actual_value = IsSomeTypeDuplicated<int>::value;
    EXPECT_FALSE(actual_value);
  }
  {
    const bool actual_value = IsSomeTypeDuplicated<int, char, double>::value;
    EXPECT_FALSE(actual_value);
  }
  {
    const bool actual_value = IsSomeTypeDuplicated<int, char, int, double>::value;
    EXPECT_TRUE(actual_value);
  }
}

}  // namespace
