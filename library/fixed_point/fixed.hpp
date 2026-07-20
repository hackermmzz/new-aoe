#pragma once

#include <cstdint>
#include <type_traits>
#include <limits>
#include <ostream>
#include <iostream>
#include <cstdlib>


// ============================================================================
// fast_shr_trunc(x, n) - generic helper used by Fixed<> to divide the raw
// intermediate value by 2^FractionBits (i.e. by Scale) after a multiply.
//
// For built-in integer types the compiler already turns "x / (T(1)<<n)"
// into an efficient shift sequence when the divisor is a compile-time
// constant, so the generic template is left as a plain division.
// ============================================================================
template <typename T,
        typename = typename std::enable_if<std::is_integral<T>::value>::type>
__forceinline constexpr T fast_shr_trunc(T v, int n) {
    return v / (T(1) << n);
}
// ============================================================================
// Fixed-point number template
//
//   Fixed<BaseType, Store, FractionBits>
//
//   BaseType     — underlying type for intermediate computation (wider is safer)
//   Store        — storage type (may be smaller than BaseType to save memory)
//   FractionBits — number of fractional bits (0 = plain integer)
//
//   All arithmetic on BaseType and Store must be supported (checked at compile
//   time via static_assert).
// ============================================================================

template <typename BaseType,
          typename Store       = BaseType,
          int FractionBits     = 0>
class Fixed
{
    // ------------------------------------------------------------------------
    // Static sanity checks
    // ------------------------------------------------------------------------
    static_assert(FractionBits < static_cast<int>(sizeof(BaseType) * 8),
                  "FractionBits must be less than the bit-width of BaseType");
    static_assert(FractionBits >= 0,
                  "FractionBits must be non-negative");

    static_assert(sizeof(Store) <= sizeof(BaseType),
                  "Store should not be larger than BaseType");

    // Verify that BaseType and Store support basic arithmetic operations.
    // Each check fails to compile if the operator is missing on that type.
    static_assert(std::is_same<decltype(std::declval<BaseType>() + std::declval<BaseType>()), BaseType>::value
                  && std::is_same<decltype(std::declval<BaseType>() - std::declval<BaseType>()), BaseType>::value
                  && std::is_same<decltype(std::declval<BaseType>() * std::declval<BaseType>()), BaseType>::value
                  && std::is_same<decltype(std::declval<BaseType>() / std::declval<BaseType>()), BaseType>::value
                  && std::is_same<decltype(-std::declval<BaseType>()),                      BaseType>::value,
                  "BaseType must support +, -, *, /, unary -");

    static_assert(std::is_same<decltype(std::declval<Store>() + std::declval<Store>()), Store>::value
                  && std::is_same<decltype(std::declval<Store>() - std::declval<Store>()), Store>::value
                  && std::is_same<decltype(std::declval<Store>() * std::declval<Store>()), Store>::value
                  && std::is_same<decltype(std::declval<Store>() / std::declval<Store>()), Store>::value
                  && std::is_same<decltype(-std::declval<Store>()),                      Store>::value,
                  "Store must support +, -, *, /, unary -");

public:
    // ------------------------------------------------------------------------
    // Types
    // ------------------------------------------------------------------------
    using StorageType    = Store;
    using ComputeType    = BaseType;
    using ThisType       = Fixed<BaseType, Store, FractionBits>;

    static constexpr int kFractionBits = FractionBits;
    static constexpr BaseType Scale=static_cast<BaseType>(uint16_t(1)) << kFractionBits;
    __forceinline static ThisType Zero(){ ThisType ret; ret.raw_=static_cast<Store>(0); return ret;}
    // ------------------------------------------------------------------------
    // Data
    // ------------------------------------------------------------------------
    Store raw_;

    // ------------------------------------------------------------------------
    // Construction
    // ------------------------------------------------------------------------
    __forceinline constexpr Fixed() : raw_(0) {}
    __forceinline constexpr Fixed(const BaseType&val):raw_(val<<kFractionBits){}
    __forceinline constexpr Fixed(const Fixed&) = default;
    __forceinline constexpr Fixed& operator=(const Fixed&) = default;

