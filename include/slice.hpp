#ifndef CITIDI_INCLUDE_SLICE_HPP
#define CITIDI_INCLUDE_SLICE_HPP

#include <tuple>
#include <type_traits>

#include "include/utils.hpp"

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

  auto& GetSingle()
  {
    static_assert(1 == std::tuple_size<DType>::value,
                  "There is more than 1 value");
    return std::get<0>(data);
  }

  const auto& GetSingle() const
  {
    static_assert(1 == std::tuple_size<DType>::value,
                  "There is more than 1 value");
    return std::get<0>(data);
  }

  template<typename... MTs>
  auto ShrinkTo()
  {
    return Slice<DType, MTs...>::Create(this->data);
  }

  DType data;
};

#endif  // CITIDI_INCLUDE_SLICE_HPP
