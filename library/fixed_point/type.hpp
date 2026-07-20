#pragma once

#include <cstdint>
#include <climits>
#include <ostream>
#include <utility>
#include <type_traits>
#include "macro.h"

// ============================================================================
// Int128 — portable 128-bit signed integer (two's complement)
//
// Usable as BaseType / Store in Fixed<>.
// ============================================================================
#if (defined(__GNUC__) || defined(__clang__))&&defined(MMZZ_X64)
using Int128 = __int128;
#else
// MSVC (or any other compiler without a native 128-bit integer type).
//
// Two sub-cases, selected purely by whether the target is a 64-bit x64
// build that has the hardware-backed _umul128 / _udiv128 intrinsics
// (declared in <intrin.h>, x64-only):
//   - 64-bit (x64) MSVC: MMZZ_INT128_HAS_X64_INTRIN = 1. Multiply and the
//     "divisor fits in 64 bits" division fast path each become a single
//     native 64-bit-wide hardware instruction (MUL / DIV) via intrinsics.
//   - 32-bit MSVC (or ARM/ARM64 MSVC, or any compiler that reaches this
//     branch without x64 intrinsics): MMZZ_INT128_HAS_X64_INTRIN = 0. Both
//     multiply and the division fast path fall back to code built only
//     from native 32-bit/64-bit arithmetic (32-bit limb multiply, and a
//     Hacker's-Delight "divlu" digit-division) — no 128-bit hardware
//     capability is assumed, so this path works on any 32-bit CPU, but it
//     is still O(1)/O(few) rather than the O(128) bit-by-bit loop kept
//     further below as the final, fully general fallback.
#if defined(_MSC_VER) && defined(_M_X64)
#include <intrin.h>
#define MMZZ_INT128_HAS_X64_INTRIN 1
#else
#define MMZZ_INT128_HAS_X64_INTRIN 0
#endif

struct Int128 {
    uint64_t lo;
    int64_t  hi;

    // ---- constructors ----

    __forceinline  constexpr  Int128() : lo(0), hi(0) {}
    __forceinline  constexpr Int128(uint16_t v) : lo(v), hi(0) {}
    __forceinline  constexpr Int128(uint32_t v) : lo(v), hi(0) {}
    __forceinline  constexpr Int128(uint64_t v) : lo(v), hi(0) {}
   
    __forceinline constexpr  Int128(int32_t v) : lo(static_cast<uint64_t>(static_cast<int64_t>(v))), hi(v < 0 ? -1 : 0) {}
    __forceinline constexpr  Int128(int64_t v) : lo(static_cast<uint64_t>(v)), hi(v < 0 ? -1 : 0) {}
    __forceinline constexpr  Int128(int64_t hi_, uint64_t lo_) : lo(lo_), hi(hi_) {}




    // ---- unary ----

    __forceinline   Int128 operator+() const { return *this; }

    __forceinline    Int128 operator-() const {
        // -x = ~x + 1
        uint64_t neg_lo = ~lo + 1;
        int64_t  neg_hi = ~hi + (neg_lo == 0 ? 1 : 0);
        return Int128(neg_hi, neg_lo);
    }

    __forceinline   Int128 operator~() const { return Int128(~hi, ~lo); }

    // ---- comparison ----

    __forceinline bool operator==(const Int128& rhs) const { return hi == rhs.hi && lo == rhs.lo; }
    __forceinline  bool operator< (const Int128& rhs) const {
        if (hi != rhs.hi) return hi < rhs.hi;
        return lo < rhs.lo;
    }
    __forceinline bool operator> (const Int128& rhs) const { return rhs < *this; }
    __forceinline bool operator<=(const Int128& rhs) const { return !(*this > rhs); }
    __forceinline bool operator>=(const Int128& rhs) const { return !(*this < rhs); }
    __forceinline bool operator!=(const Int128& rhs) const { return !(*this == rhs); }

    // ---- addition ----

    __forceinline    Int128 operator+(const Int128& rhs) const {
        uint64_t sum_lo = lo + rhs.lo;
        int64_t  carry  = (sum_lo < lo) ? 1 : 0;
        int64_t  sum_hi = hi + rhs.hi + carry;
        return Int128(sum_hi, sum_lo);
    }

    // ---- subtraction ----

    __forceinline   Int128 operator-(const Int128& rhs) const {
        uint64_t diff_lo = lo - rhs.lo;
        int64_t  borrow  = (diff_lo > lo) ? 1 : 0;
        int64_t  diff_hi = hi - rhs.hi - borrow;
        return Int128(diff_hi, diff_lo);
    }

