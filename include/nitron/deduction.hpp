/**
 * @file deduction.hpp
 * @brief Helper utilities for template argument deduction across nitron testing components.
 */
#pragma once

#include <type_traits>

namespace nitron {

template <typename F, typename = void>
struct first_argument {
    using type = void;
};

template <typename R, typename Arg, typename... Args>
struct first_argument<R(*)(Arg, Args...)> {
    using type = std::remove_cvref_t<Arg>;
};

template <typename C, typename R, typename Arg, typename... Args>
struct first_argument<R(C::*)(Arg, Args...) const> {
    using type = std::remove_cvref_t<Arg>;
};

template <typename C, typename R, typename Arg, typename... Args>
struct first_argument<R(C::*)(Arg, Args...)> {
    using type = std::remove_cvref_t<Arg>;
};

template <typename F>
struct first_argument<F, std::void_t<decltype(&F::operator())>>
    : first_argument<decltype(&F::operator())> {};

template <typename F>
using first_argument_t = typename first_argument<std::decay_t<F>>::type;

} // namespace nitron
