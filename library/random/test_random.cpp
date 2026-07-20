#include "random.hpp"
#include <cstdio>
#include <cassert>
#include <cmath>

static int passed = 0;
static int failed = 0;

#define TEST(name) do { printf("  %-40s ", name); } while(0)
#define CHECK(cond) do { \
    if (cond) { printf("PASS\n"); ++passed; } \
    else { printf("FAIL  (%s:%d)\n", __FILE__, __LINE__); ++failed; } \
} while(0)

int main()
{
    printf("=== Random 单元测试 ===\n\n");

    // ---- 确定性 ----
    TEST("相同种子产生相同序列");
    {
        Random<double> r1(42);
        Random<double> r2(42);
        bool same = true;
        for (int i = 0; i < 1000; ++i) {
            if (r1.nextRaw() != r2.nextRaw()) { same = false; break; }
        }
        CHECK(same);
    }

    // ---- 不同种子产生不同序列 ----
    TEST("不同种子产生不同序列");
    {
        Random<double> r1(42);
        Random<double> r2(99);
        int diff = 0;
        for (int i = 0; i < 100; ++i) {
            if (r1.nextRaw() != r2.nextRaw()) ++diff;
        }
        CHECK(diff > 0);
    }

    // ---- setSeed 重置 ----
    TEST("setSeed 重置后序列一致");
    {
        Random<double> r(42);
        uint64_t vals[10];
        for (int i = 0; i < 10; ++i) vals[i] = r.nextRaw();

        r.setSeed(42);
        bool same = true;
        for (int i = 0; i < 10; ++i) {
            if (r.nextRaw() != vals[i]) { same = false; break; }
        }
        CHECK(same);
    }

    // ---- nextInt(max) 范围 ----
    TEST("nextInt(max) 范围检查");
    {
        Random<double> r(123);
        bool ok = true;
        for (int i = 0; i < 10000; ++i) {
            int v = r.nextInt(50);
            if (v < 0 || v > 50) { ok = false; break; }
        }
        CHECK(ok);
    }

    // ---- nextInt(min, max) 范围 ----
    TEST("nextInt(min, max) 范围检查");
    {
        Random<double> r(456);
        bool ok = true;
        for (int i = 0; i < 10000; ++i) {
            int v = r.nextInt(10, 30);
            if (v < 10 || v > 30) { ok = false; break; }
        }
        CHECK(ok);
    }

    // ---- nextDouble 范围 ----
    TEST("nextDouble() 范围 [0, 1)");
    {
        Random<double> r(789);
        bool ok = true;
        for (int i = 0; i < 10000; ++i) {
            double v = r.nextDouble();
            if (v < 0.0 || v >= 1.0) { ok = false; break; }
        }
        CHECK(ok);
    }

    // ---- nextDouble(min, max) 范围 ----
    TEST("nextDouble(min, max) 范围");
    {
        Random<double> r(111);
        bool ok = true;
        for (int i = 0; i < 10000; ++i) {
            double v = r.nextDouble(1.5, 3.5);
            if (v < 1.5 || v >= 3.5) { ok = false; break; }
        }
        CHECK(ok);
    }

    // ---- nextChance ----
    TEST("nextChance 极端值");
    {
        Random<double> r(222);
        CHECK(r.nextChance(1.0) == true);   // 100% 概率一定为 true
    }
    {
        Random<double> r(333);
        CHECK(r.nextChance(0.0) == false);  // 0% 概率一定为 false
    }

    // ---- nextDouble 确定性 ----
    TEST("nextDouble 相同种子相同序列");
    {
        Random<double> r1(77);
        Random<double> r2(77);
        bool same = true;
        for (int i = 0; i < 1000; ++i) {
            if (std::abs(r1.nextDouble() - r2.nextDouble()) > 1e-15) { same = false; break; }
        }
        CHECK(same);
    }

    // ---- 默认种子 ----
    TEST("默认种子构造不崩溃");
    {
        Random<double> r;
        for (int i = 0; i < 100; ++i) {
            r.nextRaw();
            r.nextInt(100);
            r.nextDouble();
        }
        CHECK(true);
    }

    printf("\n=== 结果: %d 通过, %d 失败 ===\n", passed, failed);
    return failed > 0 ? 1 : 0;
}