    // ---- multiplication (128 × 128 → low 128 bits) ----

    __forceinline   Int128 operator*(const Int128& rhs) const {
#if MMZZ_INT128_HAS_X64_INTRIN
        // 64-bit (x64) build: the low*low term is a single native 64×64→128
        // hardware MUL instruction via _umul128, instead of the four 32-bit
        // limb multiplies the 32-bit fallback below needs.
        uint64_t res_hi;
        uint64_t res_lo = _umul128(lo, rhs.lo, &res_hi);
#else
        // 32-bit (or other non-x64) build: no 64×64→128 hardware multiply is
        // available/assumed, so decompose into 32-bit limbs instead — every
        // multiply here is a plain 32×32→64 op that any 32-bit CPU already
        // has native hardware for.
        // Decompose into 32-bit limbs: a = a1<<32 | a0,  b = b1<<32 | b0
        uint64_t a0 = lo & 0xFFFFFFFF;
        uint64_t a1 = lo >> 32;
        uint64_t b0 = rhs.lo & 0xFFFFFFFF;
        uint64_t b1 = rhs.lo >> 32;

        uint64_t p00 = a0 * b0;
        uint64_t p01 = a0 * b1;
        uint64_t p10 = a1 * b0;
        uint64_t p11 = a1 * b1;

        // low 128 bits of the product of the low 64 bits
        uint64_t mid   = (p00 >> 32) + (p01 & 0xFFFFFFFF) + (p10 & 0xFFFFFFFF);
        uint64_t res_lo = (mid << 32) | (p00 & 0xFFFFFFFF);
        uint64_t res_hi = p11 + (p01 >> 32) + (p10 >> 32) + (mid >> 32);
#endif

        // Add cross-terms: a.lo * rhs.hi  +  a.hi * rhs.lo
        int64_t a_hi = hi;
        int64_t b_hi = rhs.hi;

        // a.lo * rhs.hi  (signed * unsigned → need to handle sign)
        if (b_hi != 0) {
            // This is a bit tricky: lo is unsigned, b_hi is signed
            // We can convert to uint64_t for the multiply
            uint64_t abs_b_hi = static_cast<uint64_t>(b_hi < 0 ? -b_hi : b_hi);
            uint64_t prod_lo = lo * abs_b_hi;
            if (b_hi > 0) {
                res_hi += prod_lo;
            } else {
                res_hi -= prod_lo;
            }
        }

        if (a_hi != 0) {
            uint64_t abs_a_hi = static_cast<uint64_t>(a_hi < 0 ? -a_hi : a_hi);
            uint64_t prod_lo = rhs.lo * abs_a_hi;
            if (a_hi > 0) {
                res_hi += prod_lo;
            } else {
                res_hi -= prod_lo;
            }
        }

        // a.hi * b.hi  → contributes to bits 128..255, only affects low 128 via truncation
        // For a full 128-bit product, we ignore anything beyond bit 127.
        // But we do need the sign contribution from a.hi * b.hi when negative.
        // The cross terms already handle the sign correctly for the low 128 bits.

        return Int128(static_cast<int64_t>(res_hi), res_lo);
    }

    // ---- division ----

    __forceinline    Int128 operator/(const Int128& rhs) const {
        return divmod(rhs).first;
    }

    __forceinline   Int128 operator%(const Int128& rhs) const {
        return divmod(rhs).second;
    }

    // ---- shift ----

    __forceinline   constexpr Int128 operator<<(int n) const {
        if (n <= 0) return *this;
        if (n >= 128) return Int128(0, 0);
        if (n >= 64) return Int128(static_cast<int64_t>(lo << (n - 64)), 0);
        return Int128((hi << n) | static_cast<int64_t>(lo >> (64 - n)), lo << n);
    }

    __forceinline  constexpr Int128 operator>>(int n) const {
        if (n <= 0) return *this;
        if (n >= 128) return Int128(hi < 0 ? -1 : 0, hi < 0 ? ~uint64_t(0) : 0);
        if (n >= 64) return Int128(hi < 0 ? -1 : 0, static_cast<uint64_t>(hi) >> (n - 64));
        uint64_t shift_lo = (lo >> n) | (static_cast<uint64_t>(hi) << (64 - n));
        return Int128(hi >> n, shift_lo);
    }

    // ---- fast power-of-2 division (avoids the O(128) long-division loop) ----

