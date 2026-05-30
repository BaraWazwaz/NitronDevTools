#include <stdexcept>
#include "math/DualNumber.hpp"

namespace nitron
{

template <Number T>
DualNumber<T>::DualNumber(T const& r, T const& d) :
    real_value(r),
    dual_value(d)
{}

template <Number T>
DualNumber<T> DualNumber<T>::operator+() const
{
    return *this;
}

template <Number T>
DualNumber<T> DualNumber<T>::operator-() const
{
    return DualNumber<T>(-real_value, -dual_value);
}

template <Number T>
DualNumber<T> DualNumber<T>::operator+(DualNumber<T> const& rhs) const
{
    return DualNumber<T>(real_value + rhs.real_value, dual_value + rhs.dual_value);
}

template <Number T>
DualNumber<T> DualNumber<T>::operator-(DualNumber<T> const& rhs) const
{
    return DualNumber<T>(real_value - rhs.real_value, dual_value - rhs.dual_value);
}

template <Number T>
DualNumber<T> DualNumber<T>::operator*(DualNumber<T> const& rhs) const
{
    return DualNumber<T>(real_value * rhs.real_value,
                         dual_value * rhs.real_value + real_value * rhs.dual_value);
}

template <Number T>
DualNumber<T> DualNumber<T>::operator/(DualNumber<T> const& rhs) const
{
    if (rhs.real_value == T{0})
        throw std::invalid_argument("cannot divide by a nitron::DualNumber<T> with a real part that's a zero");
    return DualNumber<T>(real_value / rhs.real_value,
                         (dual_value * rhs.real_value - real_value * rhs.dual_value) / (rhs.real_value * rhs.real_value));
}

template <Number T>
DualNumber<T>& DualNumber<T>::operator+=(DualNumber<T> const& rhs)
{
    return *this = *this + rhs;
}

template <Number T>
DualNumber<T>& DualNumber<T>::operator-=(DualNumber<T> const& rhs)
{
    return *this = *this - rhs;
}

template <Number T>
DualNumber<T>& DualNumber<T>::operator*=(DualNumber<T> const& rhs)
{
    return *this = *this * rhs;
}

template <Number T>
DualNumber<T>& DualNumber<T>::operator/=(DualNumber<T> const& rhs)
{
    return *this = *this / rhs;
}

template <Number T>
bool DualNumber<T>::operator==(DualNumber<T> const& rhs) const
{
    return real_value == rhs.real_value && dual_value == rhs.dual_value;
}

template <Number T>
bool DualNumber<T>::operator!=(DualNumber<T> const& rhs) const
{
    return real_value != rhs.real_value || dual_value != rhs.dual_value;
}

template <Number T>
bool DualNumber<T>::operator<(DualNumber<T> const& rhs) const
{
    return real_value < rhs.real_value || real_value == rhs.real_value && dual_value < rhs.dual_value;
}

template <Number T>
bool DualNumber<T>::operator>(DualNumber<T> const& rhs) const
{
    return real_value > rhs.real_value || real_value == rhs.real_value && dual_value > rhs.dual_value;
}

template <Number T>
bool DualNumber<T>::operator<=(DualNumber<T> const& rhs) const
{
    return real_value < rhs.real_value || real_value == rhs.real_value && dual_value <= rhs.dual_value;
}

template <Number T>
bool DualNumber<T>::operator>=(DualNumber<T> const& rhs) const
{
    return real_value > rhs.real_value || real_value == rhs.real_value && dual_value >= rhs.dual_value;
}

template <Number T>
T DualNumber<T>::real() const
{
    return real_value;
}

template <Number T>
T DualNumber<T>::dual() const
{
    return dual_value;
}

template <Number T>
DualNumber<T> DualNumber<T>::conj() const
{
    return DualNumber<T>(real_value, -dual_value);
}

template <Number T>
std::ostream& operator<<(std::ostream& os, DualNumber<T> const& x)
{
    if (x.dual_value >= 0)
        return os << x.real_value << "+" << DualNumber<T>::EPSILON << abs(x.dual_value);
    else
        return os << x.real_value << "-" << DualNumber<T>::EPSILON << abs(x.dual_value);
}

template <Number T, typename Functor>
requires NumericalFunctor<Functor, DualNumber<T>, DualNumber<T>>
T derivative(Functor&& f, T x)
{
    return f(DualNumber<T>(x, 1)).dual_value;
}

template <typename T>
requires IsDualNumber<T>
T sgn(T const& x)
{
    return sgn(x.real_value);
}

template <typename T>
requires IsDualNumber<T>
T abs(T const& x)
{
    T s = sgn(x);
    if (s == 0)
        throw std::invalid_argument("cannot calculate nitron::abs() of a nitron::DualNumber<T> with a zero real part");
    return x * T(s);
}

template <typename T>
requires IsDualNumber<T>
T pow(T const& x, T const& exponent)
{
    auto const& [a, b] = x;
    auto const& [c, d] = exponent;
    const auto pow_a_c_1 = pow(a, c - 1);
    return T(a * pow_a_c_1,
             pow_a_c_1 * b * c + a * pow_a_c_1 * log(a) * d);
}

template <typename T>
requires IsDualNumber<T>
T exp(T const& x)
{
    auto const& [a, b] = x;
    auto exp_a = exp(a);
    return T(exp_a, exp_a * b);
}

template <typename T>
requires IsDualNumber<T>
T log(T const& x, T const& base)
{
    auto const& [a, b] = x;
    auto const& [c, d] = base;
    auto ln_a = log(a);
    auto ln_c = log(c);
    return T(ln_a / ln_c,
             (b * c * ln_c - a * d * ln_a) / (a * c * ln_c * ln_c));
}

template <typename T>
requires IsDualNumber<T>
T log(T const& x)
{
    return log(x, exp(T(1)));
}

template <typename T>
requires IsDualNumber<T>
T sqrt(T const& x)
{
    auto const& [a, b] = x;
    const auto s = sqrt(a);
    return T(s, b / (T{2} * s));
}

template <typename T>
requires IsDualNumber<T>
T sin(T const& x)
{
    auto const& [a, b] = x;
    return T(sin(a), b * cos(a));
}

template <typename T>
requires IsDualNumber<T>
T cos(T const& x)
{
    auto const& [a, b] = x;
    return T(cos(a), - b * sin(a));
}


template <typename T>
requires IsDualNumber<T>
T tan(T const& x)
{
    auto const& [a, b] = x;
    const auto sec_a = sec(a);
    return T(tan(a), b * sec_a * sec_a);
}

template <typename T>
requires IsDualNumber<T>
T cot(T const& x)
{
    auto const& [a, b] = x;
    const auto csc_a = csc(a);
    return T(cot(a), - b * csc_a * csc_a);
}

template <typename T>
requires IsDualNumber<T>
T sec(T const& x)
{
    auto const& [a, b] = x;
    const auto sec_a = sec(a);
    return T(sec_a, b * sec_a * tan(a));
}

template <typename T>
requires IsDualNumber<T>
T csc(T const& x)
{
    auto const& [a, b] = x;
    const auto csc_a = csc(a);
    return T(csc_a, - b * csc_a * cot(a));
}

template <typename T>
requires IsDualNumber<T>
T asin(T const& x)
{
    auto const& [a, b] = x;
    return T(asin(a), b / sqrt(1 - a * a));
}

template <typename T>
requires IsDualNumber<T>
T acos(T const& x)
{
    auto const& [a, b] = x;
    return T(acos(a), - b / sqrt(1 - a * a));
}

template <typename T>
requires IsDualNumber<T>
T atan(T const& x)
{
    auto const& [a, b] = x;
    return T(atan(a), b / (1 + a * a));
}

template <typename T>
requires IsDualNumber<T>
T acot(T const& x)
{
    auto const& [a, b] = x;
    return T(acot(a), - b / (1 + a * a));
}

template <typename T>
requires IsDualNumber<T>
T asec(T const& x)
{
    auto const& [a, b] = x;
    return T(asec(a), b / (abs(a) * sqrt(a * a - 1)));
}

template <typename T>
requires IsDualNumber<T>
T acsc(T const& x)
{
    auto const& [a, b] = x;
    return T(acsc(a), - b / (abs(a) * sqrt(a * a - 1)));
}

template <typename T>
requires IsDualNumber<T>
T sinh(T const& x)
{
    auto const& [a, b] = x;
    return T(sinh(a), b * cosh(a));
}

template <typename T>
requires IsDualNumber<T>
T cosh(T const& x)
{
    auto const& [a, b] = x;
    return T(cosh(a), b * sinh(a));
}

template <typename T>
requires IsDualNumber<T>
T tanh(T const& x)
{
    auto const& [a, b] = x;
    const auto sech_a = sech(a);
    return T(tanh(a), b * sech_a * sech_a);
}

template <typename T>
requires IsDualNumber<T>
T coth(T const& x)
{
    auto const& [a, b] = x;
    const auto csch_a = csch(a);
    return T(coth(a), - b * csch_a * csch_a);
}

template <typename T>
requires IsDualNumber<T>
T sech(T const& x)
{
    auto const& [a, b] = x;
    const auto sech_a = sech(a);
    return T(sech_a, - b * sech_a * tanh(a));
}

template <typename T>
requires IsDualNumber<T>
T csch(T const& x)
{
    auto const& [a, b] = x;
    const auto csch_a = csch(a);
    return T(csch_a, - b * csch_a * coth(a));
}

template <typename T>
requires IsDualNumber<T>
T asinh(T const& x)
{
    auto const& [a, b] = x;
    return T(asinh(a), b / sqrt(a * a + 1));
}

template <typename T>
requires IsDualNumber<T>
T acosh(T const& x)
{
    auto const& [a, b] = x;
    return T(acosh(a), b / sqrt(a * a - 1));
}

template <typename T>
requires IsDualNumber<T>
T atanh(T const& x)
{
    auto const& [a, b] = x;
    return T(atanh(a), b / (1 - a * a));
}

template <typename T>
requires IsDualNumber<T>
T acoth(T const& x)
{
    auto const& [a, b] = x;
    return T(acoth(a), b / (1 - a * a));
}

template <typename T>
requires IsDualNumber<T>
T asech(T const& x)
{
    auto const& [a, b] = x;
    return T(asech(a), - b / (a * sqrt(1 - a * a)));
}

template <typename T>
requires IsDualNumber<T>
T acsch(T const& x)
{
    auto const& [a, b] = x;
    return T(acsch(a), - b / (abs(a) * sqrt(1 + a * a)));
}

} // namespace nitron