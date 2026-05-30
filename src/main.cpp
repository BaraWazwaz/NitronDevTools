#ifdef _WIN32
#include <windows.h>
#endif
#include <iostream>
#include <iomanip>
#include "index.hpp"

void runTests()
{
}

template <nitron::Number T>
T f(T x)
{
    return nitron::exp<T>(-x*x);
}

int experimenting()
{
    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    #endif
    std::cout << std::setprecision(10);
    for (long double x = -2; x <= 2; x += 0.05) {
        auto [y, d] = f(nitron::DualNumber<long double>(x, 1));
        y = std::round(y * (1ll << 20ll)) / (1ll << 20ll);
        d = std::round(d * (1ll << 20ll)) / (1ll << 20ll);
        std::cout << "x = " << std::setw(15) << x << ", ";
        std::cout << "f(x) = " << std::setw(13) << y << ", ";
        std::cout << "f'(x) = " << std::setw(13) << d << std::endl;
    }
    return 1;
}

int main()
{
    int status = experimenting();
    if (status != 0)
        return 0;
    runTests();
    return 0;
}