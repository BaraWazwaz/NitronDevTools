/**
 * @file Singleton.hpp
 * @brief implements the @ref Singleton class
 * @copyright 2026 Bara Wazwaz. Released under the GNU License
 */
#pragma once

#include <concepts>
#include <mutex>

namespace nitron
{

/// @brief a proper singleton should not be publically constructable or moved
template <typename T>
concept ProperSingleton = !std::is_default_constructible_v<T> &&
                          !std::is_copy_constructible_v<T> &&
                          !std::is_move_constructible_v<T> &&
                          !std::is_copy_assignable_v<T> &&
                          !std::is_move_assignable_v<T>;

/// @brief Singleton wrapper for safe access of single instance classes
/// @tparam ValueType needs to have a private or protected default constructor,
///                   needs to have nitron::Singleton<T> as a friend
template <typename ValueType>
class Singleton
{
public:
    /// - `first`  is the instance reference
    /// - `second` is mutex lock guard object for thread safey
    using AccessToken = std::pair<ValueType&, std::lock_guard<std::mutex>>;

    static_assert(ProperSingleton<ValueType>,
                  "Wrapped type is NOT suitable to be a Singleton.");

    Singleton()                            = delete;
    Singleton(Singleton const&)            = delete;
    Singleton(Singleton&&)                 = delete;
    Singleton& operator=(Singleton const&) = delete;
    Singleton& operator=(Singleton&&)      = delete;
    
    /// @brief acquiring @ref AccessToken for instance
    static AccessToken get();
};

} // namespace nitron

#include "Singleton.tpp"