/**
 * @file Promise.hpp
 * @brief implements the JS-like @ref Promise class
 * @copyright 2026 Bara Wazwaz. Released under the GNU License
 */
#pragma once

#include <future>           // std::future, std::async
#include <optional>         // std::optional
#include <exception>        // std::exception_ptr, std::current_exception, std::rethrow_exception
#include <mutex>            // std::mutex, std::lock_guard
#include "util/Functor.hpp" // nitron::LooseFunctor

namespace nitron
{

template <typename ResultType>
class Promise
{
public:
    enum struct Status { PENDING, SUCCEEDED, FAILED };

    /// @brief construction using async function and arguments
    template <typename Functor, typename... Args>
    requires LooseFunctor<Functor, ResultType, Args...>
    Promise(Functor&& functor,
            Args&&... args);
        
    inline Status getStatus() const;
    inline bool   pending()   const;
    inline bool   succeed()   const;
    inline bool   failed()    const;
    
    /// @brief get the value of promise result
    inline ResultType get() const;
    
    /// @brief chain a follow-up promise
    template <typename NextType, typename Functor, typename... Args>
    requires LooseFunctor<Functor, NextType, ResultType, Args...>
    Promise<NextType>
    then(Functor&& functor,
         Args&&... args) const;
    
    /// @brief chain a fallback promise
    template <typename ExceptionType, typename Functor, typename... Args>
    requires LooseFunctor<Functor, ResultType, ExceptionType, Args...>
    Promise<ResultType>
    except(Functor&& functor,
           Args&&... args) const;

private:
    /// @brief make sure the promise is fully executed
    void blockAndEvaluate() const;
    
    mutable Status status = Status::PENDING;
    mutable std::future<ResultType> future;
    mutable std::optional<ResultType> result;
    mutable std::optional<std::exception_ptr> exception;
    mutable std::mutex mutex;
};

} // namespace nitron

#include "Promise.tpp"