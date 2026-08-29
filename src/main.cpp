#include <iostream>
#include <nitron/spec.hpp>

int main() {
    using namespace nitron;

    Spec("Evaluated-On-Judge TDD")
    .addTest(TestReturned(
        []() -> int { return 0; },
        [](int result) -> bool { return result == 0; },
        "Expected to return 0"
    ))
    .addTest(TestReturned(
        []() -> int { return 1; },
        [](int result) -> bool { return result == 0; },
        "Expected not to return 0"
    ).expectedToFail())
    .displayResult(std::cout);

    return 0;
}
