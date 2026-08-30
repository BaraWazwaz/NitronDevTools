/**
 * @file spec.cpp
 * @brief Implements Test-Driven-Development (TDD) testing suite.
 */
#include <nitron/spec.hpp>

namespace nitron {

Test::Test(std::string description)
    : mDescription(std::move(description))
{}

bool Test::judge() const {
    return check() == mExpectedToPass;
}

Test& Test::expectedToPass() & {
    mExpectedToPass = true;
    return *this;
}

Test&& Test::expectedToPass() && {
    mExpectedToPass = true;
    return std::move(*this);
}

Test& Test::expectedToFail() & {
    mExpectedToPass = false;
    return *this;
}

Test&& Test::expectedToFail() && {
    mExpectedToPass = false;
    return std::move(*this);
}

std::ostream& operator<<(std::ostream& os, const Test& test) {
    os << test.mDescription;
    return os;
}

Spec::Spec(std::string title)
    : mTitle(std::move(title))
    , mParent(this)
{}

Spec::Spec(Spec&& other) noexcept
    : mTitle(std::move(other.mTitle))
    , mParent(other.mParent == &other ? this : other.mParent)
    , mTests(std::move(other.mTests))
    , mSubSpecs(std::move(other.mSubSpecs))
{
    for (auto& child : mSubSpecs) {
        child.mParent = this;
    }
}

Spec& Spec::operator=(Spec&& other) noexcept {
    if (this != &other) {
        mTitle = std::move(other.mTitle);
        mParent = (other.mParent == &other) ? this : other.mParent;
        mTests = std::move(other.mTests);
        mSubSpecs = std::move(other.mSubSpecs);
        for (auto& child : mSubSpecs) {
            child.mParent = this;
        }
    }
    return *this;
}

Spec& Spec::addSubSpec(Spec&& subSpec) {
    subSpec.mParent = this;
    mSubSpecs.push_back(std::move(subSpec));
    return *this;
}

Spec& Spec::openSubSpec(std::string title) {
    Spec nested(std::move(title));
    nested.mParent = this;
    mSubSpecs.push_back(std::move(nested));
    return mSubSpecs.back();
}

Spec& Spec::closeSubSpec() {
    return *mParent;
}

bool Spec::displayResult(std::ostream& os) const {
    return displayResult(os, 0);
}

bool Spec::displayResult(std::ostream& os, std::size_t tabs) const {
    bool baselineStatusTracker = true;
    std::string indentPadding(tabs, '\t');
    
    os << indentPadding << "Suite: " << mTitle << "\n";
    
    for (const auto& internalCaseElement : mTests) {
        bool operationalStateResult = internalCaseElement->judge();
        os << indentPadding << "  " << (operationalStateResult ? "[PASS] " : "[FAIL] ") 
           << *internalCaseElement << "\n";
        if (!operationalStateResult) {
            baselineStatusTracker = false;
        }
    }
    
    for (const auto& activeSubSuiteNode : mSubSpecs) {
        if (!activeSubSuiteNode.displayResult(os, tabs + 1)) {
            baselineStatusTracker = false;
        }
    }
    
    return baselineStatusTracker;
}

} // namespace nitron
