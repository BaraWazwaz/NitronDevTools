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
    Tested function,
    Checker checker,
    std::string description
)
    : Test(std::move(description))
    , mFunction(std::move(function))
    , mChecker(std::move(checker))
{}

template <typename T, typename Tested, typename Checker>
requires is_valid_TestReturned<T, Tested, Checker>
std::unique_ptr<Test> TestReturned<T, Tested, Checker>::clone() const & {
    return std::make_unique<TestReturned>(*this);
}

template <typename T, typename Tested, typename Checker>
requires is_valid_TestReturned<T, Tested, Checker>
std::unique_ptr<Test> TestReturned<T, Tested, Checker>::clone() && {
    return std::make_unique<TestReturned>(std::move(*this));
}

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
    Tested function,
    Checker checker,
    std::string description
)
    : Test(std::move(description))
    , mFunction(std::move(function))
    , mChecker(std::move(checker))
{}

template <typename T, typename Tested, typename Checker>
requires is_valid_TestThrownWithChecker<T, Tested, Checker>
std::unique_ptr<Test> TestThrown<T, Tested, Checker>::clone() const & {
    return std::make_unique<TestThrown>(*this);
}

template <typename T, typename Tested, typename Checker>
requires is_valid_TestThrownWithChecker<T, Tested, Checker>
std::unique_ptr<Test> TestThrown<T, Tested, Checker>::clone() && {
    return std::make_unique<TestThrown>(std::move(*this));
}

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
    Tested function,
    std::string description
)
    : Test(std::move(description))
    , mFunction(std::move(function))
{}

template <typename T, typename Tested, typename Checker>
requires is_valid_TestThrownTypeOnly<T, Tested, Checker>
std::unique_ptr<Test> TestThrown<T, Tested, Checker>::clone() const & {
    return std::make_unique<TestThrown>(*this);
}

template <typename T, typename Tested, typename Checker>
requires is_valid_TestThrownTypeOnly<T, Tested, Checker>
std::unique_ptr<Test> TestThrown<T, Tested, Checker>::clone() && {
    return std::make_unique<TestThrown>(std::move(*this));
}

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
    Tested function,
    std::string description
)
    : Test(std::move(description))
    , mFunction(std::move(function))
{}

template <typename T, typename Tested, typename Checker>
requires is_valid_TestNoThrow<T, Tested, Checker>
std::unique_ptr<Test> TestThrown<T, Tested, Checker>::clone() const & {
    return std::make_unique<TestThrown>(*this);
}

template <typename T, typename Tested, typename Checker>
requires is_valid_TestNoThrow<T, Tested, Checker>
std::unique_ptr<Test> TestThrown<T, Tested, Checker>::clone() && {
    return std::make_unique<TestThrown>(std::move(*this));
}

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

template <typename TestClass>
requires std::is_base_of_v<Test, std::remove_cvref_t<TestClass>>
Spec& Spec::addTest(TestClass&& test) {
    mTests.push_back(std::forward<TestClass>(test).clone());
    return *this;
}

} // namespace nitron
