#ifndef CITIDI_INCLUDE_SLICE_HPP
#define CITIDI_INCLUDE_SLICE_HPP

#include <tuple>
#include <type_traits>

#include "include/condition.hpp"
#include "include/utils.hpp"

template<typename BType, typename Cond>
class Slice;

namespace
{

struct ArrayMaker
{
  template<std::size_t I, std::size_t J>
  struct Eq
  {
    constexpr static bool value = I == J;
  };

  template<
      std::size_t AI,
      std::size_t AS,
      std::size_t DI,
      std::size_t DS,
      typename D,
      typename C,
      std::enable_if_t<
          !(C::template Evaluate<
              typename std::tuple_element_t<DI, D>::MarkerTypesTupleType>()),
          bool> = true>
  constexpr static void MakeImpl(std::array<std::size_t, AS>& arr)
  {
    Make<AI, AS, DI + 1, DS, D, C>(arr);
  }

  template<
      std::size_t AI,
      std::size_t AS,
      std::size_t DI,
      std::size_t DS,
      typename D,
      typename C,
      std::enable_if_t<
          (C::template Evaluate<
              typename std::tuple_element_t<DI, D>::MarkerTypesTupleType>()),
          bool> = true>
  constexpr static void MakeImpl(std::array<std::size_t, AS>& arr)
  {
    std::get<AI>(arr) = DI;
    Make<AI + 1, AS, DI + 1, DS, D, C>(arr);
  }

  template<
      std::size_t AI,
      std::size_t AS,
      std::size_t DI,
      std::size_t DS,
      typename D,
      typename C,
      std::enable_if_t<!(Eq<AI, AS>::value || Eq<DI, DS>::value), bool> = true>
  constexpr static void Make(std::array<std::size_t, AS>& arr)
  {
    MakeImpl<AI, AS, DI, DS, D, C>(arr);
  }

  template<
      std::size_t AI,
      std::size_t AS,
      std::size_t DI,
      std::size_t DS,
      typename D,
      typename C,
      std::enable_if_t<(Eq<AI, AS>::value || Eq<DI, DS>::value), bool> = true>
  constexpr static void Make(std::array<std::size_t, AS>&)
  {
  }
};

template<typename D, typename C, std::size_t ArraySize>
constexpr static std::array<std::size_t, ArraySize> MakeArray()
{
  std::array<std::size_t, ArraySize> arr {};
  ArrayMaker::template Make<0, ArraySize, 0, std::tuple_size<D>::value, D, C>(
      arr);
  return arr;
};

template<typename BT, typename C>
struct MakeConditionType
{
  using Condition = C;
};

template<typename BT, typename COld, typename CNew>
struct MakeConditionType<Slice<BT, COld>, CNew>
{
  using Condition = And<COld, CNew>;
};

}  // namespace

template<typename BType, typename Cond>
class Slice
{
public:
  using DType = typename BType::DType;
  using Condition = typename MakeConditionType<BType, Cond>::Condition;

  Slice(DType& t)
      : base_data {t}
  {
  }

  template<typename C>
  operator Slice<Slice<BType, Condition>, C>()
  {
    return {base_data};
  }

  constexpr std::size_t Size() { return kArraySize; }

  template<typename C>
  auto Get()
  {
    return Slice<Slice<BType, Condition>, C> {base_data};
  }

  template<typename C>
  auto Get() const
  {
    return Slice<Slice<const BType, Condition>, C> {base_data};
  }

  template<std::size_t LocalIndex>
  auto& Get()
  {
    constexpr std::size_t kGlobalIndex =
        std::get<LocalIndex>(kUsedElementsArray);
    return std::get<kGlobalIndex>(base_data);
  }

  template<std::size_t LocalIndex>
  auto& Get() const
  {
    constexpr std::size_t kGlobalIndex =
        std::get<LocalIndex>(kUsedElementsArray);
    return std::get<kGlobalIndex>(base_data);
  }

  auto& Single()
  {
    static_assert(1 == kArraySize, "There is more than 1 value");
    return Get<0>();
  }

  const auto& Single() const
  {
    static_assert(1 == kArraySize, "There is more than 1 value");
    return Get<0>();
  }

private:
  constexpr static std::size_t kArraySize {
      CountTypesInsideTupleByCondition<DType, Cond>::value};
  constexpr static std::array<std::size_t, kArraySize> kUsedElementsArray {
      MakeArray<DType, Condition, kArraySize>()};

  DType& base_data;
};

#endif  // CITIDI_INCLUDE_SLICE_HPP
