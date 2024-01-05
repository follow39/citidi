#ifndef CITIDI_INCLUDE_UTILS_H
#define CITIDI_INCLUDE_UTILS_H

#include <array>
#include <tuple>
#include <type_traits>

template<typename SearchType, typename... PackTypes>
struct IsTypeInsidePack
{
  template<typename SType, typename PType>
  constexpr static bool IsTypeInsidePackImpl()
  {
    return std::is_same<SType, PType>::value;
  }

  template<typename SType,
           typename PType,
           typename PTypeNext,
           typename... PTypes>
  constexpr static bool IsTypeInsidePackImpl()
  {
    return std::is_same<SType, PType>::value
        || IsTypeInsidePackImpl<SType, PTypeNext, PTypes...>();
  }

  constexpr static bool value =
      IsTypeInsidePackImpl<SearchType, PackTypes...>();
};

template<typename SearchType, typename TupleType>
struct IsTypeInsideTuple
{
  template<typename SType, typename TType>
  struct IsTypeInsideTupleImpl
  {
  };

  template<typename SType, typename... Ts>
  struct IsTypeInsideTupleImpl<SType, std::tuple<Ts...>>
  {
    constexpr static std::size_t value = IsTypeInsidePack<SType, Ts...>::value;
  };

  constexpr static bool value =
      IsTypeInsideTupleImpl<SearchType, TupleType>::value;
};

template<typename TupleType1, typename TupleType2>
struct CountTheSameTypesInUnorderedTuples
{
  template<std::size_t I, std::size_t S, typename TType1, typename TType2>
  struct CountTuplesImpl
  {
    constexpr static std::size_t value =
        static_cast<std::size_t>(
            IsTypeInsideTuple<std::tuple_element_t<I, TType1>, TType2>::value)
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

// TODO: To be refactoredb

namespace match
{

template<std::size_t N1, std::size_t N2>
struct Eq
{
  static constexpr bool value = (N1 == N2);
};

template<std::size_t N1, std::size_t N2>
struct Le
{
  static constexpr bool value = (N1 <= N2);
};

template<typename T1, typename T2>
struct MatchUnorderedTuplesExactly
{
  constexpr static std::size_t number =
      CountTheSameTypesInUnorderedTuples<T1, T2>::value;
  constexpr static bool match =
      (Eq<std::tuple_size<T1>::value, std::tuple_size<T2>::value>::value)
      && (Eq<std::tuple_size<T1>::value, number>::value);
};

template<typename T1, typename T2>
struct FindFirstTupleInsideSecond
{
  constexpr static std::size_t number =
      CountTheSameTypesInUnorderedTuples<T1, T2>::value;
  constexpr static bool value =
      (Le<std::tuple_size<T1>::value, std::tuple_size<T2>::value>::value)
      && (Eq<std::tuple_size<T1>::value, number>::value);
};

}  // namespace match

template<typename... Types>
struct FindRepeatedTypes
{
  using D = std::tuple<Types...>;
  constexpr static std::size_t s = std::tuple_size<D>();

  template<std::size_t I, std::size_t J, std::size_t S>
  struct FindRepeatedTypesImpl
  {
    constexpr static bool value =
        std::is_same<std::tuple_element_t<I, D>,
                     std::tuple_element_t<J, D>>::value
        || FindRepeatedTypesImpl<I, J + 1, S>::value;
  };

  template<std::size_t I, std::size_t S>
  struct FindRepeatedTypesImpl<I, S, S>
  {
    constexpr static bool value = FindRepeatedTypesImpl<I + 1, 0, S>::value;
  };

  template<std::size_t J, std::size_t S>
  struct FindRepeatedTypesImpl<S, J, S>
  {
    constexpr static bool value = false;
  };

  constexpr static bool value = FindRepeatedTypesImpl<0, 0, s>::value;
};

template<typename... Ts>
struct CheckMarkerTypesForUniqueness
{
  constexpr static bool is_uniq = !FindRepeatedTypes<Ts...>::value;
  static_assert(!is_uniq, "Marker types should be uniq");
};

namespace detail
{

static constexpr std::size_t kNotFound = static_cast<std::size_t>(-1);
static constexpr std::size_t kAmbiguous = kNotFound - 1;

constexpr std::size_t find_idx_return(std::size_t cidx,
                                      std::size_t res,
                                      bool matches)
{
  return !matches ? res : (res == kNotFound ? cidx : kAmbiguous);
}

template<std::size_t Nx>
constexpr std::size_t find_idx(const std::size_t idx,
                               const std::array<bool, Nx>& matches)
{
  return idx == Nx
      ? kNotFound
      : find_idx_return(idx, find_idx(idx + 1, matches), matches[idx]);
}

template<typename SType, typename... ETypes>
struct FindExactlyOneChecked
{
  static constexpr std::array<bool, sizeof...(ETypes)> matches = {
      match::MatchUnorderedTuplesExactly<SType, ETypes>::match...};
  static constexpr std::size_t value = find_idx(0, matches);
  static_assert(value != kNotFound, "type not found in type list");
  static_assert(value != kAmbiguous,
                "type occurs more than once in type  list");
};

template<typename SType>
struct FindExactlyOneChecked<SType>
{
  static_assert(!std::is_same<SType, SType>::value,
                "type not in empty type list");
};

}  // namespace detail

template<typename SType, typename... ETypes>
struct FindElement : detail::FindExactlyOneChecked<SType, ETypes...>
{
};

template<typename T1, typename T2 = void, typename... Ts>
struct MergeTuples
{
  template<typename TT1, typename TT2>
  struct MergeTuplesImpl;

  template<typename... Args1, typename... Args2>
  struct MergeTuplesImpl<std::tuple<Args1...>, std::tuple<Args2...>>
  {
    using R = std::tuple<Args1..., Args2...>;
  };

  using T = typename MergeTuplesImpl<T1, T2>::R;
  using R = typename MergeTuples<T, Ts...>::R;
};

template<typename T1>
struct MergeTuples<T1, void>
{
  using R = T1;
};

struct MarkerGroupBase
{
};

template<typename... MTypes>
struct MarkerGroup : public MarkerGroupBase
{
  using MarkerTypes = std::tuple<MTypes...>;
  static const std::size_t N = sizeof...(MTypes);
};

template<typename Arg, typename Enable = void>
struct MakeTuple
{
  using T = std::tuple<Arg>;
};

template<typename Arg>
struct MakeTuple<
    Arg,
    typename std::enable_if<std::is_base_of<MarkerGroupBase, Arg>::value>::type>
{
  using T = typename Arg::MarkerTypes;
};

template<typename... Args>
struct MergeMarkers
{
  using MarkerTypes = typename MergeTuples<typename MakeTuple<Args>::T...>::R;
};

#endif  // CITIDI_INCLUDE_UTILS_H
