#pragma once

#include <cstdint>
#include <climits>
#include <ostream>
#include <utility>
#include <type_traits>

// ============================================================================
// Int128 — portable 128-bit signed integer (two's complement)
//
// Usable as BaseType / Store in Fixed<>.
// ============================================================================

struct Int128 {
    uint64_t lo;
    int64_t  hi;

    // ---- constructors ----

   constexpr  Int128() : lo(0), hi(0) {}

    Int128(int64_t v) : lo(static_cast<uint64_t>(v)), hi(v < 0 ? -1 : 0) {}

    Int128(uint64_t v) : lo(v), hi(0) {}

    Int128(int32_t v) : lo(static_cast<uint64_t>(static_cast<int64_t>(v))), hi(v < 0 ? -1 : 0) {}

    Int128(uint32_t v) : lo(v), hi(0) {}

    Int128(int64_t hi_, uint64_t lo_) : lo(lo_), hi(hi_) {}

    // ---- unary ----

    Int128 operator+() const { return *this; }

    Int128 operator-() const {
        // -x = ~x + 1
        uint64_t neg_lo = ~lo + 1;
        int64_t  neg_hi = ~hi + (neg_lo == 0 ? 1 : 0);
        return Int128(neg_hi, neg_lo);
    }

    Int128 operator~() const { return Int128(~hi, ~lo); }

    // ---- comparison ----

    bool operator==(const Int128& rhs) const { return hi == rhs.hi && lo == rhs.lo; }
    bool operator< (const Int128& rhs) const {
        if (hi != rhs.hi) return hi < rhs.hi;
        return lo < rhs.lo;
    }

    // ---- addition ----

     Int128 operator+(const Int128& rhs) const {
        uint64_t sum_lo = lo + rhs.lo;
        int64_t  carry  = (sum_lo < lo) ? 1 : 0;
        int64_t  sum_hi = hi + rhs.hi + carry;
        return Int128(sum_hi, sum_lo);
    }

    // ---- subtraction ----

     Int128 operator-(const Int128& rhs) const {
        uint64_t diff_lo = lo - rhs.lo;
        int64_t  borrow  = (diff_lo > lo) ? 1 : 0;
        int64_t  diff_hi = hi - rhs.hi - borrow;
        return Int128(diff_hi, diff_lo);
    }

    // ---- multiplication (128 × 128 → low 128 bits) ----

     Int128 operator*(const Int128& rhs) const {
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

     Int128 operator/(const Int128& rhs) const {
        return divmod(rhs).first;
    }

     Int128 operator%(const Int128& rhs) const {
        return divmod(rhs).second;
    }

    // ---- shift ----

    Int128 operator<<(int n) const {
        if (n <= 0) return *this;
        if (n >= 128) return Int128(0, 0);
        if (n >= 64) return Int128(static_cast<int64_t>(lo << (n - 64)), 0);
        return Int128((hi << n) | static_cast<int64_t>(lo >> (64 - n)), lo << n);
    }

     Int128 operator>>(int n) const {
        if (n <= 0) return *this;
        if (n >= 128) return Int128(hi < 0 ? -1 : 0, hi < 0 ? ~uint64_t(0) : 0);
        if (n >= 64) return Int128(hi < 0 ? -1 : 0, static_cast<uint64_t>(hi) >> (n - 64));
        uint64_t shift_lo = (lo >> n) | (static_cast<uint64_t>(hi) << (64 - n));
        return Int128(hi >> n, shift_lo);
    }

    // ---- compound assignment ----

     Int128& operator+=(const Int128& rhs) { *this = *this + rhs; return *this; }
     Int128& operator-=(const Int128& rhs) { *this = *this - rhs; return *this; }
     Int128& operator*=(const Int128& rhs) { *this = *this * rhs; return *this; }
     Int128& operator/=(const Int128& rhs) { *this = *this / rhs; return *this; }
     Int128& operator%=(const Int128& rhs) { *this = *this % rhs; return *this; }
     Int128& operator<<=(int n) { *this = *this << n; return *this; }
     Int128& operator>>=(int n) { *this = *this >> n; return *this; }

    // ---- explicit conversions ----

     explicit operator bool() const { return lo != 0 || hi != 0; }
     explicit operator int64_t() const { return static_cast<int64_t>(lo); }
     explicit operator uint64_t() const { return lo; }
     explicit operator int32_t() const { return static_cast<int32_t>(lo); }
     explicit operator uint32_t() const { return static_cast<uint32_t>(lo); }
     explicit operator int16_t()const{return static_cast<int16_t>(lo);}
     explicit operator uint16_t() const{return static_cast<uint16_t>(lo);}
     explicit operator double()const{
         // Approximate: high bits scaled + low bits
         double d =static_cast<double>(lo);
         return d;
     }

    // ---- helpers ----

     bool is_zero()  const { return lo == 0 && hi == 0; }
     bool is_neg()   const { return hi < 0; }

private:
    // Unsigned 128-bit division (used internally)
    struct U128 { uint64_t hi = 0, lo = 0; };

    // Unsigned 128-bit comparison: a >= b ?
    static  bool _u128_ge(const U128& a, const U128& b) {
        return a.hi > b.hi || (a.hi == b.hi && a.lo >= b.lo);
    }

    // Unsigned 128-bit subtraction: a -= b  (assumes a >= b).
    static  void _u128_sub(U128& a, const U128& b) {
        uint64_t sub_lo = a.lo - b.lo;
        uint64_t borrow = (sub_lo > a.lo) ? 1 : 0;
        a.hi = a.hi - b.hi - borrow;
        a.lo = sub_lo;
    }

    // Test bit i (0 = LSB, 127 = MSB) of a 128-bit unsigned value.
    static  int _bit128(const U128& x, int i) {
        return (i >= 64) ? static_cast<int>((x.hi >> (i - 64)) & 1)
                         : static_cast<int>((x.lo >> i) & 1);
    }

    // Shift 128-bit value left by 1, returning the carried-out bit (bit 127).
    static  int _shl1_128(U128& x) {
        int carry = static_cast<int>(x.hi >> 63);
        x.hi = (x.hi << 1) | (x.lo >> 63);
        x.lo = x.lo << 1;
        return carry;
    }

    // Unsigned 128/128 restoring division.
    // Partial remainder stored as (r_extra * 2^128 + r.hi * 2^64 + r.lo).
    static  void _udiv128(const U128& u, const U128& v,
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
     std::pair<Int128, Int128> divmod(const Int128& rhs) const {
        bool neg_q = is_neg() != rhs.is_neg();
        bool neg_r = is_neg();

        U128 a = _abs128(*this);
        U128 b = _abs128(rhs);
        U128 q, r;
        _udiv128(a, b, q, r);

        Int128 quotient = neg_q ? -_from_u128(q) : _from_u128(q);
        Int128 remainder = neg_r ? -_from_u128(r) : _from_u128(r);
        return {quotient, remainder};
    }

    static  U128 _abs128(const Int128& x) {
        if (x.is_neg()) {
            Int128 n = -x;
            return {static_cast<uint64_t>(n.hi), n.lo};
        }
        return {static_cast<uint64_t>(x.hi), x.lo};
    }

    static  Int128 _from_u128(const U128& x) {
        return Int128(static_cast<int64_t>(x.hi), x.lo);
    }
};

// ---- stream output ----
inline std::ostream& operator<<(std::ostream& os, const Int128& v) {
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
