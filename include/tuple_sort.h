#ifndef CITIDI_INCLUDE_TUPLE_SORT_H
#define CITIDI_INCLUDE_TUPLE_SORT_H

#include <tuple>
#include <type_traits>
#include <typeindex>

template<typename TType, std::size_t i, std::size_t j>
struct SwapElementTypes
{
  template<typename Is>
  struct SwapElementTypesImpl;

  template<std::size_t... Is>
  struct SwapElementTypesImpl<std::index_sequence<Is...>>
  {
    using R = std::tuple<std::tuple_element_t<Is == i       ? j
                                                  : Is == j ? i
                                                            : Is,
                                              TType>...>;
  };

  using R = typename SwapElementTypesImpl<
      std::make_index_sequence<std::tuple_size<TType>::value>>::R;
};

template<typename ITType, template<typename, typename> class CType>
struct SortTuple
{
  template<std::size_t i, std::size_t j, std::size_t size, typename TType>
  struct SortTupleImpl
  {
    using tt = std::conditional_t<CType<std::tuple_element_t<i, TType>,
                                        std::tuple_element_t<j, TType>>::value,
                                  typename SwapElementTypes<TType, i, j>::R,
                                  TType>;

    using R = typename SortTupleImpl<i, j + 1, size, tt>::R;
  };

  template<std::size_t i, std::size_t size, typename TType>
  struct SortTupleImpl<i, size, size, TType>
  {
    using R = typename SortTupleImpl<i + 1, i + 2, size, TType>::R;
  };

  template<std::size_t j, std::size_t size, typename TType>
  struct SortTupleImpl<size, j, size, TType>
  {
    using R = TType;
  };

  using R =
      typename SortTupleImpl<0, 1, std::tuple_size<ITType>::value, ITType>::R;
};

// TODO implement proper comparator
template<typename LhsType, typename RhsType>
struct DefaultComparator
    : std::conditional_t<(sizeof(LhsType) > sizeof(RhsType)),
                         std::true_type,
                         std::false_type>
{
};

#endif  // CITIDI_INCLUDE_TUPLE_SORT_H
