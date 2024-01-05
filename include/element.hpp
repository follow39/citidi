#ifndef CITIDI_INCLUDE_ELEMENT_HPP
#define CITIDI_INCLUDE_ELEMENT_HPP

#include "include/utils.hpp"

template<typename DType, typename... MTypes>
struct Element
{
  using Check = CheckMarkerTypesForUniqueness<MTypes...>;
  using DataType = DType;
  using MarkerTypes = typename MergeMarkers<MTypes...>::MarkerTypes;

  DType data;
};

#endif  // CITIDI_INCLUDE_ELEMENT_HPP