    // Construct from a raw stored value (no scaling applied)
    __forceinline static  Fixed FromRaw(Store raw) {
        Fixed f;
        f.raw_ = raw;
        return f;
    }

    // Construct from integer or enum (both convert directly to BaseType)
    template <typename T,
              typename = typename std::enable_if<
                  std::is_integral<T>::value || std::is_enum<T>::value>::type>
     __forceinline constexpr Fixed(T val)
        : raw_(static_cast<Store>(static_cast<BaseType>(val)<<kFractionBits)) {}

    // Compile-time constructor (e.g. Fixed("3.14")).  No error checking —
    // invalid input produces a compile error at  time.
    __forceinline constexpr Fixed(const char* s)
        : raw_(static_cast<Store>(_parse(s))) {}

    // Runtime constructor from double (non-, for convenience).
    static __forceinline constexpr ThisType FromDouble(double val)
     {
        ThisType ret;
        ret.raw_=(static_cast<Store>(
            val * static_cast<double>(Scale)
        ));
        return ret;
    }
    static __forceinline constexpr ThisType FromFloat(float val)
    {
        ThisType ret;
        ret.raw_=(static_cast<Store>(
                   val * static_cast<double>(Scale)
                   ));
        return ret;
    }

    // Runtime string parser with validation.
    // Reports invalid characters and precision overflow, then exits.
    static __forceinline Fixed FromString(const char* s) {
        // --- sign ---
        bool neg = false;
        if (*s == '-') { neg = true; ++s; }
        else if (*s == '+') { ++s; }

        // --- integer part ---
        BaseType int_part = static_cast<BaseType>(0);
        while (*s >= '0' && *s <= '9') {
            int_part = int_part * static_cast<BaseType>(10)
                     + static_cast<BaseType>(*s - '0');
            ++s;
        }

        // --- fractional part ---
        BaseType frac_part   = static_cast<BaseType>(0);
        int      frac_digits = 0;
        if (*s == '.') {
            ++s;
            while (*s >= '0' && *s <= '9') {
                frac_part = frac_part * static_cast<BaseType>(10)
                          + static_cast<BaseType>(*s - '0');
                ++frac_digits;
                ++s;
            }
        }

        // --- invalid trailing characters ---
        if (*s != '\0') {
            std::cerr << "Fixed::FromString: invalid character '"
                      << *s << "' (0x" << std::hex << (int)(unsigned char)*s
                      << std::dec << ") in \"" << s << "\"" << std::endl;
            std::exit(1);
        }

        // --- assemble ---
        BaseType scale  = static_cast<BaseType>(1) << FractionBits;
        BaseType result = int_part * scale;

        if (frac_digits > 0) {
            BaseType pow10 = static_cast<BaseType>(1);
            for (int i = 0; i < frac_digits; ++i)
                pow10 = pow10 * static_cast<BaseType>(10);

            if (pow10 > scale) {
                std::cerr << "Fixed::FromString: " << frac_digits
                          << " fractional digits exceed "
                          << FractionBits << "-bit precision, value truncated"
                          << std::endl;
            }

            result = result + (frac_part * scale) / pow10;
        }

        return Fixed::FromRaw(static_cast<Store>(neg ? -result : result));
    }

    private:
    // ---- recursive  helpers (C++11-compatible, no loops) ----

    static   BaseType _pow10(int n) {
        return n <= 0 ? static_cast<BaseType>(1)
                      : static_cast<BaseType>(10) * _pow10(n - 1);
    }

    static   BaseType _parse_int(const char* s, BaseType acc) {
        return (*s >= '0' && *s <= '9')
                   ? _parse_int(s + 1, acc * static_cast<BaseType>(10)
                                         + static_cast<BaseType>(*s - '0'))
                   : acc;
    }

    static  int _skip_digits(const char* s) {
        return (*s >= '0' && *s <= '9') ? 1 + _skip_digits(s + 1) : 0;
    }

    static   BaseType _parse_frac(const char* s, int n) {
        return n <= 0 ? static_cast<BaseType>(0)
                      : static_cast<BaseType>(*s - '0') * _pow10(n - 1)
                          + _parse_frac(s + 1, n - 1);
    }

    // ---- main parser ----

