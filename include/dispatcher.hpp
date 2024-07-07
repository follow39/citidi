#ifndef CITIDI_INCLUDE_DISPATCHER_HPP
#define CITIDI_INCLUDE_DISPATCHER_HPP

#include <tuple>

#include "include/slice.hpp"

template <typename... ElementTypes>
class Dispatcher {
 public:
  using DType = std::tuple<ElementTypes...>;

  Dispatcher() = default;

  Dispatcher(const Dispatcher<ElementTypes...>&) = delete;
  Dispatcher(Dispatcher<ElementTypes...>&&) = delete;
  Dispatcher<ElementTypes...> operator=(const Dispatcher<ElementTypes...>&) =
      delete;
  Dispatcher<ElementTypes...> operator=(Dispatcher<ElementTypes...>&&) = delete;

  template <typename Condition>
  auto Get() {
    return Slice<Dispatcher<ElementTypes...>, Condition>{data};
  }

  template <typename Condition>
  operator Slice<Dispatcher<ElementTypes...>, Condition>() {
    return {data};
  }

 private:
  DType data;
};

#endif  // CITIDI_INCLUDE_DISPATCHER_HPP
