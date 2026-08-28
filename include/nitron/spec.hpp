#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <type_traits>

#include <nitron/functor.hpp>

namespace nitron {

class Test {
public:
    Test(std::string&& description = "[No Description]");
    virtual ~Test() = default;

    /**
     * @brief Evaluates the test logic to determine a pass or fail outcome.
     * @return true if the test criteria are met, false otherwise.
     */
    virtual bool judge() const = 0;

    virtual Test expectedToPass() &;
    virtual Test expectedToPass() &&;
    virtual Test expectedToFail() &;
    virtual Test expectedToFail() &&;

    inline friend std::ostream& operator<<(std::ostream& os, const Test& test);

private:
    std::string mDescription;
};

template <typename T, typename Tested, typename Checker>
concept is_valid_TestReturned =
    LooseFunctor<Tested, T> &&
    LooseFunctor<Checker, bool, T>;

/// @brief check if the function provided returns a value the passes provided checker
template <typename T, typename Tested, typename Checker>
requires is_valid_TestReturned<T, Tested, Checker> 
class TestReturned : public Test {
public:
    /// @param description test title or message
    /// @param function    tested function
    /// @param checker     function to test the return value of the tested function
    TestReturned(
        Tested&& function,
        Checker&& checker,
        std::string&& description = "[No Description]"
    );

    /**
     * @brief Executes the target function and runs the return value through the checker function.
     * @return true if the checker approves the returned value, false otherwise.
     */
    bool judge() const override;

private:
    Tested mFunction;
    Checker mChecker;
};

// Primary template for TestThrown
template <typename T, typename Tested, typename Checker>
class TestThrown;

template <typename T, typename Tested, typename Checker>
concept is_valid_TestThrownWithChecker =
    LooseFunctor<Tested, void> &&
    LooseFunctor<Checker, bool, T>;

/// @brief check if the function provided throws a value that passes provided checker
template <typename T, typename Tested, typename Checker>
requires is_valid_TestThrownWithChecker<T, Tested, Checker>
class TestThrown<T, Tested, Checker> : public Test {
public:
    /// @param function    tested function
    /// @param checker     function to test the throw value of the tested function
    /// @param description test title or message
    TestThrown(
        Tested&& function,
        Checker&& checker,
        std::string&& description = "[No Description]"
    );

    /**
     * @brief Executes the target function, catches the thrown exception of type T, and runs it through the checker.
     * @return true if an exception of type T is caught and passes the checker, false otherwise.
     */
    bool judge() const override;

private:
    Tested mFunction;
    Checker mChecker;
};

template <typename T, typename Tested, typename Checker>
concept is_valid_TestThrownTypeOnly =
    LooseFunctor<Tested, void> &&
    std::is_void_v<Checker>;

/// @brief check if the function provided throws a value of expected type
template <typename T, typename Tested, typename Checker>
requires is_valid_TestThrownTypeOnly<T, Tested, Checker>
class TestThrown<T, Tested, Checker> : public Test {
public:
    /// @param function    tested function
    /// @param description test title or message
    TestThrown(
        Tested&& function,
        std::string&& description = "[No Description]"
    );

    /**
     * @brief Executes the target function and intercepts exceptions to verify the thrown instance type.
     * @return true if an exception of type T is explicitly caught, false otherwise.
     */
    bool judge() const override;

private:
    Tested mFunction;
};

class Spec {
public:
    Spec() = default;
    Spec(std::string&& title);

    /// @brief adds a new test to Spec suite
    /// @param test Test object to add to suite
    /// @return reference to current Spec 
    Spec& addTest(Test&& test);

    /// @brief adds a new subspec to Spec suite
    /// @param subSpec Spec object to consider as a subspec
    /// @return reference to current Spec
    Spec& addSubSpec(Spec&& subSpec);
    
    /// @brief creates a nested Spec inside current Spec and enters it
    /// @param title title of nested Spec created
    /// @return reference to nested Spec created
    Spec& openSubSpec(std::string&& title);

    /// @brief finishes working on nested Spec and goes back to parent
    /// @return reference to parent Spec
    Spec& closeSubSpec();

    /// @brief output testing verdict information
    /// @param os   output scream used to print results
    inline bool displayResult(std::ostream& os) const;

private:
    std::string mTitle = "[Untitled]";
    Spec* mParent = this;
    std::vector<Test> mTests;
    std::vector<Spec> mSubSpecs;

    /// @brief output testing verdict information
    /// @param os   output scream used to print results
    /// @param tabs number of `"\t"` characters used in indentation
    bool displayResult(std::ostream& os, std::size_t tabs) const;
};

} // namespace nitron

#include <nitron/template/spec.tpp>