    static  __forceinline BaseType _parse(const char* s) {
        // --- sign ---
        bool neg = false;
        if (*s == '-') { neg = true; ++s; }
        else if (*s == '+') { ++s; }

        // --- integer part ---
        BaseType int_part = _parse_int(s, static_cast<BaseType>(0));
        s += _skip_digits(s);

        // --- fractional part ---
        BaseType frac_part   = static_cast<BaseType>(0);
        int      frac_digits = 0;
        if (*s == '.') {
            ++s;
            frac_digits = _skip_digits(s);
            frac_part   = _parse_frac(s, frac_digits);
            s += frac_digits;
        }

        // --- assemble ---
        BaseType scale  = static_cast<BaseType>(1) << FractionBits;
        BaseType result = int_part * scale;

        if (frac_digits > 0) {
            BaseType pow10 = _pow10(frac_digits);
            result = result + (frac_part * scale) / pow10;
        }

        return neg ? -result : result;
    }

    public:

    // ------------------------------------------------------------------------
    // Conversion operators
    // ------------------------------------------------------------------------
    template <typename Int,
              typename = typename std::enable_if<std::is_integral<Int>::value>::type>
    __forceinline constexpr  operator Int() const {
        // raw_ is already stored at Store's width; dividing directly in
        // Store avoids promoting to the (possibly much slower, e.g. Int128)
        // BaseType just to perform a division that Store can do natively.
        return static_cast<Int>(raw_ / static_cast<Store>(Scale));
    }
    __forceinline constexpr operator char()const{
        return static_cast<char>(int());
    }
    __forceinline constexpr operator unsigned char()const{
        return static_cast<unsigned char>(char());
    }
    __forceinline constexpr  explicit operator float() const {
        // Go through double to avoid missing Int128->float conversion
        return static_cast<float>(static_cast<double>(raw_) / static_cast<double>(Scale));
    }

    __forceinline constexpr explicit operator double() const {
        // Direct conversion from raw fixed-point value to double.
        // Avoids going through BaseType (Int128) which has broken negative conversion.
        return static_cast<double>(raw_) / static_cast<double>(Scale);
    }

    // ------------------------------------------------------------------------
    // Raw access
    // ------------------------------------------------------------------------
    __forceinline   Store raw() const { return raw_; }

    // ------------------------------------------------------------------------
    // Unary operators
    // ------------------------------------------------------------------------
    __forceinline   Fixed operator+() const { return *this; }
    __forceinline   Fixed operator-() const { return Fixed::FromRaw(static_cast<Store>(-raw_)); }

    // ------------------------------------------------------------------------
    // Comparison operators
    // ------------------------------------------------------------------------
    __forceinline   bool operator==(const Fixed& rhs) const { return raw_ == rhs.raw_; }
    __forceinline   bool operator!=(const Fixed& rhs) const { return !(raw_ == rhs.raw_); }
    __forceinline   bool operator< (const Fixed& rhs) const { return raw_ <  rhs.raw_; }
    __forceinline   bool operator> (const Fixed& rhs) const { return rhs.raw_<raw_; }
    __forceinline   bool operator<=(const Fixed& rhs) const { return !(raw_ > rhs.raw_); }
    __forceinline   bool operator>=(const Fixed& rhs) const { return !(raw_ < rhs.raw_); }

    // ------------------------------------------------------------------------
    // Arithmetic: Fixed OP Fixed
    // ------------------------------------------------------------------------
    __forceinline   Fixed operator+(const Fixed& rhs) const {
        return Fixed::FromRaw(static_cast<Store>(raw_ + rhs.raw_));
    }

    __forceinline  Fixed operator-(const Fixed& rhs) const {
        return Fixed::FromRaw(static_cast<Store>(raw_ - rhs.raw_));
    }

    __forceinline   Fixed operator*(const Fixed& rhs) const {
        return Fixed::FromRaw(static_cast<Store>(
            fast_shr_trunc(static_cast<BaseType>(raw_) * static_cast<BaseType>(rhs.raw_), FractionBits)));
    }

    __forceinline constexpr Fixed operator/(const Fixed& rhs) const {
        return Fixed::FromRaw(static_cast<Store>(
            (static_cast<BaseType>(raw_) << FractionBits) / static_cast<BaseType>(rhs.raw_)));
    }

