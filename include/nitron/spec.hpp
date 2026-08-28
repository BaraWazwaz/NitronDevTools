/**
 * @file Spec.hpp
 * @brief implements JS-jasmine-like spec suite using @ref Test and @ref Spec classes
 */
#pragma once

#include <iostream>
#include <string>
#include <vector>

#include <nitron/functor.hpp>

namespace nitron
{

class Test
{
public:
    /// @brief check if the function provided throws a value of expected type
    /// @param function   tested function
    /// @param desciption test title or message
    template <typename T, typename FunctorTested>
    requires LooseFunctor<FunctorTested, void>
    static Test throwsValueOfType(FunctorTested&& function,
                                  std::string&& description = "[No Description]");

    /// @brief check if the function provided throws a value that passes provided checker
    /// @param function   tested function
    /// @param checker    function to test the throw value of the tested function
    /// @param desciption test title or message
    template <typename T, typename FunctorTested, typename FunctorChecker>
    requires LooseFunctor<FunctorTested, void> &&
             LooseFunctor<FunctorChecker, bool, T>
    static Test throwsValue(FunctorTested&& function,
                            FunctorChecker&& checker,
                            std::string&& description = "[No Description]");

    /// @brief check if the function provided returns a value the passes provided checker
    /// @param function   tested function
    /// @param checker    function to test the return value of the tested function
    /// @param desciption test title or message
    template <typename T, typename FunctorTested, typename FunctorChecker>
    requires LooseFunctor<FunctorTested, T> &&
             LooseFunctor<FunctorChecker, bool, T>
    static Test returnsValue(FunctorTested&& function,
                             FunctorChecker&& checker,
                             std::string&& description = "[No Description]");

    /// @brief output testing verdict information
    /// @param os   output scream used to print results
    /// @param tabs number of `"\t"` characters used in indentation
    bool displayResult(std::ostream& os, std::size_t tabs = 0) const;

    Test expectedToPass() &;
    Test expectedToPass() &&;
    Test expectedToFail() &;
    Test expectedToFail() &&;

private:
    bool verdict;
    std::string description = "[No Description]";

    Test(bool verdict, std::string&& description);
};

class Spec
{
public:
    Spec() = default;
    Spec(std::string&& title);

    /// @brief adds a new test to Spec suite
    /// @param test Test object to add to suite
    /// @return reference to current Spec 
    Spec& addTest(Test&& test);

    /// @brief adds a new subspec to Spec suite
    /// @param spec Spec object to consider as a subspec
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
    /// @param tabs number of `"\t"` characters used in indentation
    bool displayResult(std::ostream& os, std::size_t tabs = 0) const;

private:
    std::string title = "[Untitled]";
    Spec* parent      = this;
    std::vector<Test> direct;
    std::vector<Spec> children;
};

} // namespace nitron

#include <nitron/template/spec.tpp>
