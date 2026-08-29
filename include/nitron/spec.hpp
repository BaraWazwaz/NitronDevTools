/**
 * @file Spec.hpp
 * @brief implements JS-jasmine-like spec suite using @ref Test and @ref Spec classes
 */
#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <type_traits>
#include <utility>

#include <nitron/functor.hpp>

namespace nitron {

/**
 * @class Test
 * @brief Abstract base class representing a generic test case with evaluation modifiers.
 */
class Test {
public:
    /**
     * @brief Constructs a Test object.
     * @param description Test title or message.
     */
    Test(std::string&& description = "[No Description]");

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
    Test& expectedToPass();

    /**
     * @brief Configures the verification criteria to invert evaluation outcomes (expects failure).
     * @return Reference to the current modified Test instance.
     */
    Test& expectedToFail();

    /**
     * @brief Overloaded stream insertion operator for Test visualization.
     * @param os Output stream reference.
     * @param test Concrete Test reference to output.
     * @return Reference to the output stream.
     */
    inline friend std::ostream& operator<<(std::ostream& os, const Test& test);

protected:
    /**
     * @brief Core specialized execution method implemented by derived test types.
     * @return true if the structural engine criteria are met, false otherwise.
     */
    virtual bool check() const = 0;

    Test(const Test&) = default;
    Test(Test&&) noexcept = default;
    Test& operator=(const Test&) = default;
    Test& operator=(Test&&) noexcept = default;

    std::string mDescription;

private:
    bool mExpectedToPass = true;
};

/**
 * @brief Concept validating requirements for checking a returned function value.
 */
template <typename T, typename Tested, typename Checker>
concept is_valid_TestReturned =
    LooseFunctor<Tested, T> &&
    LooseFunctor<Checker, bool, T>;

/**
 * @class TestReturned
 * @brief Check if the function provided returns a value that passes provided checker.
 */
template <typename T, typename Tested, typename Checker>
requires is_valid_TestReturned<T, Tested, Checker> 
class TestReturned : public Test {
public:
    /**
     * @brief Constructs a TestReturned tracker wrapper.
     * @param function Tested function execution pipeline.
     * @param checker Function to test the return value of the tested function.
     * @param description Test title or message context.
     */
    TestReturned(
        Tested&& function,
        Checker&& checker,
        std::string&& description = "[No Description]"
    );

protected:
    /**
     * @brief Executes the target function and runs the return value through the checker function.
     */
    bool check() const override;

private:
    Tested mFunction;
    Checker mChecker;
};

/**
 * @class TestThrown
 * @brief Primary unspecialized blueprint configuration for assertion verification tracking.
 */
template <typename T, typename Tested, typename Checker>
class TestThrown;

/**
 * @brief Concept validating requirements for standard exceptions tracking combined with custom validation check conditions.
 */
template <typename T, typename Tested, typename Checker>
concept is_valid_TestThrownWithChecker =
    LooseFunctor<Tested, void> &&
    LooseFunctor<Checker, bool, T> &&
    (!std::is_void_v<T>);

/**
 * @class TestThrown
 * @brief Check if the function provided throws a value that passes provided checker.
 */
template <typename T, typename Tested, typename Checker>
requires is_valid_TestThrownWithChecker<T, Tested, Checker>
class TestThrown<T, Tested, Checker> : public Test {
public:
    /**
     * @brief Constructs an exception instance evaluator target tracker wrapper.
     * @param function Tested function wrapper executed locally.
     * @param checker Function to test the throw value of the tested function.
     * @param description Test title or context message details.
     */
    TestThrown(
        Tested&& function,
        Checker&& checker,
        std::string&& description = "[No Description]"
    );

protected:
    /**
     * @brief Executes the target function, catches the thrown exception of type T, and runs it through the checker.
     */
    bool check() const override;

private:
    Tested mFunction;
    Checker mChecker;
};

/**
 * @brief Concept tracking exact type assertions where custom predicate parsing step logic isn't requested.
 */
template <typename T, typename Tested, typename Checker>
concept is_valid_TestThrownTypeOnly =
    LooseFunctor<Tested, void> &&
    std::is_void_v<Checker> &&
    (!std::is_void_v<T>);

/**
 * @class TestThrown
 * @brief Check if the function provided throws a value of expected type.
 */
template <typename T, typename Tested, typename Checker>
requires is_valid_TestThrownTypeOnly<T, Tested, Checker>
class TestThrown<T, Tested, Checker> : public Test {
public:
    /**
     * @brief Constructs a clean instance framework capturing structural exception matching profiles.
     * @param function Tested function execution scope element.
     * @param description Test title or context statement identifier profile.
     */
    TestThrown(
        Tested&& function,
        std::string&& description = "[No Description]"
    );

protected:
    /**
     * @brief Executes the target function and intercepts exceptions to verify the thrown instance type.
     */
    bool check() const override;

private:
    Tested mFunction;
};

/**
 * @brief Concept validating scenario expectations where checking void type guarantees target executes without exceptions.
 */
template <typename T, typename Tested, typename Checker>
concept is_valid_TestNoThrow =
    std::is_void_v<T> &&
    LooseFunctor<Tested, void> &&
    std::is_void_v<Checker>;

/**
 * @class TestThrown
 * @brief Explicit specialization validating that an execution block executes completely cleanly without throwing errors.
 */
template <typename T, typename Tested, typename Checker>
requires is_valid_TestNoThrow<T, Tested, Checker>
class TestThrown<T, Tested, Checker> : public Test {
public:
    /**
     * @brief Constructs a No-Throw validation runtime engine assertion track proxy state instance block.
     * @param function Tested function executable block targeted context.
     * @param description Test title or context message logging data string.
     */
    TestThrown(
        Tested&& function,
        std::string&& description = "[No Description]"
    );

protected:
    /**
     * @brief Executes the target function and tracks runtime path status properties to verify that no exceptions escape.
     */
    bool check() const override;

private:
    Tested mFunction;
};

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
     * @brief Parameterized initialization assignment tracking tracking suite structures.
     * @param title Root identifier matching test environment target scopes.
     */
    Spec(std::string&& title);