    // Signed division by 2^n, truncating toward zero (same rounding as
    // operator/), but O(1) instead of the general long-division loop used
    // by operator/. Only valid for n in [0, 127]; callers only ever pass
    // FractionBits which is already range-checked at compile time.
    __forceinline Int128 divpow2(int n) const {
        if (n <= 0) return *this;
        if (is_neg()) {
            Int128 neg = -(*this);
            return -(neg >> n);
        }
        return (*this) >> n;
    }

    // ---- compound assignment ----

    __forceinline  Int128& operator+=(const Int128& rhs) { *this = *this + rhs; return *this; }
    __forceinline  Int128& operator-=(const Int128& rhs) { *this = *this - rhs; return *this; }
    __forceinline  Int128& operator*=(const Int128& rhs) { *this = *this * rhs; return *this; }
    __forceinline  Int128& operator/=(const Int128& rhs) { *this = *this / rhs; return *this; }
    __forceinline  Int128& operator%=(const Int128& rhs) { *this = *this % rhs; return *this; }
    __forceinline  Int128& operator<<=(int n) { *this = *this << n; return *this; }
    __forceinline  Int128& operator>>=(int n) { *this = *this >> n; return *this; }

    // ---- explicit conversions ----

     __forceinline explicit operator bool() const { return lo != 0 || hi != 0; }
     __forceinline constexpr explicit operator int64_t() const { return static_cast<int64_t>(lo); }
     __forceinline explicit operator uint64_t() const { return lo; }
     __forceinline explicit operator int32_t() const { return static_cast<int32_t>(lo); }
     __forceinline explicit operator uint32_t() const { return static_cast<uint32_t>(lo); }
     __forceinline explicit operator int16_t()const{return static_cast<int16_t>(lo);}
     __forceinline explicit operator uint16_t() const{return static_cast<uint16_t>(lo);}
     __forceinline explicit operator double()const{
         // Approximate: high bits scaled + low bits
         double d =static_cast<double>(lo);
         return d;
     }

    // ---- helpers ----

     __forceinline bool is_zero()  const { return lo == 0 && hi == 0; }
     __forceinline bool is_neg()   const { return hi < 0; }

private:
    // Unsigned 128-bit division (used internally)
    struct U128 { uint64_t hi = 0, lo = 0; };

    // Unsigned 128-bit comparison: a >= b ?
    __forceinline static  bool _u128_ge(const U128& a, const U128& b) {
        return a.hi > b.hi || (a.hi == b.hi && a.lo >= b.lo);
    }

    // Unsigned 128-bit subtraction: a -= b  (assumes a >= b).
    __forceinline  static  void _u128_sub(U128& a, const U128& b) {
        uint64_t sub_lo = a.lo - b.lo;
        uint64_t borrow = (sub_lo > a.lo) ? 1 : 0;
        a.hi = a.hi - b.hi - borrow;
        a.lo = sub_lo;
    }

    // Test bit i (0 = LSB, 127 = MSB) of a 128-bit unsigned value.
    __forceinline static  int _bit128(const U128& x, int i) {
        return (i >= 64) ? static_cast<int>((x.hi >> (i - 64)) & 1)
                         : static_cast<int>((x.lo >> i) & 1);
    }

    // Shift 128-bit value left by 1, returning the carried-out bit (bit 127).
    __forceinline static  int _shl1_128(U128& x) {
        int carry = static_cast<int>(x.hi >> 63);
        x.hi = (x.hi << 1) | (x.lo >> 63);
        x.lo = x.lo << 1;
        return carry;
    }

#if !MMZZ_INT128_HAS_X64_INTRIN
    // Count leading zeros of a 64-bit value (64 for input 0). Used only by
    // the divlu fallback below, which itself is only compiled in when no
    // x64 hardware 128/64 division intrinsic is available.
    __forceinline static int _nlz64(uint64_t x) {
        if (x == 0) return 64;
        int n = 0;
        while ((x & (uint64_t(1) << 63)) == 0) { x <<= 1; ++n; }
        return n;
    }

