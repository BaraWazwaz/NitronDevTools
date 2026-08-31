/**
 * @file spec.cpp
 * @brief Implements Test-Driven-Development (TDD) testing suite.
 */
#include <nitron/spec.hpp>

#include <fmt/core.h>
#include <fmt/color.h>
#include <fmt/ostream.h>

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

Spec& Spec::add(Spec&& subSpec) {
    subSpec.mParent = this;
    mSubSpecs.push_back(std::move(subSpec));
    return *this;
}

Spec& Spec::enter(std::string title) {
    Spec nested(std::move(title));
    nested.mParent = this;
    mSubSpecs.push_back(std::move(nested));
    return mSubSpecs.back();
}

Spec& Spec::leave() {
    return *mParent;
}

struct SpecStats {
    std::size_t passed = 0;
    std::size_t failed = 0;
    inline std::size_t total() const { return passed + failed; }
};

bool Spec::report(std::ostream& os) const {
    SpecStats stats;
    bool success = report(os, 0, stats);

    fmt::print(os, "\n{}\n", fmt::format(fmt::fg(fmt::color::gray), "--------------------------------------------------"));
    
    auto passedStr = fmt::format(fmt::fg(fmt::color::green) | fmt::emphasis::bold, "{} passed", stats.passed);
    auto failedStr = stats.failed > 0
        ? fmt::format(fmt::fg(fmt::color::red)   | fmt::emphasis::bold, "{} failed", stats.failed)
        : fmt::format(fmt::fg(fmt::color::green) | fmt::emphasis::bold, "{} failed", stats.failed);

    fmt::print(os, "Results: {}, {} ({} total)\n", passedStr, failedStr, stats.total());

    return success;
}

bool Spec::report(std::ostream& os, std::size_t tabs, SpecStats& stats) const {
    bool overallSuccess = true;
    std::string indentPadding(tabs, '\t');
    
    fmt::print(os, "{}{}\n", indentPadding, fmt::format(fmt::fg(fmt::color::cyan) | fmt::emphasis::bold, "Suite: {}", mTitle));
    
    for (const auto& test : mTests) {
        bool result = test->judge();
        if (result) {
            stats.passed++;
        } else {
            stats.failed++;
            overallSuccess = false;
        }

        auto statusBadge = result
            ? fmt::format(fmt::fg(fmt::color::green) | fmt::emphasis::bold, "[PASS] ")
            : fmt::format(fmt::fg(fmt::color::red)   | fmt::emphasis::bold, "[FAIL] ");

        fmt::print(os, "{}  {}{}\n", indentPadding, statusBadge, fmt::streamed(*test));
    }
    
    for (const auto& subSpec : mSubSpecs) {
        if (!subSpec.report(os, tabs + 1, stats)) {
            overallSuccess = false;
        }
    }
    
    return overallSuccess;
}

} // namespace nitron
