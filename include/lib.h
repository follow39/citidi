#ifndef CITIDI_INCLUDE_LIB_H
#define CITIDI_INCLUDE_LIB_H

#include <tuple>
#include <type_traits>

namespace match
{

template<typename T>
constexpr bool find_type_inside_tuple()
{
  return false;
}

template<typename T, typename U, typename... Ts>
constexpr bool find_type_inside_tuple()
{
  return std::is_same<T, U>::value || find_type_inside_tuple<T, Ts...>();
}

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
struct CountUnorderedTuples
{
  template<typename U1, typename U2>
  struct CountTuplesImpl
  {
    constexpr static std::size_t v = 0;
  };

  template<typename U, typename... U1s, typename... U2s>
  struct CountTuplesImpl<std::tuple<U, U1s...>, std::tuple<U2s...>>
  {
    constexpr static std::size_t v = find_type_inside_tuple<U, U2s...>()
        + CountTuplesImpl<std::tuple<U1s...>, std::tuple<U2s...>>::v;
  };

  template<typename... U2s>
  struct CountTuplesImpl<void, std::tuple<U2s...>>
  {
    constexpr static std::size_t v = 0;
  };

  constexpr static std::size_t value = CountTuplesImpl<T1, T2>::v;
};

template<typename T1, typename T2>
struct MatchUnorderedTuplesExactly
{
  constexpr static std::size_t number = CountUnorderedTuples<T1, T2>::value;
  constexpr static bool match =
      (Eq<std::tuple_size<T1>::value, std::tuple_size<T2>::value>::value)
      && (Eq<std::tuple_size<T1>::value, number>::value);
};

template<typename T1, typename T2>
struct FindFirstTupleInsideSecond
{
  constexpr static std::size_t number = CountUnorderedTuples<T1, T2>::value;
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

static constexpr size_t kNotFound = static_cast<size_t>(-1);
static constexpr size_t kAmbiguous = kNotFound - 1;

constexpr size_t find_idx_return(size_t cidx, size_t res, bool matches)
{
  return !matches ? res : (res == kNotFound ? cidx : kAmbiguous);
}

template<size_t Nx>
constexpr size_t find_idx(const size_t idx, const std::array<bool, Nx>& matches)
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
  static constexpr size_t value = find_idx(0, matches);
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

template<typename DType, typename... MTypes>
struct Element
{
  using Check = CheckMarkerTypesForUniqueness<MTypes...>;
  using DataType = DType;
  using MarkerTypes = typename MergeMarkers<MTypes...>::MarkerTypes;

  DType data;
};

template<typename T, typename... MTypes>
struct Slice
{
  using Check = CheckMarkerTypesForUniqueness<MTypes...>;
  using MTuple = typename MergeMarkers<MTypes...>::MarkerTypes;

  template<std::size_t I, typename U>
  struct Cond
  {
    constexpr static bool value = match::FindFirstTupleInsideSecond<
        MTuple,
        typename std::remove_reference_t<
            std::tuple_element_t<I, U>>::MarkerTypes>::value;
  };

  template<std::size_t I,
           typename R,
           std::enable_if_t<Cond<I, T>::value, bool> = true>
  constexpr static auto AddValueCond(T& t, R r)
  {
    return std::tuple_cat(
        std::tuple<typename std::add_lvalue_reference<
            std::tuple_element_t<I, T>>::type> {std::get<I>(t)},
        r);
  }

  template<std::size_t I,
           typename R,
           std::enable_if_t<!Cond<I, T>::value, bool> = true>
  constexpr static auto AddValueCond(T&, R r)
  {
    return r;
  }

  template<std::size_t I,
           std::size_t S,
           typename R,
           std::enable_if_t<(I >= S), bool> = true>
  constexpr static auto CreateImpl(T&, R r)
  {
    return r;
  }

  template<std::size_t I,
           std::size_t S,
           typename R,
           std::enable_if_t<(I < S), bool> = true>
  constexpr static auto CreateImpl(T& t, R r)
  {
    auto r2 = AddValueCond<I>(t, r);

    return CreateImpl<I + 1, S>(t, r2);
  }

  constexpr static auto Create(T& t)
  {
    constexpr std::size_t ss {std::tuple_size<T>()};
    return CreateImpl<0U, ss>(t, std::tuple<> {});
  }

  template<typename TT, typename MM>
  struct CreateType
  {
    template<typename V, typename U>
    struct Foo
    {
    };

    template<typename V, typename... Us>
    struct Foo<V, std::tuple<Us...>>
    {
      using type = std::tuple<V&, Us...>;
    };

    template<std::size_t I, typename U, typename Enable = void>
    struct Bar
    {
      using type = U;
    };

    template<std::size_t I, typename U>
    struct Bar<I, U, typename std::enable_if_t<Cond<I, T>::value>>
    {
      using type = typename Foo<std::tuple_element_t<I, T>, U>::type;
    };

    template<std::size_t I, std::size_t S, typename U>
    struct CreateTypeImpl
    {
      using V = typename Bar<I, U>::type;
      using type = typename CreateTypeImpl<I + 1, S, V>::type;
    };

    template<std::size_t S, typename U>
    struct CreateTypeImpl<S, S, U>
    {
      using type = U;
    };

    using type = typename CreateTypeImpl<0,
                                         std::tuple_size<TT>::value,
                                         std::tuple<>>::type;
  };

  using DType = typename CreateType<T, MTuple>::type;

  Slice(T& t)
      : data {Create(t)}
  {
  }

  template<typename S, typename V>
  struct FindIndex
  {
  };

  template<typename S, typename... ETs>
  struct FindIndex<S, std::tuple<ETs...>>
  {
    constexpr static std::size_t value =
        FindElement<S, typename std::remove_reference_t<ETs>::MarkerTypes...>::
            value;
  };

  template<typename... MTs>
  auto& Get()
  {
    std::ignore = CheckMarkerTypesForUniqueness<MTs...> {};
    using SType = typename MergeMarkers<MTs...>::MarkerTypes;
    return std::get<FindIndex<SType, DType>::value>(this->data);
  }

  template<typename... MTs>
  const auto& Get() const
  {
    std::ignore = CheckMarkerTypesForUniqueness<MTs...> {};
    using SType = typename MergeMarkers<MTs...>::MarkerTypes;
    return std::get<FindIndex<SType, DType>::value>(this->data);
  }

  template<typename... MTs>
  auto ShrinkTo()
  {
    return Slice<DType, MTs...>::Create(this->data);
  }

  DType data;
};

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

#endif  // CITIDI_INCLUDE_LIB_H
