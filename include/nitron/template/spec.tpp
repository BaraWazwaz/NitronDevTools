#include <nitron/spec.hpp>

namespace nitron
{

template <typename T, typename Tested>
requires LooseFunctor<Tested, void>
Test Test::throwsValueOfType(
    Tested&& function,
    std::string&& description
) {
    bool verdict = false;
    try { function(); }
    catch(const T&) { verdict = true; }
    catch(...) {}
    return Test(verdict, std::forward<std::string>(description));
}

template <typename T, typename Tested, typename Checker>
requires LooseFunctor<Tested, void> &&
         LooseFunctor<Checker, bool, T>
Test Test::throwsValue(
    Tested&& function,
    Checker&& checker,
    std::string&& description
) {
    bool verdict = false;
    try { function(); }
    catch(const T& x) { verdict = checker(x); }
    catch(...) {}
    return Test(verdict, std::forward<std::string>(description));
}

template <typename T, typename Tested, typename Checker>
requires LooseFunctor<Tested, T> &&
         LooseFunctor<Checker, bool, T>
Test Test::returnsValue(
    Tested&& function,
    Checker&& checker,
    std::string&& description
) {
    bool verdict = false;
    try { verdict = checker(function()); }
    catch(...) {}
    return Test(verdict, std::forward<std::string>(description));
}

} // namespace nitron
