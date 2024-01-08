#include "include/dispatcher.hpp"
#include "include/condition.hpp"
#include "include/element.hpp"
#include "include/slice.hpp"

#include "gtest/gtest.h"

namespace
{

using DD = Dispatcher<Element<int, char, int>,
                      Element<int, int>,
                      Element<double, double>>;

TEST(Dispatcher, SimpleTest)
{
  DD disp {};

  auto& a = disp.Get<And<WithExactly<int>>>().Single();
  a.data = 123;
  auto& b = disp.Get<With<double>>().Single();
  b.data = 12.34;
  auto& c = disp.Get<With<char>>().Single();
  c.data = 'A';

  EXPECT_EQ(123, a.data);
  EXPECT_EQ(12.34, b.data);
  EXPECT_EQ('A', c.data);
}

template<typename T>
void TestCoercionInt(Slice<T, With<int>> s)
{
  EXPECT_EQ(s.template Get<With<char>>().Single().data, 7);
  EXPECT_EQ(s.template Get<0>().data, 7);
  EXPECT_EQ(s.template Get<WithExactly<int>>().Single().data, 13);
  EXPECT_EQ(s.template Get<1>().data, 13);
}

template<typename T>
void TestCoercionChar(Slice<T, With<char>> s)
{
  EXPECT_EQ(s.template Get<0>().data, 7);
  EXPECT_EQ(s.Single().data, 7);
}

template<typename T>
void TestCoercionDouble(const Slice<T, WithExactly<double>>& s)
{
  EXPECT_DOUBLE_EQ(s.template Get<0>().data, 12.34);
  EXPECT_DOUBLE_EQ(s.Single().data, 12.34);
}

TEST(Dispatcher, ImplicitCoercionSliceTest)
{
  DD disp {};

  disp.Get<With<char>>().Single().data = 7;
  disp.Get<WithExactly<int>>().Single().data = 13;
  disp.Get<With<double>>().Single().data = 12.34;

  TestCoercionInt<DD>(disp);
  TestCoercionChar<DD>(disp);
  TestCoercionDouble<DD>(disp);

  auto t = disp.Get<With<double>>();
  TestCoercionDouble<decltype(t)>(t);
}

}  // namespace
