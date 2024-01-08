#ifndef CITIDI_INCLUDE_CONDITION_H
#define CITIDI_INCLUDE_CONDITION_H

#include <tuple>

#include "include/utils.hpp"

template<bool Value>
struct BoolConst
{
  template<typename T>
  constexpr static bool Evaluate()
  {
    return Value;
  }
};

template<typename... OperationTypes>
struct Or
{
  template<typename MarkersTupleType, typename Op>
  constexpr static bool EvaluateImpl()
  {
    return Op::template Evaluate<MarkersTupleType>();
  }

  template<typename MarkersTupleType,
           typename Op,
           typename OpNext,
           typename... OpTypes>
  constexpr static bool EvaluateImpl()
  {
    return EvaluateImpl<MarkersTupleType, Op>()
        || EvaluateImpl<MarkersTupleType, OpNext, OpTypes...>();
  }

  template<typename MarkersTupleType>
  constexpr static bool Evaluate()
  {
    return EvaluateImpl<MarkersTupleType, OperationTypes...>();
  }
};

template<typename... OperationTypes>
struct And
{
  template<typename MarkersTupleType, typename Op>
  constexpr static bool EvaluateImpl()
  {
    return Op::template Evaluate<MarkersTupleType>();
  }

  template<typename MarkersTupleType,
           typename Op,
           typename OpNext,
           typename... OpTypes>
  constexpr static bool EvaluateImpl()
  {
    return EvaluateImpl<MarkersTupleType, Op>()
        && EvaluateImpl<MarkersTupleType, OpNext, OpTypes...>();
  }

  template<typename MarkersTupleType>
  constexpr static bool Evaluate()
  {
    return EvaluateImpl<MarkersTupleType, OperationTypes...>();
  }
};

template<typename OperationType>
struct Not
{
  template<typename MarkersTupleType>
  constexpr static bool Evaluate()
  {
    return !(OperationType::template Evaluate<MarkersTupleType>());
  }
};

template<typename... MarkerTypes>
struct With
{
  using Check = CheckMarkerTypesForUniqueness<MarkerTypes...>;

  template<typename T1, typename T2>
  constexpr static bool EvaluateImpl()
  {
    return std::tuple_size<T1>::value
        == CountTheSameTypesInUnorderedTuples<T1, T2>::value;
  }

  template<typename MarkersTupleType>
  constexpr static bool Evaluate()
  {
    return EvaluateImpl<std::tuple<MarkerTypes...>, MarkersTupleType>();
  }
};

template<typename... MarkerTypes>
struct WithExactly
{
  using Check = CheckMarkerTypesForUniqueness<MarkerTypes...>;

  template<typename T1, typename T2>
  constexpr static bool EvaluateImpl()
  {
    return (std::tuple_size<T1>::value == std::tuple_size<T2>::value)
        && (std::tuple_size<T1>::value
            == CountTheSameTypesInUnorderedTuples<T1, T2>::value);
  }

  template<typename MarkersTupleType>
  constexpr static bool Evaluate()
  {
    return EvaluateImpl<std::tuple<MarkerTypes...>, MarkersTupleType>();
  }
};

// Without works as AND condition internaly
template<typename... MarkerTypes>
struct Without
{
  using Check = CheckMarkerTypesForUniqueness<MarkerTypes...>;

  template<typename MarkersTupleType>
  constexpr static bool Evaluate()
  {
    return Not<With<MarkerTypes...>>::template Evaluate<MarkersTupleType>();
  }
};

template<typename MarkersTupleType, typename Condition>
struct ConditionEvaluator
{
  constexpr static bool value =
      Condition::template Evaluate<MarkersTupleType>();
};

#endif  // CITIDI_INCLUDE_CONDITION_H
