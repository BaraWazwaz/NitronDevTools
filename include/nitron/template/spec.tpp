/**
 * @file spec.tpp
 * @brief Template implementation structures for custom polymorphic functional evaluation architectures.
 */
#pragma once

#include <nitron/spec.hpp>

namespace nitron {

template <typename T, typename Tested, typename Checker>
requires is_valid_TestReturnedValue<T, Tested, Checker>
TestReturnedValue<T, Tested, Checker>::TestReturnedValue(
    Tested function,
    Checker checker,
    std::string description
)
    : Test(std::move(description))
    , mFunction(std::move(function))
    , mChecker(std::move(checker))
{}

template <typename T, typename Tested, typename Checker>
requires is_valid_TestReturnedValue<T, Tested, Checker>
std::unique_ptr<Test> TestReturnedValue<T, Tested, Checker>::clone() const & {
    return std::make_unique<TestReturnedValue>(*this);
}

template <typename T, typename Tested, typename Checker>
requires is_valid_TestReturnedValue<T, Tested, Checker>
std::unique_ptr<Test> TestReturnedValue<T, Tested, Checker>::clone() && {
    return std::make_unique<TestReturnedValue>(std::move(*this));
}

template <typename T, typename Tested, typename Checker>
requires is_valid_TestReturnedValue<T, Tested, Checker>
bool TestReturnedValue<T, Tested, Checker>::check() const {
    try {
        return mChecker(mFunction());
    } catch (...) {
        return false;
    }
}

template <typename ExceptionType>
std::unique_ptr<Test> TestThrownType<ExceptionType>::clone() const & {
    return std::make_unique<TestThrownType>(*this);
}

template <typename ExceptionType>
std::unique_ptr<Test> TestThrownType<ExceptionType>::clone() && {
    return std::make_unique<TestThrownType>(std::move(*this));
}

template <typename ExceptionType>
bool TestThrownType<ExceptionType>::check() const {
    try {
        mFunction();
    } catch (const ExceptionType&) {
        return true;
    } catch (...) {
        return false;
    }
    return false;
}

template <typename ExceptionType, typename Tested, typename Checker>
requires is_valid_TestThrownValue<ExceptionType, Tested, Checker>
TestThrownValue<ExceptionType, Tested, Checker>::TestThrownValue(
    Tested function,
    Checker checker,
    std::string description
)
    : Test(std::move(description))
    , mFunction(std::move(function))
    , mChecker(std::move(checker))
{}

template <typename ExceptionType, typename Tested, typename Checker>
requires is_valid_TestThrownValue<ExceptionType, Tested, Checker>
std::unique_ptr<Test> TestThrownValue<ExceptionType, Tested, Checker>::clone() const & {
    return std::make_unique<TestThrownValue>(*this);
}

template <typename ExceptionType, typename Tested, typename Checker>
requires is_valid_TestThrownValue<ExceptionType, Tested, Checker>
std::unique_ptr<Test> TestThrownValue<ExceptionType, Tested, Checker>::clone() && {
    return std::make_unique<TestThrownValue>(std::move(*this));
}

template <typename ExceptionType, typename Tested, typename Checker>
requires is_valid_TestThrownValue<ExceptionType, Tested, Checker>
bool TestThrownValue<ExceptionType, Tested, Checker>::check() const {
    try {
        mFunction();
    } catch (const ExceptionType& exception) {
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

template <typename Tested>
requires is_valid_TestThrownNone<Tested>
TestThrownNone<Tested>::TestThrownNone(
    Tested function,
    std::string description
)
    : Test(std::move(description))
    , mFunction(std::move(function))
{}

template <typename Tested>
requires is_valid_TestThrownNone<Tested>
std::unique_ptr<Test> TestThrownNone<Tested>::clone() const & {
    return std::make_unique<TestThrownNone>(*this);
}

template <typename Tested>
requires is_valid_TestThrownNone<Tested>
std::unique_ptr<Test> TestThrownNone<Tested>::clone() && {
    return std::make_unique<TestThrownNone>(std::move(*this));
}

template <typename Tested>
requires is_valid_TestThrownNone<Tested>
bool TestThrownNone<Tested>::check() const {
    try {
        mFunction();
        return true;
    } catch (...) {
        return false;
    }
}

template <typename TestClass>
requires std::is_base_of_v<Test, std::remove_cvref_t<TestClass>>
Spec& Spec::add(TestClass&& test) {
    mTests.push_back(std::forward<TestClass>(test).clone());
    return *this;
}

} // namespace nitron