    __forceinline  constexpr Fixed operator%(const Fixed& rhs) const {
        return Fixed::FromRaw(static_cast<Store>((raw_%rhs.raw_)));
    }

    // ------------------------------------------------------------------------
    // Arithmetic: Fixed OP integer
    // ------------------------------------------------------------------------

    template <typename Int>
    __forceinline   auto operator+(Int rhs) const ->
        typename std::enable_if<std::is_integral<Int>::value, Fixed>::type
    {
        return *this + Fixed(static_cast<BaseType>(rhs));
    }

    template <typename Int>
    __forceinline   auto operator-(Int rhs) const ->
        typename std::enable_if<std::is_integral<Int>::value, Fixed>::type
    {
        return *this - Fixed(static_cast<BaseType>(rhs));
    }

    template <typename Int>
    __forceinline   auto operator*(Int rhs) const ->
        typename std::enable_if<std::is_integral<Int>::value, Fixed>::type
    {
        return Fixed::FromRaw(static_cast<Store>(
            static_cast<BaseType>(raw_) *
            static_cast<BaseType>(rhs)
        ));
    }

    template <typename Int>
    __forceinline   auto operator/(Int rhs) const ->
        typename std::enable_if<std::is_integral<Int>::value, Fixed>::type
    {
        return Fixed::FromRaw(static_cast<Store>(
            static_cast<BaseType>(raw_) / static_cast<BaseType>(rhs)));
    }

    // ------------------------------------------------------------------------
    // Compound-assignment: Fixed OP Fixed
    // ------------------------------------------------------------------------
    __forceinline   Fixed& operator+=(const Fixed& rhs) {
        raw_ = static_cast<Store>(raw_ + rhs.raw_);
        return *this;
    }

    __forceinline  Fixed& operator-=(const Fixed& rhs) {
        raw_ = static_cast<Store>(raw_ - rhs.raw_);
        return *this;
    }

    __forceinline   Fixed& operator*=(const Fixed& rhs) {
        raw_ = static_cast<Store>(
            fast_shr_trunc(static_cast<BaseType>(raw_) * static_cast<BaseType>(rhs.raw_), FractionBits));
        return *this;
    }

    __forceinline   Fixed& operator/=(const Fixed& rhs) {
        raw_ = static_cast<Store>(
            (static_cast<BaseType>(raw_) << FractionBits) / static_cast<BaseType>(rhs.raw_));
        return *this;
    }

    __forceinline   Fixed& operator%=(const Fixed& rhs) {
        raw_ = static_cast<Store>(raw_ % rhs.raw_);
        return *this;
    }

    // ------------------------------------------------------------------------
    // Increment / Decrement
    // ------------------------------------------------------------------------
    __forceinline   Fixed& operator++() { *this += Fixed(1); return *this; }
    __forceinline   Fixed  operator++(int) { Fixed tmp = *this; ++*this; return tmp; }
    __forceinline   Fixed& operator--() { *this -= Fixed(1); return *this; }
    __forceinline   Fixed  operator--(int) { Fixed tmp = *this; --*this; return tmp; }

    // ------------------------------------------------------------------------
    // Absolute value
    // ------------------------------------------------------------------------
    __forceinline   Fixed abs() const {
        return Fixed::FromRaw(raw_ < 0 ? static_cast<Store>(-raw_) : raw_);
    }

    // ------------------------------------------------------------------------
    // Round to nearest integer (returning a Fixed still in the same format)
    // ------------------------------------------------------------------------
    __forceinline Fixed round() const {
        Store v = raw_;
        constexpr Store s = static_cast<Store>(Scale);   // 编译期已知的2的幂
        constexpr Store mask = s - 1;
        if (v >= 0) {
            return Fixed::FromRaw(static_cast<Store>((v + s / 2) & ~mask));
        } else {
            Store nv = -v;
            return Fixed::FromRaw(static_cast<Store>(-((nv + s / 2) & ~mask)));
        }
    }

    __forceinline Fixed floor() const {
        constexpr Store s = static_cast<Store>(Scale);
        constexpr Store mask = s - 1;
        return Fixed::FromRaw(static_cast<Store>(raw_ & ~mask));
    }

