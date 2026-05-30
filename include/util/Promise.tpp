#pragma once

#include <chrono>           // std::chrono::seconds
#include <memory>           // std::make_shared
#include "util/Promise.hpp"

namespace nitron
{

template <typename ResultType>
template <typename Functor, typename... Args>
requires LooseFunctor<Functor, ResultType, Args...>
Promise<ResultType>::Promise(Functor&& functor, Args&&... args) :
    future(std::async(std::launch::async,
                      std::forward<Functor>(functor), 
                      std::forward<Args>(args)...))
{}

template <typename ResultType>
Promise<ResultType>::Status Promise<ResultType>::getStatus() const
{
    if (status == Status::PENDING &&
        future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
    {
        blockAndEvaluate();
    }
    return status;
}

template <typename ResultType>
ResultType Promise<ResultType>::get() const
{
    if (status == Status::PENDING)
    {
        blockAndEvaluate();
    }
    if (status == Status::FAILED)
    {
        std::rethrow_exception(exception.value());
    }
    return result.value();
}

template <typename ResultType>
bool Promise<ResultType>::pending() const
{
    return getStatus() == Status::PENDING;
}

template <typename ResultType>
bool Promise<ResultType>::succeed() const
{
    return getStatus() == Status::SUCCEEDED;
}

template <typename ResultType>
bool Promise<ResultType>::failed() const
{
    return getStatus() == Status::FAILED;
}

template <typename ResultType>
template <typename NextType, typename Functor, typename... Args>
requires LooseFunctor<Functor, NextType, ResultType, Args...>
Promise<NextType>
Promise<ResultType>::then(Functor&& functor,
                          Args&&... args) const
{
    return Promise<NextType>(
        [parent = std::make_shared<Promise<ResultType>>(*this),
         functorCaptured = std::forward<Functor>(functor),
         ...argsCaptured = std::forward<Args>(args)]() mutable -> NextType
        {
            return functorCaptured(parent->get(),
                                   std::forward<Args>(argsCaptured)...);
        }
    );
}

template <typename ResultType>
template <typename ExceptionType, typename Functor, typename... Args>
requires LooseFunctor<Functor, ResultType, ExceptionType, Args...>
Promise<ResultType>
Promise<ResultType>::except(Functor&& functor,
                            Args&&... args) const
{
    return Promise<ResultType>(
        [parent = std::make_shared<Promise<ResultType>>(*this),
         functorCaptured = std::forward<Functor>(functor),
         ...argsCaptured = std::forward<Args>(args)]() mutable -> ResultType
        {
            try
            {
                return parent->get();
            }
            catch (ExceptionType const& exception)
            {
                return functorCaptured(std::forward<ExceptionType>(exception),
                                       std::forward<Args>(argsCaptured)...);
            }
            catch (...)
            {
                throw;
            }
        }
    );
}

template <typename ResultType>
void Promise<ResultType>::blockAndEvaluate() const
{
    std::lock_guard<std::mutex> lock (mutex);
    if (status != Status::PENDING)
    {
        return;
    }
    try
    {
        result = future.get();
        status = Status::SUCCEEDED;
    }
    catch(...)
    {
        exception = std::current_exception();
        status = Status::FAILED;
    }
}

} // namespace nitron