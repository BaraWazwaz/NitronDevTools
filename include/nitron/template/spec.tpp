#include <nitron/spec.hpp>

namespace nitron
{

template <typename T, typename FunctorTested>
requires LooseFunctor<FunctorTested, void>
Test Test::throwsValueOfType(FunctorTested&& function,
                             std::string&& description)
{
    bool verdict = false;
    try { function(); }
    catch(const T&) { verdict = true; }
    catch(...) {}
    return Test(verdict, std::forward<std::string>(description));
}

template <typename T, typename FunctorTested, typename FunctorChecker>
requires LooseFunctor<FunctorTested, void> &&
         LooseFunctor<FunctorChecker, bool, T>
Test Test::throwsValue(FunctorTested&& function,
                       FunctorChecker&& checker,
                       std::string&& description)
{
    bool verdict = false;
    try { function(); }
    catch(const T& x) { verdict = checker(x); }
    catch(...) {}
    return Test(verdict, std::forward<std::string>(description));
}

template <typename T, typename FunctorTested, typename FunctorChecker>
requires LooseFunctor<FunctorTested, T> &&
         LooseFunctor<FunctorChecker, bool, T>
Test Test::returnsValue(FunctorTested&& function,
                        FunctorChecker&& checker,
                        std::string&& description)
{
    bool verdict = false;
    try { verdict = checker(function()); }
    catch(...) {}
    return Test(verdict, std::forward<std::string>(description));
}

} // namespace nitron
