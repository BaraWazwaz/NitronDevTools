/**
 * @file spec.hpp
 * @brief Implements Test-Driven-Development (TDD) testing suite.
 */
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <type_traits>
#include <utility>
#include <functional>

#include <nitron/functor.hpp>
#include <nitron/deduction.hpp>

namespace nitron {

class Spec;

/**
 * @class Test
 * @brief Abstract base class representing a generic test case with evaluation modifiers.
 */
class Test {
    friend class Spec;

public:
    /**
     * @brief Constructs a Test object.
     * @param description Test title or message.
     */
    Test(std::string description = "[No Description]");

    Test(const Test&) = default;
    Test(Test&&) noexcept = default;
    Test& operator=(const Test&) = default;
    Test& operator=(Test&&) noexcept = default;

    /**
     * @brief Virtual destructor.
     */
    virtual ~Test() = default;

    /**
     * @brief Evaluates the final outcome of the test suite validation tracking logic.
     * @return true if the processed test run matches behavioral expectations, false otherwise.
     */
    bool judge() const;

    /**
     * @brief Configures the verification criteria to flag success when the underlying execution passes.
     * @return Reference to the current modified Test instance.
     */
    Test& expectedToPass() &;

    /**
     * @brief Configures the verification criteria to flag success when the underlying execution passes.
     * @return Reference to the current modified Test instance.
     */
    Test&& expectedToPass() &&;

    /**
     * @brief Configures the verification criteria to invert evaluation outcomes (expects failure).
     * @return Reference to the current modified Test instance.
     */
    Test& expectedToFail() &;

    /**
     * @brief Configures the verification criteria to invert evaluation outcomes (expects failure).
     * @return Reference to the current modified Test instance.
     */
    Test&& expectedToFail() &&;
    
    /**
     * @brief Overloaded stream insertion operator for Test visualization.
     * @param os Output stream reference.
     * @param test Concrete Test reference to output.
     * @return Reference to the output stream.
     */
    friend std::ostream& operator<<(std::ostream& os, const Test& test);

protected:
    /**
     * @brief Core specialized execution method implemented by derived test types.
     * @return true if the structural engine criteria are met, false otherwise.
     */
    virtual bool check() const = 0;

    /**
     * @brief Creates a heap-allocated copy of this test.
     */
    virtual std::unique_ptr<Test> clone() const & = 0;

    /**
     * @brief Creates a heap-allocated move of this test.
     */
    virtual std::unique_ptr<Test> clone() && = 0;

private:
    std::string mDescription;
    bool mExpectedToPass = true;
};

/**
 * @brief Concept validating requirements for checking a returned function value.
 */
template <typename T, typename Tested, typename Checker>
concept is_valid_TestReturnedValue =
    LooseFunctor<Tested, T> &&
    LooseFunctor<Checker, bool, T>;

/**
 * @class TestReturnedValue
 * @brief Check if the function provided returns a value that passes provided checker.
 */
template <typename T, typename Tested, typename Checker>
requires is_valid_TestReturnedValue<T, Tested, Checker> 
class TestReturnedValue : public Test {
    friend class Spec;

public:
    /**
     * @brief Constructs a TestReturnedValue tracker wrapper.
     * @param function Tested function execution pipeline.
     * @param checker Function to test the return value of the tested function.
     * @param description Test title or message context.
     */
    TestReturnedValue(
        Tested function,
        Checker checker,
        std::string description = "[No Description]"
    );
    
protected:
    /**
     * @brief Executes the target function and runs the return value through the checker function.
     */
    bool check() const override;

    /**
     * @brief Creates a heap-allocated copy of this test.
     */
    std::unique_ptr<Test> clone() const & override;

    /**
     * @brief Creates a heap-allocated move of this test.
     */
    std::unique_ptr<Test> clone() && override;

private:
    Tested mFunction;
    Checker mChecker;
};

/**
 * @brief Deduction guide for TestReturnedValue
 */
template <typename Tested, typename Checker>
TestReturnedValue(Tested, Checker, std::string = "[No Description]")
    -> TestReturnedValue<
        std::invoke_result_t<Tested>,
        Tested,
        Checker
    >;

/**
 * @class TestThrownType
 * @brief Check if the function provided throws an exception of type ExceptionType.
 */
template <typename ExceptionType>
class TestThrownType : public Test {
    friend class Spec;

public:
    /**
     * @brief Constructs a TestThrownType evaluation wrapper.
     * @param function Tested function scope.
     * @param description Test title or context message details.
     */
    template <typename Tested>
    requires LooseFunctor<std::decay_t<Tested>, void>
    TestThrownType(
        Tested&& function,
        std::string description = "[No Description]"
    )
        : Test(std::move(description))
        , mFunction(std::forward<Tested>(function))
    {}

protected:
    /**
     * @brief Executes the target function and catches the thrown exception of type ExceptionType.
     */
    bool check() const override;

    /**
     * @brief Creates a heap-allocated copy of this test.
     */
    std::unique_ptr<Test> clone() const & override;

