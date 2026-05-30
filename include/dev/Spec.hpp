/**
 * @file Spec.hpp
 * @brief implements JS-jasmine-like spec suite using @ref Test and @ref Spec classes
 * @copyright 2026 Bara Wazwaz. Released under the [GNU License](/.github/LICENSE)
 */
#pragma once

#include <iostream>         // std::ostream
#include <string>           // std::string
#include <vector>           // std::vector
#include "util/Functor.hpp" // nitron::LooseFunctor

namespace nitron
{

class Test
{
public:
    /// @brief check if the function provided throws a value of expected type
    template <typename T, typename FunctorTested>
    requires LooseFunctor<FunctorTested, void>
    static Test throwsValueOfType(FunctorTested&& function,
                                  std::string&& description = "[No Description]");

    /// @brief check if the function provided throws a value that passes provided checker
    template <typename T, typename FunctorTested, typename FunctorChecker>
    requires LooseFunctor<FunctorTested, void> &&
             LooseFunctor<FunctorChecker, bool, T>
    static Test throwsValue(FunctorTested&& function,
                            FunctorChecker&& checker,
                            std::string&& description = "[No Description]");

    /// @brief check if the function provided returns a value the passes provided checker
    template <typename T, typename FunctorTested, typename FunctorChecker>
    requires LooseFunctor<FunctorTested, T> &&
             LooseFunctor<FunctorChecker, bool, T>
    static Test returnsValue(FunctorTested&& function,
                             FunctorChecker&& checker,
                             std::string&& description = "[No Description]");

    /// @brief output testing verdict information
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

    Spec& closeSubSpec();
    Spec& openSubSpec(std::string&& title);
    Spec& addTest(Test&& test);

    /// @brief output testing verdict information
    /// @param tabs number of `"\t"` characters used in indentation
    bool displayResult(std::ostream& os, std::size_t tabs = 0) const;

private:
    std::string title = "Untitled";
    Spec* parent = this;
    std::vector<Test> direct;
    std::vector<Spec> children;
};

} // namespace nitron

#include "Spec.tpp"