#include "include/dispatcher.hpp"
#include "include/element.hpp"

#include "gtest/gtest.h"

namespace
{

using MG = MarkerGroup<char, int>;
using MG2 = MarkerGroup<double>;

using DD =
    Dispatcher<Element<int, MG>, Element<int, int>, Element<double, double>>;

TEST(Dispatcher, SimpleTest)
{
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
