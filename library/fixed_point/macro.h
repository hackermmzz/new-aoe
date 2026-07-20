#pragma once


#if defined(_WIN64) || defined(__x86_64__) || defined(__aarch64__) || defined(__LP64__)
#define MMZZ_X64
#else
#define MMZZ_X86
#endif

#if !defined(_MSC_VER)
#define __forceinline inline __attribute__((always_inline))
#endif