    // Hacker's Delight "divlu": unsigned 128/64 -> 64-bit quotient + 64-bit
    // remainder, using only native 32-bit-digit / 64-bit divisions — no
    // 128-bit hardware capability required, so this is what 32-bit builds
    // (or any compiler without _udiv128/__int128) fall back to instead of
    // the O(128) bit-by-bit loop. Requires u1 < v, i.e. the quotient must
    // fit in 64 bits (callers here always check that before calling).
    __forceinline static uint64_t _divlu64(uint64_t u1, uint64_t u0, uint64_t v, uint64_t& r) {
        const uint64_t b = uint64_t(1) << 32;
        int s = _nlz64(v);
        uint64_t vn = v << s;
        uint64_t vn1 = vn >> 32;
        uint64_t vn0 = vn & 0xFFFFFFFFu;

        uint64_t un64 = s > 0 ? ((u1 << s) | (u0 >> (64 - s))) : u1;
        uint64_t un10 = s > 0 ? (u0 << s) : u0;
        uint64_t un1 = un10 >> 32;
        uint64_t un0 = un10 & 0xFFFFFFFFu;

        uint64_t q1 = un64 / vn1;
        uint64_t rhat = un64 - q1 * vn1;
        while (q1 >= b || q1 * vn0 > b * rhat + un1) {
            q1--; rhat += vn1;
            if (rhat >= b) break;
        }

        uint64_t un21 = un64 * b + un1 - q1 * vn;

        uint64_t q0 = un21 / vn1;
        rhat = un21 - q0 * vn1;
        while (q0 >= b || q0 * vn0 > b * rhat + un0) {
            q0--; rhat += vn1;
            if (rhat >= b) break;
        }

        r = (un21 * b + un0 - q0 * vn) >> s;
        return q1 * b + q0;
    }
#endif



    // Unsigned 128/128 restoring division.
    // Partial remainder stored as (r_extra * 2^128 + r.hi * 2^64 + r.lo).
    __forceinline static  void _udiv128(const U128& u, const U128& v,
                                   U128& q, U128& r) {
        if (v.hi == 0 && v.lo == 0) { q = {0,0}; r = {0,0}; return; }
        if (_u128_ge(v, u)) {
            if (u.hi == v.hi && u.lo == v.lo) { q = {1,0}; r = {0,0}; }
            else { q = {0,0}; r = u; }
            return;
        }

        q = {0, 0};
        r = {0, 0};
        int r_extra = 0;  // high word (bits 128+) of the partial remainder

        for (int i = 127; i >= 0; --i) {
            // --- r = (r << 1) | bit_i(u) ---
            int top = _shl1_128(r);
            r_extra = (r_extra << 1) | top;
            r.lo |= _bit128(u, i);

            // --- q = q << 1 ---
            _shl1_128(q);

            // --- if r >= v, subtract ---
            if (r_extra > 0 || _u128_ge(r, v)) {
                if (r_extra == 0) {
                    _u128_sub(r, v);
                } else if (_u128_ge(r, v)) {
                    _u128_sub(r, v);          // r_extra unchanged
                } else {
                    // r < v:  borrow from r_extra
                    // (r_extra * 2^128 + r) - v = (r_extra-1) * 2^128 + (2^128 - (v - r))
                    // 2^128 - d  (where d = v - r) in 128-bit two's complement: ~d + 1
                    r_extra--;
                    U128 d = v;
                    _u128_sub(d, r);
                    r.hi = ~d.hi;
                    r.lo = ~d.lo + 1;
                    if (r.lo == 0) r.hi++;
                }
                q.lo |= 1;
            }
        }
    }

