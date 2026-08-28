#include <nitron/spec.hpp>

namespace nitron
{

bool Test::displayResult(std::ostream& os, std::size_t tabs) const
{
    os << std::string(tabs, '\t');
    os << "[" << (this->verdict ? "Passed" : "Failed") << "]: ";
    os << "{ " << this->description << " }\n";
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
    this->children.emplace_back(std::forward<std::string>(title));
    this->children.back().parent = this;
    return this->children.back();
}

Spec& Spec::closeSubSpec() {
    return *this->parent;
}

Spec& Spec::addTest(Test&& test) {
    this->direct.emplace_back(std::forward<Test>(test));
    return *this;
}

Spec& Spec::addSubSpec(Spec&& subSpec) {
    subSpec.parent = this;
    this->children.emplace_back(std::forward<Spec>(subSpec));
    return *this;
}

bool Spec::displayResult(std::ostream& os) const {
    return this->displayResult(os, 0ull);
}

bool Spec::displayResult(std::ostream& os, std::size_t tabs) const {
    bool verdict = true;
    os << std::string(tabs, '\t');
    os << "Spec { " << title << " } :\n";

    for (const Test& test : this->direct)
        verdict = test.displayResult(os, tabs + 1) && verdict;
    
    for (const Spec& spec : this->children)
        verdict = spec.displayResult(os, tabs + 1) && verdict;

    os << std::string(tabs, '\t');
    os << "Verdict: [" << (verdict ? "Passed" : "Failed") << "]\n";

    return verdict;
}

Test Test::expectedToPass() & {
    Test copy = *this;
    return copy;
}

Test Test::expectedToPass() && {
    return Test(this->verdict, std::move(this->description));
}

Test Test::expectedToFail() & {
    Test copy = *this;
    copy.verdict = !copy.verdict;
    return copy;
}

Test Test::expectedToFail() && {
    return Test(!this->verdict, std::move(this->description));
}

} // namespace nitron
