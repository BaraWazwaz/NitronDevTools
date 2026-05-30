#pragma once

#include <numeric>
#include <cmath>
#include <complex>
#include "util/Functor.hpp"
#include "dev/Pass.hpp"

namespace nitron
{

template <typename T>
struct is_complex : std::false_type {};

template <typename T>
struct is_complex<std::complex<T>> : std::true_type {};

template <typename T>
concept LegacyMath = std::is_arithmetic_v<T> || is_complex<T>::value;

template <typename T>
concept Number = LegacyMath<T> ||
                 requires(T x)
                 {
                     typename T::number_tag;
                     typename T::value_type;
                 };

template <typename T> T sgn (T const& x)             { NITRON_REQUIRES_SPECIALIZATION; return T{}; }
template <typename T> T pow (T const& x, T const& y) { NITRON_REQUIRES_SPECIALIZATION; return T{}; }
template <typename T> T exp (T const& x)             { NITRON_REQUIRES_SPECIALIZATION; return T{}; }
template <typename T> T log (T const& x, T const& y) { NITRON_REQUIRES_SPECIALIZATION; return T{}; }
template <typename T> T log (T const& x)             { NITRON_REQUIRES_SPECIALIZATION; return T{}; }
template <typename T> T sqrt(T const& x)             { NITRON_REQUIRES_SPECIALIZATION; return T{}; }

template <typename T> T sin(T const& x) { NITRON_REQUIRES_SPECIALIZATION; return T{}; }
template <typename T> T cos(T const& x) { NITRON_REQUIRES_SPECIALIZATION; return T{}; }
template <typename T> T tan(T const& x) { NITRON_REQUIRES_SPECIALIZATION; return T{}; }
template <typename T> T cot(T const& x) { NITRON_REQUIRES_SPECIALIZATION; return T{}; }
template <typename T> T sec(T const& x) { NITRON_REQUIRES_SPECIALIZATION; return T{}; }
template <typename T> T csc(T const& x) { NITRON_REQUIRES_SPECIALIZATION; return T{}; }

template <typename T> T asin(T const& x) { NITRON_REQUIRES_SPECIALIZATION; return T{}; }
template <typename T> T acos(T const& x) { NITRON_REQUIRES_SPECIALIZATION; return T{}; }
template <typename T> T atan(T const& x) { NITRON_REQUIRES_SPECIALIZATION; return T{}; }
template <typename T> T acot(T const& x) { NITRON_REQUIRES_SPECIALIZATION; return T{}; }
template <typename T> T asec(T const& x) { NITRON_REQUIRES_SPECIALIZATION; return T{}; }
template <typename T> T acsc(T const& x) { NITRON_REQUIRES_SPECIALIZATION; return T{}; }

template <typename T> T sinh(T const& x) { NITRON_REQUIRES_SPECIALIZATION; return T{}; }
template <typename T> T cosh(T const& x) { NITRON_REQUIRES_SPECIALIZATION; return T{}; }
template <typename T> T tanh(T const& x) { NITRON_REQUIRES_SPECIALIZATION; return T{}; }
template <typename T> T coth(T const& x) { NITRON_REQUIRES_SPECIALIZATION; return T{}; }
template <typename T> T sech(T const& x) { NITRON_REQUIRES_SPECIALIZATION; return T{}; }
template <typename T> T csch(T const& x) { NITRON_REQUIRES_SPECIALIZATION; return T{}; }

template <typename T> T asinh(T const& x) { NITRON_REQUIRES_SPECIALIZATION; return T{}; }
template <typename T> T acosh(T const& x) { NITRON_REQUIRES_SPECIALIZATION; return T{}; }
template <typename T> T atanh(T const& x) { NITRON_REQUIRES_SPECIALIZATION; return T{}; }
template <typename T> T acoth(T const& x) { NITRON_REQUIRES_SPECIALIZATION; return T{}; }
template <typename T> T asech(T const& x) { NITRON_REQUIRES_SPECIALIZATION; return T{}; }
template <typename T> T acsch(T const& x) { NITRON_REQUIRES_SPECIALIZATION; return T{}; }

template <typename T> requires LegacyMath<T> T sgn (T const& x)             { return (T{0} < x) - (x < T{0}); }
template <typename T> requires LegacyMath<T> T pow (T const& x, T const& y) { return std::pow(x, y); }
template <typename T> requires LegacyMath<T> T exp (T const& x)             { return std::exp(x); }
template <typename T> requires LegacyMath<T> T log (T const& x, T const& y) { return std::log(x) / std::log(y); }
template <typename T> requires LegacyMath<T> T log (T const& x)             { return std::log(x); }
template <typename T> requires LegacyMath<T> T sqrt(T const& x)             { return std::sqrt(x); }

template <typename T> requires LegacyMath<T> T sin(T const& x) { return std::sin(x); }
template <typename T> requires LegacyMath<T> T cos(T const& x) { return std::cos(x); }
template <typename T> requires LegacyMath<T> T tan(T const& x) { return std::tan(x); }
template <typename T> requires LegacyMath<T> T cot(T const& x) { return T{1} / std::tan(x); }
template <typename T> requires LegacyMath<T> T sec(T const& x) { return T{1} / std::cos(x); }
template <typename T> requires LegacyMath<T> T csc(T const& x) { return T{1} / std::sin(x); }

template <typename T> requires LegacyMath<T> T asin(T const& x) { return std::asin(x); }
template <typename T> requires LegacyMath<T> T acos(T const& x) { return std::acos(x); }
template <typename T> requires LegacyMath<T> T atan(T const& x) { return std::atan(x); }
template <typename T> requires LegacyMath<T> T acot(T const& x) { return std::atan(T{1} / x); }
template <typename T> requires LegacyMath<T> T asec(T const& x) { return std::acos(T{1} / x); }
template <typename T> requires LegacyMath<T> T acsc(T const& x) { return std::asin(T{1} / x); }

template <typename T> requires LegacyMath<T> T sinh(T const& x) { return std::sinh(x); }
template <typename T> requires LegacyMath<T> T cosh(T const& x) { return std::cosh(x); }
template <typename T> requires LegacyMath<T> T tanh(T const& x) { return std::tanh(x); }
template <typename T> requires LegacyMath<T> T coth(T const& x) { return std::tanh(T{1} / x); }
template <typename T> requires LegacyMath<T> T sech(T const& x) { return std::cosh(T{1} / x); }
template <typename T> requires LegacyMath<T> T csch(T const& x) { return std::sinh(T{1} / x); }

template <typename T> requires LegacyMath<T> T asinh(T const& x) { return std::asinh(x); }
template <typename T> requires LegacyMath<T> T acosh(T const& x) { return std::acosh(x); }
template <typename T> requires LegacyMath<T> T atanh(T const& x) { return std::atanh(x); }
template <typename T> requires LegacyMath<T> T acoth(T const& x) { return std::atanh(T{1} / x); }
template <typename T> requires LegacyMath<T> T asech(T const& x) { return std::acosh(T{1} / x); }
template <typename T> requires LegacyMath<T> T acsch(T const& x) { return std::asinh(T{1} / x); }

template <Number T, bool enabler = std::is_arithmetic_v<T>>
struct is_integral;

template <Number T>
struct is_integral<T, true> : std::conditional_t<
                                  std::is_integral_v<T>,
                                  std::true_type,
                                  std::false_type
                              > {};

template <Number T>
struct is_integral<T, false> : std::conditional_t<
                                   is_integral<typename T::value_type>::value,
                                   std::true_type,
                                   std::false_type
                               > {};

template <typename T>
concept Integral      = Number<T> && is_integral<T>::value;

template <typename T>
concept FloatingPoint = Number<T> && !is_integral<T>::value;

template <typename Functor, typename ResultType, typename... Args>
concept NumericalFunctor = LooseFunctor<Functor, ResultType, Args...> &&
                           Number<ResultType>                         &&
                           (Number<Args> && ...);

} // namespace nitron