    // Signed division
     __forceinline std::pair<Int128, Int128> divmod(const Int128& rhs) const {
        bool neg_q = is_neg() != rhs.is_neg();
        bool neg_r = is_neg();

        U128 a = _abs128(*this);
        U128 b = _abs128(rhs);

        // Tier 1 fast path: both operands fit in 64 unsigned bits (the
        // common case for game/physics-scale fixed-point values). Use the
        // CPU's native 64/64 division instruction directly instead of the
        // general 128-round software long-division loop below. Result is
        // bit-for-bit identical to the general path since both operands
        // and results fit exactly in 64 bits here.
        if (a.hi == 0 && b.hi == 0) {
            uint64_t qq = a.lo / b.lo;
            uint64_t rr = a.lo % b.lo;
            Int128 quotient  = neg_q ? -Int128(static_cast<int64_t>(0), qq) : Int128(static_cast<int64_t>(0), qq);
            Int128 remainder = neg_r ? -Int128(static_cast<int64_t>(0), rr) : Int128(static_cast<int64_t>(0), rr);
            return {quotient, remainder};
        }

        // Tier 2 fast path: dividend needs the full 128 bits but the
        // divisor still fits in 64 bits (e.g. Fixed::operator/ shifts the
        // numerator left by FractionBits before dividing, so a numerator
        // with a non-zero integer part routinely overflows 64 bits while
        // the denominator — an ordinary Fixed value's raw_ — usually does
        // not). This is exactly the "128/64" shape hardware division
        // instructions are built for, so instead of falling through to the
        // O(128) bit-loop we compute it in O(1)/O(few):
        //   - x64 build: one _udiv128 hardware instruction (after making
        //     sure the quotient actually fits in 64 bits, otherwise that
        //     intrinsic traps with #DE).
        //   - 32-bit (or other non-x64) build: the classic Hacker's
        //     Delight "divlu" algorithm, which gets the same 128/64 answer
        //     using only two native 64/32-ish digit divisions instead of
        //     128 bit-shift-and-compare iterations.
        if (b.hi == 0 && b.lo != 0) {
            if (a.hi < b.lo) {
                // quotient fits in 64 bits: hi/lo can be combined with a
                // single 128/64 division instead of the O(128) loop.
#if MMZZ_INT128_HAS_X64_INTRIN
                unsigned __int64 rem;
                unsigned __int64 qq = ::_udiv128(a.hi, a.lo, b.lo, &rem);
#else
                uint64_t rem;
                uint64_t qq = _divlu64(a.hi, a.lo, b.lo, rem);
#endif
                Int128 quotient  = neg_q ? -Int128(static_cast<int64_t>(0), qq)  : Int128(static_cast<int64_t>(0), qq);
                Int128 remainder = neg_r ? -Int128(static_cast<int64_t>(0), rem) : Int128(static_cast<int64_t>(0), rem);
                return {quotient, remainder};
            }
            // a.hi >= b.lo: quotient needs both 64-bit halves. Peel off the
            // high half with one native division, then feed the remainder
            // plus the low half through the same 128/64 routine above —
            // still just two hardware-speed steps, never the O(128) loop.
            uint64_t q_hi = a.hi / b.lo;
            uint64_t r_hi = a.hi % b.lo;
#if MMZZ_INT128_HAS_X64_INTRIN
            unsigned __int64 rem;
            unsigned __int64 q_lo = ::_udiv128(r_hi, a.lo, b.lo, &rem);
#else
            uint64_t rem;
            uint64_t q_lo = _divlu64(r_hi, a.lo, b.lo, rem);
#endif
            Int128 quotient  = neg_q ? -Int128(static_cast<int64_t>(q_hi), q_lo) : Int128(static_cast<int64_t>(q_hi), q_lo);
            Int128 remainder = neg_r ? -Int128(static_cast<int64_t>(0), rem)     : Int128(static_cast<int64_t>(0), rem);
            return {quotient, remainder};
        }

        // Tier 3: fully general fallback (divisor also needs the full 128
        // bits). No native instruction handles 128/128 division on any
        // mainstream CPU, so this stays the O(128) bit-by-bit loop.
        U128 q, r;
        _udiv128(a, b, q, r);

        Int128 quotient = neg_q ? -_from_u128(q) : _from_u128(q);
        Int128 remainder = neg_r ? -_from_u128(r) : _from_u128(r);
        return {quotient, remainder};
    }


    __forceinline static  U128 _abs128(const Int128& x) {
        if (x.is_neg()) {
            Int128 n = -x;
            return {static_cast<uint64_t>(n.hi), n.lo};
        }
        return {static_cast<uint64_t>(x.hi), x.lo};
    }

    __forceinline static  Int128 _from_u128(const U128& x) {
        return Int128(static_cast<int64_t>(x.hi), x.lo);
    }
};

// ============================================================================
// fast_shr_trunc(x, n) - generic helper used by Fixed<> to divide the raw
// intermediate value by 2^FractionBits (i.e. by Scale) after a multiply.
//
// For built-in integer types the compiler already turns "x / (T(1)<<n)"
// into an efficient shift sequence when the divisor is a compile-time
// constant, so the generic template is left as a plain division.
// Int128 has no such compiler help (it is a software type), so it gets a
// dedicated O(1) overload built on Int128::divpow2 instead of falling
// through to the very slow general Int128 division.
// ============================================================================

__forceinline Int128 fast_shr_trunc(const Int128& v, int n) {
    return v.divpow2(n);
}

// ---- stream output ----
__forceinline  std::ostream& operator<<(std::ostream& os, const Int128& v) {
    if (v.is_neg()) {
        Int128 n = -v;
        // Will recurse once (n is non-negative)
        os << '-' << n;
        return os;
    }
    if (v.hi == 0) {
        os << v.lo;
        return os;
    }
    // Divide by 10 recursively to print decimal
    Int128 q = v / Int128(10);
    Int128 r = v % Int128(10);
    if (!q.is_zero())
        os << q;
    os << (char)('0' + static_cast<int>(r.lo));
    return os;
}

#endif
