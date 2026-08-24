#include <iostream>
#include <nitron/spec.hpp>

int main() {
    using nitron::Spec, nitron::Test;

    Spec("Test-Driven-Development [TDD]")
    .addTest(Test::returnsValue<long long>(
        []() -> long long {
            long long n = 5;
            return n * n;
        },
        [](long long x) -> bool {
            return x == 25;
        },
        "5x5 = 25"
    ).expectedToPass())
    .openSubSpec("Nested Spec")
        .addTest(Test::throwsValueOfType<int>(
            []() -> void {
                throw std::invalid_argument("Hello");
            },
            "It should fail to throw `int`"
        ).expectedToFail())
    .closeSubSpec()
    .displayResult(std::cout);
    
    return 0;
}
