#pragma once

#include <cstdint>
#include <cstdlib>

#if !defined(_MSC_VER)
#define __forceinline inline __attribute__((always_inline))
#endif
// ============================================================================
// Random — 64 位版本，使用 Xorshift64 算法
// ============================================================================
class Random
{
public:
    __forceinline explicit Random(uint64_t seed = 0) { setSeed(seed); }

    __forceinline void setSeed(uint64_t seed)
    {
        m_state = (seed == 0) ? 88172645463325252ULL : seed;
        for (int i = 0; i < 12; ++i) { nextRaw(); }
    }

    __forceinline uint64_t nextRaw()
    {
        uint64_t x = m_state;
        x ^= x << 13;
        x ^= x >> 7;
        x ^= x << 17;
        m_state = x;
        return x;
    }

    __forceinline int32_t nextInt(int32_t max)
    {
        if (max <= 0) return 0;
        return static_cast<int32_t>(nextRaw() % static_cast<uint64_t>(max + 1));
    }

    __forceinline int32_t nextInt(int32_t min, int32_t max)
    {
        if (min >= max) min^=max,max^=min,min^=max;
        return min + nextInt(max - min);
    }

    template<class DoubleTp>
    __forceinline DoubleTp nextDouble()
    {
        constexpr DoubleTp div(65536);
        //max is 65535
        return static_cast<DoubleTp>(nextRaw() >> 48) / div; // / 2^16
    }

    template<class DoubleTp>
    __forceinline DoubleTp nextDouble(const DoubleTp&min, const DoubleTp&max)
    {
        return min + nextDouble<DoubleTp>() * (max - min);
    }

    template<class DoubleTp>
    __forceinline bool nextChance(const DoubleTp&probability)
    {
        return nextDouble<DoubleTp>() < probability;
    }

private:
    uint64_t m_state;
};
