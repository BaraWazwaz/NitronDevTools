/**
 * @file spec.tpp
 * @brief Template implementation structures for custom polymorphic functional evaluation architectures.
 */
#pragma once

#include <nitron/spec.hpp>

namespace nitron {

template <typename T, typename Tested, typename Checker>
requires is_valid_TestReturned<T, Tested, Checker>
TestReturned<T, Tested, Checker>::TestReturned(
    Tested&& function,
    Checker&& checker,
    std::string&& description
)
    : Test(std::move(description))
    , mFunction(std::forward<Tested>(function))
    , mChecker(std::forward<Checker>(checker)) 
{}

template <typename T, typename Tested, typename Checker>
requires is_valid_TestReturned<T, Tested, Checker>
bool TestReturned<T, Tested, Checker>::check() const {
    try {
        return mChecker(mFunction());
    } catch (...) {
        return false;
    }
}

template <typename T, typename Tested, typename Checker>
requires is_valid_TestThrownWithChecker<T, Tested, Checker>
TestThrown<T, Tested, Checker>::TestThrown(
    Tested&& function,
    Checker&& checker,
    std::string&& description
)
    : Test(std::move(description))
    , mFunction(std::forward<Tested>(function))
    , mChecker(std::forward<Checker>(checker)) 
{}

template <typename T, typename Tested, typename Checker>
requires is_valid_TestThrownWithChecker<T, Tested, Checker>
bool TestThrown<T, Tested, Checker>::check() const {
    try {
        mFunction();
    } catch (const T& exception) {
        try {
            return mChecker(exception);
        } catch (...) {
            return false;
        }
    } catch (...) {
        return false;
    }
    return false;
}

template <typename T, typename Tested, typename Checker>
requires is_valid_TestThrownTypeOnly<T, Tested, Checker>
TestThrown<T, Tested, Checker>::TestThrown(
    Tested&& function,
    std::string&& description
)
    : Test(std::move(description))
    , mFunction(std::forward<Tested>(function)) 
{}

template <typename T, typename Tested, typename Checker>
requires is_valid_TestThrownTypeOnly<T, Tested, Checker>
bool TestThrown<T, Tested, Checker>::check() const {
    try {
        mFunction();
    } catch (const T&) {
        return true;
    } catch (...) {
        return false;
    }
    return false;
}

template <typename T, typename Tested, typename Checker>
requires is_valid_TestNoThrow<T, Tested, Checker>
TestThrown<T, Tested, Checker>::TestThrown(
    Tested&& function,
    std::string&& description
)
    : Test(std::move(description))
    , mFunction(std::forward<Tested>(function)) 
{}

template <typename T, typename Tested, typename Checker>
requires is_valid_TestNoThrow<T, Tested, Checker>
bool TestThrown<T, Tested, Checker>::check() const {
    try {
        mFunction();
        return true;
    } catch (...) {
        return false;
    }
}

} // namespace nitron
