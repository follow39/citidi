#ifndef CITIDI_INCLUDE_ELEMENT_HPP
#define CITIDI_INCLUDE_ELEMENT_HPP

#include "include/utils.hpp"

template <typename DType, typename... MarkerTypes>
struct Element {
  using Check = IsSomeTypeIsDuplicatedAssertion<MarkerTypes...>;
  using DataType = DType;
  using MarkerTypesTupleType = std::tuple<MarkerTypes...>;

  Element() = default;

  Element(const Element<DType, MarkerTypes...>&) = delete;
  Element(Element<DType, MarkerTypes...>&&) = delete;
  Element<DType, MarkerTypes...> operator=(
      const Element<DType, MarkerTypes...>&) = delete;
  Element<DType, MarkerTypes...> operator=(Element<DType, MarkerTypes...>&&) =
      delete;

  DType data;
};

#endif  // CITIDI_INCLUDE_ELEMENT_HPP
