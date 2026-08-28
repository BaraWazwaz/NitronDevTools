#pragma once

#include <concepts>
#include <type_traits>

namespace nitron
{

/// @brief checks if class is a functor that takes some argument list 
///        and its return value is exactly expected type
/// @tparam Functor checked type
/// @tparam Result  return type expected
/// @tparam Args    argument list types
template <typename Functor, typename Result, typename... Args>
concept StrictFunctor = std::is_invocable_r_v<Result, Functor, Args...>;

/// @brief checks if class is a functor that takes some argument list 
///        and its return value is convertible to expected type
/// @tparam Functor checked type
/// @tparam Result  return type expected
/// @tparam Args    argument list types
template <typename Functor, typename Result, typename... Args>
concept LooseFunctor = std::invocable<Functor, Args...> &&
                       std::convertible_to<std::invoke_result_t<Functor, Args...>, Result>;

} // namespace nitron
