#ifndef CITIDI_INCLUDE_DISPATCHER_HPP
#define CITIDI_INCLUDE_DISPATCHER_HPP

#include <tuple>
#include <type_traits>

#include "include/slice.hpp"
#include "include/utils.hpp"

template<typename... ElementTypes>
class Dispatcher
{
public:
  using DType = std::tuple<ElementTypes...>;

  Dispatcher() = default;

  explicit Dispatcher(std::tuple<ElementTypes...> new_data)
      : data {new_data}
  {
  }

  template<typename... ETypes>
  static auto CreateFromTuple(std::tuple<ETypes...> new_data)
  {
    return Dispatcher<ETypes...> {new_data};
  }

  template<typename... MTypes>
  auto& Get()
  {
    std::ignore = CheckMarkerTypesForUniqueness<MTypes...> {};
    using SType = typename MergeMarkers<MTypes...>::MarkerTypes;
    return std::get<FindElement<
        SType,
        typename std::remove_reference_t<ElementTypes>::MarkerTypes...>::value>(
        this->data);
  }

  template<typename... MTypes>
  const auto& Get() const
  {
    std::ignore = CheckMarkerTypesForUniqueness<MTypes...> {};
    using SType = typename MergeMarkers<MTypes...>::MarkerTypes;
    return std::get<FindElement<
        SType,
        typename std::remove_reference_t<ElementTypes>::MarkerTypes...>::value>(
        this->data);
  }

  template<typename... MTypes>
  auto ShrinkTo()
  {
    return CreateFromTuple(
        Slice<std::tuple<ElementTypes...>, MTypes...>::Create(data));
  }

  template<typename... MTypes>
  operator Slice<DType, MTypes...>()
  {
    return Slice<DType, MTypes...> {data};
  }

  std::tuple<ElementTypes...> data;
};

#endif  // CITIDI_INCLUDE_DISPATCHER_HPP
