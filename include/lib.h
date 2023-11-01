#ifndef CODISP_INCLUDE_LIB_H
#define CODISP_INCLUDE_LIB_H

#include <tuple>

namespace detail
{

static constexpr size_t kNotFound = static_cast<size_t>(-1);
static constexpr size_t kAmbiguous = kNotFound - 1;

constexpr size_t find_idx_return(size_t cidx, size_t res, bool matches)
{
  return !matches ? res : (res == kNotFound ? cidx : kAmbiguous);
}

template<size_t Nx>
constexpr size_t find_idx(const size_t idx, const bool (&matches)[Nx])
{
  return idx == Nx
      ? kNotFound
      : find_idx_return(idx, find_idx(idx + 1, matches), matches[idx]);
}

template<typename SType, typename... ETypes>
struct FindExactlyOneChecked
{
  static constexpr bool __matches[sizeof...(ETypes)] = {
      std::is_same<SType, typename ETypes::MarkerTypes>::value...};
  static constexpr size_t value = find_idx(0, __matches);
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

template<typename DType, typename... MTypes>
struct Element
{
  using DataType = DType;
  using MarkerTypes = std::tuple<MTypes...>;

  DType data;
};

template<typename... ElementTypes>
class Dispatcher
{
public:
  template<typename... MarkerTypes>
  auto& Get()
  {
    using SType = std::tuple<MarkerTypes...>;
    return std::get<FindElement<SType, ElementTypes...>::value>(this->data_);
  }

private:
  std::tuple<ElementTypes...> data_;
};

#endif  // CODISP_INCLUDE_LIB_H
