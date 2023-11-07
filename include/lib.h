#ifndef CITIDI_INCLUDE_LIB_H
#define CITIDI_INCLUDE_LIB_H

#include <tuple>
#include <type_traits>

#include "include/tuple_sort.h"

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
      std::is_same<SType,
                   typename SortTuple<typename ETypes::MarkerTypes,
                                      DefaultComparator>::R>::value...};
  static constexpr size_t value = find_idx(0, matches);
  static_assert(value != kNotFound, "type not found in type list");
  static_assert(value != kAmbiguous, "type occurs more than once in type list");
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
  using DataType = DType;
  using MarkerTypes =
      typename SortTuple<typename MergeMarkers<MTypes...>::MarkerTypes,
                         DefaultComparator>::R;

  DType data;
};

template<typename... ElementTypes>
class Dispatcher
{
public:
  template<typename... MTypes>
  auto& Get()
  {
    using SType =
        typename SortTuple<typename MergeMarkers<MTypes...>::MarkerTypes,
                           DefaultComparator>::R;
    return std::get<FindElement<SType, ElementTypes...>::value>(this->data_);
  }

private:
  std::tuple<ElementTypes...> data_;
};

#endif  // CITIDI_INCLUDE_LIB_H