    /**
     * @brief Creates a heap-allocated move of this test.
     */
    std::unique_ptr<Test> clone() && override;

private:
    std::function<void()> mFunction;
};

/**
 * @brief Concept validating requirements for checking thrown exception with a custom checker.
 */
template <typename ExceptionType, typename Tested, typename Checker>
concept is_valid_TestThrownValue =
    LooseFunctor<Tested, void> &&
    LooseFunctor<Checker, bool, ExceptionType>;

/**
 * @class TestThrownValue
 * @brief Check if the function provided throws an exception that passes provided checker.
 */
template <typename ExceptionType, typename Tested, typename Checker>
requires is_valid_TestThrownValue<ExceptionType, Tested, Checker>
class TestThrownValue : public Test {
    friend class Spec;

public:
    /**
     * @brief Constructs a TestThrownValue tracker wrapper.
     * @param function Tested function execution pipeline.
     * @param checker Function to test the thrown exception.
     * @param description Test title or message context.
     */
    TestThrownValue(
        Tested function,
        Checker checker,
        std::string description = "[No Description]"
    );

protected:
    /**
     * @brief Executes target function, catches exception of type ExceptionType, and verifies it with checker.
     */
    bool check() const override;

    /**
     * @brief Creates a heap-allocated copy of this test.
     */
    std::unique_ptr<Test> clone() const & override;

    /**
     * @brief Creates a heap-allocated move of this test.
     */
    std::unique_ptr<Test> clone() && override;

private:
    Tested mFunction;
    Checker mChecker;
};

/**
 * @brief Deduction guide for TestThrownValue
 */
template <typename Tested, typename Checker>
TestThrownValue(Tested, Checker, std::string = "[No Description]")
    -> TestThrownValue<
        first_argument_t<Checker>,
        Tested,
        Checker
    >;

/**
 * @brief Concept validating requirements for checking no exception is thrown.
 */
template <typename Tested>
concept is_valid_TestThrownNone = LooseFunctor<Tested, void>;

/**
 * @class TestThrownNone
 * @brief Check if the function provided executes completely without throwing any exceptions.
 */
template <typename Tested>
requires is_valid_TestThrownNone<Tested>
class TestThrownNone : public Test {
    friend class Spec;

public:
    /**
     * @brief Constructs a TestThrownNone tracker wrapper.
     * @param function Tested function execution block.
     * @param description Test title or message context.
     */
    TestThrownNone(
        Tested function,
        std::string description = "[No Description]"
    );

protected:
    /**
     * @brief Executes the target function and ensures no exceptions escape.
     */
    bool check() const override;

    /**
     * @brief Creates a heap-allocated copy of this test.
     */
    std::unique_ptr<Test> clone() const & override;

    /**
     * @brief Creates a heap-allocated move of this test.
     */
    std::unique_ptr<Test> clone() && override;

private:
    Tested mFunction;
};

/**
 * @brief Deduction guide for TestThrownNone
 */
template <typename Tested>
TestThrownNone(Tested, std::string = "[No Description]")
    -> TestThrownNone<Tested>;

struct SpecStats;

/**
 * @class Spec
 * @brief Suite interface mimicking behavior models seen across testing runtime designs.
 */
class Spec {
public:
    /**
     * @brief Default constructor.
     */
    Spec() = default;

    /**
     * @brief Parameterized initialization tracking suite structures.
     * @param title Root identifier matching test environment target scopes.
     */
    Spec(std::string title);

    Spec(const Spec&) = delete;
    Spec& operator=(const Spec&) = delete;

    Spec(Spec&& other) noexcept;
    Spec& operator=(Spec&& other) noexcept;

    /**
     * @brief Accepts any concrete test rvalue, constructs it on the heap, and registers it.
     * @tparam TestClass The automatically deduced type of the test class.
     * @param test The temporary test object (optionally with chained modifiers).
     * @return Reference to the current Spec for chaining.
     */
    template <typename TestClass>
    requires std::is_base_of_v<Test, std::remove_cvref_t<TestClass>>
    Spec& add(TestClass&& test);

    /**
     * @brief Adds a new subspec to Spec suite.
     * @param subSpec Spec object to consider as a subspec.
     * @return Reference to current Spec.
     */
    Spec& add(Spec&& subSpec);
    
    /**
     * @brief Creates a nested Spec inside current Spec and enters it.
     * @param title Title of nested Spec created.
     * @return Reference to nested Spec created.
     */
    Spec& enter(std::string title);

    /**
     * @brief Finishes working on nested Spec and goes back to parent.
     * @return Reference to parent Spec.
     */
    Spec& leave();

    /**
     * @brief Output testing verdict information.
     * @param os Output stream used to print results.
     * @return True if all containing elements execute without recording failure logs, false otherwise.
     */
    bool report(std::ostream& os) const;

private:
    std::string mTitle = "[Untitled]";
    Spec* mParent = this;
    std::vector<std::unique_ptr<Test>> mTests;
    std::vector<Spec> mSubSpecs;

    /**
     * @brief Recursive implementation for generating formatted test execution reports.
     */
    bool report(std::ostream& os, std::size_t tabs, SpecStats& stats) const;
};

} // namespace nitron

#include <nitron/template/spec.tpp>
