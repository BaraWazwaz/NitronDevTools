#include <nitron/spec.hpp>

namespace nitron
{

bool Test::displayResult(std::ostream& os, std::size_t tabs) const
{
    if (verdict)
        os << std::string(tabs, '\t') << "[Passed]: { " << description << " }\n";
    else
        os << std::string(tabs, '\t') << "[Failed]: { " << description << " }\n";

    return verdict;
}

Test::Test(bool verdict, std::string&& description) :
    verdict(verdict),
    description(std::forward<std::string>(description))
{}

Spec::Spec(std::string&& title) :
    title(std::forward<std::string>(title))
{}

Spec& Spec::openSubSpec(std::string&& title) {
    children.emplace_back(std::forward<std::string>(title));
    children.back().parent = this;
    return children.back();
}

Spec& Spec::closeSubSpec() {
    return *parent;
}

Spec& Spec::addTest(Test&& test) {
    direct.emplace_back(std::forward<Test>(test));
    return *this;
}

Spec& Spec::addSubSpec(Spec&& subSpec) {
    subSpec.parent = this;
    this->children.emplace_back(std::forward<Spec>(subSpec));
    return *this;
}

bool Spec::displayResult(std::ostream& os, std::size_t tabs) const {
    bool verdict = true;
    os << std::string(tabs, '\t') << "Spec { " << title << " } :\n";
    
    for (Test const& test : direct)
        verdict = test.displayResult(os, tabs + 1) && verdict;
    
    for (Spec const& spec : children)
        verdict = spec.displayResult(os, tabs + 1) && verdict;

    if (verdict)
        os << std::string(tabs, '\t') << "Verdict: [Passed]\n";
    else
        os << std::string(tabs, '\t') << "Verdict: [Failed]\n";

    return verdict;
}

Test Test::expectedToPass() & {
    Test copy = *this;
    return copy;
}

Test Test::expectedToPass() && {
    return Test(verdict, std::move(description));
}

Test Test::expectedToFail() & {
    Test copy = *this;
    copy.verdict = !copy.verdict;
    return copy;
}

Test Test::expectedToFail() && {
    return Test(!verdict, std::move(description));
}

} // namespace nitron