    /**
     * @brief Accepts any concrete test rvalue, constructs it on the heap, and registers it.
     * @tparam ConcreteTest The automatically deduced type of the test class.
     * @param test The temporary test object (optionally with chained modifiers).
     * @return Reference to the current Spec for chaining.
     */
    Spec& addTest(Test&& test);

    /**
     * @brief Adds a new subspec to Spec suite.
     * @param subSpec Spec object to consider as a subspec.
     * @return Reference to current Spec.
     */
    Spec& addSubSpec(Spec&& subSpec);
    
    /**
     * @brief Creates a nested Spec inside current Spec and enters it.
     * @param title Title of nested Spec created.
     * @return Reference to nested Spec created.
     */
    Spec& openSubSpec(std::string&& title);

    /**
     * @brief Finishes working on nested Spec and goes back to parent.
     * @return Reference to parent Spec framework instance node locator mapping context.
     */
    Spec& closeSubSpec();

    /**
     * @brief Output testing verdict information.
     * @param os Output stream used to print results.
     * @return True if all containing elements execute without recording failure logs, false otherwise.
     */
    inline bool displayResult(std::ostream& os) const;

private:
    std::string mTitle = "[Untitled]";
    Spec* mParent = this;
    std::vector<std::unique_ptr<Test>> mTests;
    std::vector<Spec> mSubSpecs;

    /**
     * @brief Implementation variant parsing step processing operations printing diagnostics output to targeted streams.
     */
    bool displayResult(std::ostream& os, std::size_t tabs) const;
};

} // namespace nitron

#include <nitron/template/spec.tpp>
