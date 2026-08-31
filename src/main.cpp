#include <iostream>
#include <functional>
#include <stdexcept>
#include <string>

#include <nitron/spec.hpp>

int computeSquare(int x) { return x * x; }

bool isEven(int val) { return val % 2 == 0; }

void throwInvalidArgument() { throw std::invalid_argument("Invalid argument provided"); }

void safeOperation() { }

int main() {
    using namespace nitron;

    std::cout << "Running NitronDevTools Comprehensive Spec Suite...\n\n";

    auto checkInvalidMsg = [](const std::invalid_argument& ex) {
        return std::string(ex.what()).find("Invalid") != std::string::npos;
    };

    Spec("`nitron/spec.hpp` Test Suite")
        .openSubSpec("1. Free Functions")
            .addTest(testReturned(
                []() { return computeSquare(4); },
                [](int res) { return res == 16; },
                "computeSquare(4) should return 16"
            ))
            .addTest(testReturned(
                []() { return computeSquare(3); },
                isEven,
                "computeSquare(3) expected to fail isEven check"
            ).expectedToFail())
            .addTest(testThrown<std::invalid_argument>(
                throwInvalidArgument,
                "throwInvalidArgument() should throw std::invalid_argument"
            ))
            .addTest(testThrown(
                throwInvalidArgument,
                checkInvalidMsg,
                "throwInvalidArgument() exception message contains 'Invalid'"
            ))
            .addTest(testNoThrow(
                safeOperation,
                "safeOperation() should execute without throwing"
            ))
        .closeSubSpec()

        .openSubSpec("2. std::function Objects")
            .addTest(testReturned(
                std::function<int()>([]() { return 42; }),
                std::function<bool(int)>([](int val) { return val == 42; }),
                "std::function returning 42"
            ))
            .addTest(testThrown<std::out_of_range>(
                std::function<void()>([]() { throw std::out_of_range("Out of bounds"); }),
                "std::function throwing std::out_of_range"
            ))
            .addTest(testThrown(
                std::function<void()>([]() { throw std::out_of_range("Index 5 out of range"); }),
                std::function<bool(const std::out_of_range&)>([](const std::out_of_range& e) {
                    return std::string(e.what()).find("Index 5") != std::string::npos;
                }),
                "std::function exception custom checker"
            ))
        .closeSubSpec()

        .openSubSpec("3. Lambda Functions")
            .addTest(testReturned(
                []() { int a = 3; int b = 7; return a + b; },
                [](int sum) { return sum == 10; },
                "Non-capturing lambda addition (3 + 7 = 10)"
            ))
            .addTest(testReturned(
                [multiplier = 5]() { return multiplier * 3; },
                [](int product) { return product == 15; },
                "Capturing lambda multiplication (5 * 3 = 15)"
            ))
            .addTest(testReturned(
                [offset = 100]() { return offset + 5; },
                [](int val) { return val == 200; },
                "Capturing lambda with incorrect expectation (expectedToFail)"
            ).expectedToFail())
            .addTest(testThrown<std::logic_error>(
                std::function<void()>([]() { throw std::logic_error("Logic failure"); }),
                "Throwing lambda logic_error"
            ))
        .closeSubSpec()

        .openSubSpec("4. Evaluation Modifiers (expectedToPass / expectedToFail)")
            .addTest(testReturned(
                []() { return true; },
                [](bool b) { return b; },
                "Passing test with expectedToPass"
            ).expectedToPass())
            .addTest(testReturned(
                []() { return false; },
                [](bool b) { return b; },
                "Failing test correctly marked with expectedToFail"
            ).expectedToFail())
        .closeSubSpec()

        .displayResult(std::cout);

    return 0;
}