    __forceinline Fixed ceil() const {
        constexpr Store s = static_cast<Store>(Scale);
        constexpr Store mask = s - 1;
        return Fixed::FromRaw(static_cast<Store>((raw_ + mask) & ~mask));
    }

    __forceinline Fixed trunc() const {
        constexpr Store s = static_cast<Store>(Scale);
        constexpr Store mask = s - 1;
        // v >> 63 是算术右移：v为负时全1，v非负时全0，用它当"是否要补偏移量"的开关
        Store sign_mask = raw_ >> (sizeof(Store) * 8 - 1);
        Store adj = sign_mask & mask;
        return Fixed::FromRaw(static_cast<Store>((raw_ + adj) & ~mask));
    }

    // ------------------------------------------------------------------------
    // Constants
    // ------------------------------------------------------------------------
    __forceinline  static  Fixed min() {
        return Fixed::FromRaw(std::numeric_limits<Store>::min());
    }
    __forceinline  static  Fixed max() {
        return Fixed::FromRaw(std::numeric_limits<Store>::max());
    }
    __forceinline   static  Fixed epsilon() {
        return Fixed::FromRaw(static_cast<Store>(1));
    }
    __forceinline  static  Fixed pi() {
        return Fixed::FromDouble(3.14159265358979323846);
    }
    __forceinline  static  Fixed e() {
        return Fixed::FromDouble(2.71828182845904523536);
    }

};

// ============================================================================
// Symmetric operators:  integer OP Fixed
// ============================================================================

    template <typename BaseType, typename Store, int FBits, typename Int>
    __forceinline  auto operator+(Int lhs, const Fixed<BaseType, Store, FBits>& rhs) ->
    typename std::enable_if<std::is_integral<Int>::value,
                            Fixed<BaseType, Store, FBits>>::type
    {
    return rhs + lhs;
    }

    template <typename BaseType, typename Store, int FBits, typename Int>
    __forceinline auto operator-(Int lhs, const Fixed<BaseType, Store, FBits>& rhs) ->
    typename std::enable_if<std::is_integral<Int>::value,
                            Fixed<BaseType, Store, FBits>>::type
    {
    return Fixed<BaseType, Store, FBits>(static_cast<BaseType>(lhs)) - rhs;
    }

    template <typename BaseType, typename Store, int FBits, typename Int>
    __forceinline  auto operator*(Int lhs, const Fixed<BaseType, Store, FBits>& rhs) ->
    typename std::enable_if<std::is_integral<Int>::value,
                            Fixed<BaseType, Store, FBits>>::type
    {
    return rhs * lhs;
    }

    template <typename BaseType, typename Store, int FBits, typename Int>
    __forceinline auto operator/(Int lhs, const Fixed<BaseType, Store, FBits>& rhs) ->
    typename std::enable_if<std::is_integral<Int>::value,
                            Fixed<BaseType, Store, FBits>>::type
    {
    return Fixed<BaseType, Store, FBits>(static_cast<BaseType>(lhs)) / rhs;
    }

    // ============================================================================
    // Stream output
    // ============================================================================

    template <typename BaseType, typename Store, int FractionBits>
    __forceinline std::ostream& operator<<(std::ostream& os, const Fixed<BaseType, Store, FractionBits>& v) {
    return os << static_cast<double>(v);
    }

    // ============================================================================
    // Convenience aliases
    // ============================================================================

    // Q-format nomenclature: Qm.n where m = integer bits, n = FractionBits
    // Storage = BaseType

    template <int FractionBits>
    using FixedQ = Fixed<int32_t, int32_t, FractionBits>;

    template <int FractionBits>
    using FixedQL = Fixed<int64_t, int64_t, FractionBits>;

    // Common fixed-point formats
    using Fixed16_16  = Fixed<int32_t, int32_t, 16>;   // Q16.16
    using Fixed8_24   = Fixed<int32_t, int32_t, 24>;   // Q8.24
    using Fixed24_8   = Fixed<int32_t, int32_t, 8>;    // Q24.8
    using Fixed32_32  = Fixed<int64_t, int64_t, 32>;   // Q32.32
    using Fixed16      = Fixed<int32_t, int32_t, 16>;  // legacy alias
