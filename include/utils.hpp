#ifndef CITIDI_INCLUDE_UTILS_H
#define CITIDI_INCLUDE_UTILS_H

#include <tuple>
#include <type_traits>

template<typename SearchType, typename... PackTypes>
struct FindTypeInsidePack
{
  template<typename SType, typename PType>
  constexpr static bool FindTypeInsidePackImpl()
  {
    return std::is_same<SType, PType>::value;
  }

  template<typename SType,
           typename PType,
           typename PTypeNext,
           typename... PTypes>
  constexpr static bool FindTypeInsidePackImpl()
  {
    return std::is_same<SType, PType>::value
        || FindTypeInsidePackImpl<SType, PTypeNext, PTypes...>();
  }

  constexpr static bool value =
      FindTypeInsidePackImpl<SearchType, PackTypes...>();
};

template<typename SearchType, typename TupleType>
struct FindTypeInsideTuple
{
  template<typename SType, typename TType>
  struct FindTypeInsideTupleImpl
  {
  };

  template<typename SType, typename... Ts>
  struct FindTypeInsideTupleImpl<SType, std::tuple<Ts...>>
  {
    constexpr static std::size_t value =
        FindTypeInsidePack<SType, Ts...>::value;
  };

  constexpr static bool value =
      FindTypeInsideTupleImpl<SearchType, TupleType>::value;
};

template<typename TupleType1, typename TupleType2>
struct CountTheSameTypesInUnorderedTuples
{
  template<std::size_t I, std::size_t S, typename TType1, typename TType2>
  struct CountTuplesImpl
  {
    constexpr static std::size_t value =
        static_cast<std::size_t>(
            FindTypeInsideTuple<std::tuple_element_t<I, TType1>, TType2>::value)
        + CountTuplesImpl<I + 1, S, TType1, TType2>::value;
  };

  template<std::size_t S, typename TType1, typename TType2>
  struct CountTuplesImpl<S, S, TType1, TType2>
  {
    constexpr static std::size_t value = 0U;
  };

  constexpr static std::size_t value =
      CountTuplesImpl<0,
                      std::tuple_size<TupleType1>::value,
                      TupleType1,
                      TupleType2>::value;
};

#endif  // CITIDI_INCLUDE_UTILS_H
