#ifndef CITIDI_INCLUDE_UTILS_H
#define CITIDI_INCLUDE_UTILS_H

#include <array>
#include <tuple>
#include <type_traits>

template <typename SearchType, typename... PackTypes>
struct IsTypeInsidePack {
  template <typename SType, typename PType>
  constexpr static bool IsTypeInsidePackImpl() {
    return std::is_same<SType, PType>::value;
  }

  template <typename SType, typename PType, typename PTypeNext,
            typename... PTypes>
  constexpr static bool IsTypeInsidePackImpl() {
    return std::is_same<SType, PType>::value ||
           IsTypeInsidePackImpl<SType, PTypeNext, PTypes...>();
  }

  constexpr static bool value =
      IsTypeInsidePackImpl<SearchType, PackTypes...>();
};

template <typename SearchType, typename TupleType>
struct IsTypeInsideTuple {
  template <typename SType, typename TType>
  struct IsTypeInsideTupleImpl {};

  template <typename SType, typename... Ts>
  struct IsTypeInsideTupleImpl<SType, std::tuple<Ts...>> {
    constexpr static std::size_t value = IsTypeInsidePack<SType, Ts...>::value;
  };

  constexpr static bool value =
      IsTypeInsideTupleImpl<SearchType, TupleType>::value;
};

template <typename TupleType1, typename TupleType2>
struct CountTheSameTypesInUnorderedTuples {
  template <std::size_t I, std::size_t S, typename TType1, typename TType2>
  struct CountTuplesImpl {
    constexpr static std::size_t value =
        static_cast<std::size_t>(
            IsTypeInsideTuple<std::tuple_element_t<I, TType1>, TType2>::value) +
        CountTuplesImpl<I + 1, S, TType1, TType2>::value;
  };

  template <std::size_t S, typename TType1, typename TType2>
  struct CountTuplesImpl<S, S, TType1, TType2> {
    constexpr static std::size_t value = 0U;
  };

  constexpr static std::size_t value =
      CountTuplesImpl<0, std::tuple_size<TupleType1>::value, TupleType1,
                      TupleType2>::value;
};

template <typename DType, typename Cond>
struct CountTypesInsideTupleByCondition {
  template <std::size_t I, std::size_t S, typename D, typename C>
  struct CountTypesInsideTupleByConditionImpl {
    constexpr static std::size_t value =
        static_cast<std::size_t>(
            C::template Evaluate<
                typename std::tuple_element_t<I, D>::MarkerTypesTupleType>()) +
        CountTypesInsideTupleByConditionImpl<I + 1, S, D, C>::value;
  };

  template <std::size_t S, typename D, typename C>
  struct CountTypesInsideTupleByConditionImpl<S, S, D, C> {
    constexpr static std::size_t value = 0U;
  };

  constexpr static std::size_t value =
      CountTypesInsideTupleByConditionImpl<0, std::tuple_size<DType>::value,
                                           DType, Cond>::value;
};

template <typename... Types>
struct IsSomeTypeDuplicated {
  using D = std::tuple<Types...>;
  constexpr static std::size_t s = std::tuple_size<D>();

  template <std::size_t I, std::size_t J, std::size_t S>
  struct IsSomeTypeDuplicatedImpl {
    constexpr static bool value =
        std::is_same<std::tuple_element_t<I, D>,
                     std::tuple_element_t<J, D>>::value ||
        IsSomeTypeDuplicatedImpl<I, J + 1, S>::value;
  };

  template <std::size_t I, std::size_t S>
  struct IsSomeTypeDuplicatedImpl<I, S, S> {
    constexpr static bool value = IsSomeTypeDuplicatedImpl<I + 1, 0, S>::value;
  };

  template <std::size_t I, std::size_t S>
  struct IsSomeTypeDuplicatedImpl<I, I, S> {
    constexpr static bool value = IsSomeTypeDuplicatedImpl<I, I + 1, S>::value;
  };

  template <std::size_t J, std::size_t S>
  struct IsSomeTypeDuplicatedImpl<S, J, S> {
    constexpr static bool value = false;
  };

  constexpr static bool value = IsSomeTypeDuplicatedImpl<0, 0, s>::value;
};

template <typename... Ts>
struct IsSomeTypeIsDuplicatedAssertion {
  constexpr static bool is_some_type_duplicated =
      IsSomeTypeDuplicated<Ts...>::value;
  static_assert(!is_some_type_duplicated, "Marker types should be unique");
};

#endif  // CITIDI_INCLUDE_UTILS_H
