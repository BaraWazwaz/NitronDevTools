/**
 * @file Spec.cpp
 * @brief Explicit out-of-line class operational framework bindings implementation source file details.
 */
#include "Spec.hpp"

namespace nitron {

Test::Test(std::string&& description)
    : mDescription(std::move(description)) 
{}

bool Test::judge() const {
    return check() == mExpectedToPass;
}

Test& Test::expectedToPass() {
    mExpectedToPass = true;
    return *this;
}

Test& Test::expectedToFail() {
    mExpectedToPass = false;
    return *this;
}

std::ostream& operator<<(std::ostream& os, const Test& test) {
    os << test.mDescription;
    return os;
}

Spec::Spec(std::string&& title)
    : mTitle(std::move(title))
    , mParent(this) 
{}

Spec& Spec::addTest(std::unique_ptr<Test>&& test) {
    if (test) {
        mTests.push_back(std::move(test));
    }
    return *this;
}

Spec& Spec::addSubSpec(Spec&& subSpec) {
    subSpec.mParent = this;
    mSubSpecs.emplace_back(std::move(subSpec));
    return *this;
}

Spec& Spec::openSubSpec(std::string&& title) {
    Spec nested(std::move(title));
    nested.mParent = this;
    mSubSpecs.emplace_back(std::move(nested));
    return mSubSpecs.back();
}

Spec& Spec::closeSubSpec() {
    return *mParent;
}

inline bool Spec::displayResult(std::ostream& os) const {
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
