#pragma once

#include <iostream>
#include "math/Number.hpp"

namespace nitron
{

template <Number T>
class DualNumber
{
public:

    using value_type = T;
    struct number_tag {};
    inline static constexpr char const* const EPSILON = "ε";

    DualNumber(T const& r = {}, T const& d = {});
    DualNumber(DualNumber&& copy)                 = default;
    DualNumber(DualNumber const& copy)            = default;
    DualNumber& operator=(DualNumber&& copy)      = default;
    DualNumber& operator=(DualNumber const& copy) = default;

    DualNumber operator+() const;
    DualNumber operator-() const;

    DualNumber operator+(DualNumber const& rhs) const;
    DualNumber operator-(DualNumber const& rhs) const;
    DualNumber operator*(DualNumber const& rhs) const;
    DualNumber operator/(DualNumber const& rhs) const;
    
    DualNumber& operator+=(DualNumber const& rhs);
    DualNumber& operator-=(DualNumber const& rhs);
    DualNumber& operator*=(DualNumber const& rhs);
    DualNumber& operator/=(DualNumber const& rhs);
    
    bool operator==(DualNumber const& rhs) const;
    bool operator!=(DualNumber const& rhs) const;
    bool operator< (DualNumber const& rhs) const;
    bool operator> (DualNumber const& rhs) const;
    bool operator<=(DualNumber const& rhs) const;
    bool operator>=(DualNumber const& rhs) const;

    T real() const;
    T dual() const;
    DualNumber conj() const;

    T real_value = {};
    T dual_value = {};
};

template <Number T>
std::ostream& operator<<(std::ostream& os, DualNumber<T> const& x);

template <Number T, typename Functor>
requires NumericalFunctor<Functor, DualNumber<T>, DualNumber<T>>
T derivative(Functor&& f, T x);

template <typename T>
struct is_dual_number : std::false_type {};

template <typename T>
struct is_dual_number<DualNumber<T>> : std::true_type {};

template <typename T>
concept IsDualNumber = is_dual_number<T>::value;

} // namespace nitron

#include "DualNumber.tpp"