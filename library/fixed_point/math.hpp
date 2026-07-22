#pragma once

#include "fixed.hpp"
#include <cmath>

// ============================================================================
// pow(base, exp) — fast exponentiation (binary exponentiation / 快速幂)
//
//   base : Fixed<...>   — fixed-point value
//   exp  : int          — integer exponent (may be negative)
//
//   负指数: pow(base, -n) = Fixed(1) / pow(base, n)
// ============================================================================

template <typename BaseType, typename Store, int FractionBits>
__forceinline Fixed<BaseType, Store, FractionBits> pow(const Fixed<BaseType, Store, FractionBits>& base, int exp)
{
    using F = Fixed<BaseType, Store, FractionBits>;

    // x^0 = 1
    if (exp == 0)
        return F(1);

    // Handle negative exponent: x^(-n) = 1 / x^n
    bool flag=0;
    if (exp < 0) {
        flag=1;
        exp=-exp;
    }

    // Binary exponentiation (iterative)
    F result(1);
    F cur = base;

    while (exp > 0) {
        if (exp & 1)          // if lowest bit is 1
            result = result * cur;
        cur = cur * cur;      // square
        exp >>= 1;            // shift right
    }

    return flag?F(1)/result:result;
}

// ============================================================================
// sqrt(x, iterations) — Newton's method (Babylonian method)
//
//   x          : Fixed<...>   — value to take square root of
//   iterations : int          — maximum number of Newton iterations
//
//   要求 x >= 0，否则返回 0。
// ============================================================================

template <typename BaseType, typename Store, int FractionBits>
__forceinline Fixed<BaseType, Store, FractionBits> sqrt(const Fixed<BaseType, Store, FractionBits>& x,
                                           int iterations=32)
{
    using F = Fixed<BaseType, Store, FractionBits>;

    // x <= 0: sqrt(0) = 0; sqrt(negative) -> return 0
    if (!(x > F(0))) {
        return F(0);
    }

    // guess and (guess + x/guess) are always strictly positive throughout
    // this loop, so halving via raw_ >> 1 is exactly equivalent to a Fixed
    // division by two, but O(1) instead of going through the general
    // Fixed::operator/ (which needs a real division by guess.raw_).
    auto half = [](const F& v) -> F {
        return F::FromRaw(static_cast<Store>(v.raw() >> 1));
    };

    // Use 1 for fractional inputs so the initial guess can never truncate to zero.
    F guess = x > F(1) ? half(x) : F(1);
    F previous = F(0);

    for (int i = 0; i < iterations; ++i) {
        F next = half(guess + x / guess);
        if (next == guess)
            return next;
        // Fixed-point Newton iteration can alternate between two adjacent raw values.
        // Return the larger value to avoid underestimating game distances by one ulp.
        if (next == previous)
            return next > guess ? next : guess;
        previous = guess;
        guess = next;
    }

    return guess;
}

//abs
template <typename BaseType, typename Store, int FractionBits>
__forceinline Fixed<BaseType, Store, FractionBits> abs(const Fixed<BaseType, Store, FractionBits>& x){
    return x.abs();
}

//floor
template <typename BaseType, typename Store, int FractionBits>
__forceinline Fixed<BaseType, Store, FractionBits> floor(const Fixed<BaseType, Store, FractionBits>& x){
    return x.floor();
}

//round
template <typename BaseType, typename Store, int FractionBits>
__forceinline Fixed<BaseType, Store, FractionBits> round(const Fixed<BaseType, Store, FractionBits>& x){
    return x.round();
}

//ceil
template <typename BaseType, typename Store, int FractionBits>
__forceinline Fixed<BaseType, Store, FractionBits> ceil(const Fixed<BaseType, Store, FractionBits>& x){
    return x.ceil();
}
// ============================================================================
// atan2(y, x) — 2-argument arctangent (result in radians, range [-π, π])
//
//   Uses Taylor series with argument reduction:
//     |t| ≤ 0.5  →  direct polynomial (fast convergence)
//     0.5 < |t| ≤ 1  →  π/4 + atan((t-1)/(t+1))  (maps to |u| ≤ 1/3)
//     |t| > 1  →  π/2 - atan(1/|t|)
// ============================================================================

template <typename BaseType, typename Store, int FractionBits>
__forceinline Fixed<BaseType, Store, FractionBits> atan2(const Fixed<BaseType, Store, FractionBits>& y,
                                            const Fixed<BaseType, Store, FractionBits>& x)
{
    using F = Fixed<BaseType, Store, FractionBits>;

    static const F zero("0");
    static const F one ("1");
    static const F pi  = F("3.14159265358979323846");
    static const F half_pi  =pi/2;
    static const F quart_pi = pi/4;  // π/4
    static const F half("0.5");

    if (x == zero && y == zero)
        return zero;

    // Taylor: atan(t) = t * (c0 + t^2*(c1 + t^2*(c2 + t^2*(c3 + t^2*(c4 + t^2*(c5 + t^2*c6))))))
    // where c0=1, c1=-1/3, c2=1/5, c3=-1/7, c4=1/9, c5=-1/11, c6=1/13
    // Good for |t| ≤ 0.5 (error < 2e-6 at |t|=0.5, < 2e-9 at |t|≤1/3)
    auto atan_small = [](const F& t) -> F {
        static const F c6( "0.0769230769230769");  //  1/13
        static const F c5("-0.0909090909090909");  // -1/11
        static const F c4( "0.1111111111111111");  //  1/9
        static const F c3("-0.1428571428571429");  // -1/7
        static const F c2( "0.2");                  //  1/5
        static const F c1("-0.3333333333333333");  // -1/3
        static const F c0( "1.0");                  //  1

        F t2 = t * t;
        return t * (c0 + t2 * (c1 + t2 * (c2 + t2 * (c3 + t2 * (c4 + t2 * (c5 + t2 * c6))))));
    };

    // atan(z) for |z| ≤ 1 (well-conditioned)
    auto atan_01 = [&](const F& z) -> F {
        F abs_z = z.abs();
        if (abs_z <= half) {
            return atan_small(z);
        }
        // |z| > 0.5: use atan(z) = sign(z) * (π/4 + atan((|z|-1)/(|z|+1)))
        // where t = (|z|-1)/(|z|+1) has |t| ≤ 1/3 → fast convergence
        F t = (abs_z - one) / (abs_z + one);
        F result = quart_pi + atan_small(t);
        return (z > zero) ? result : -result;
    };

    // Full atan(z) for any z
    auto atan = [&](const F& z) -> F {
        F abs_z = z.abs();
        if (abs_z > one) {
            F result = half_pi - atan_01(one / abs_z);
            return (z > zero) ? result : -result;
        }
        return atan_01(z);
    };

    // atan2 quadrant logic
    if (x > zero) {
        return atan(y / x);
    } else if (x < zero) {
        if (y >= zero)
            return atan(y / x) + pi;
        else
            return atan(y / x) - pi;
    } else {
        return (y > zero) ? half_pi : -half_pi;
    }
}
