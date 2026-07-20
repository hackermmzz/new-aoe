#include "include.h"
#include <cstdio>
#include <cmath>
#include <chrono>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <cstring>

using FixedType = Fixed<Int128, int64_t, 32>;

// ============================================================================
// Benchmark helper
// ============================================================================

struct BenchResult {
    std::string name;
    double fixed_time_ms;
    double double_time_ms;
    double speedup; // >1 means fixed is slower, <1 means fixed is faster
};

static std::vector<BenchResult> g_results;

template <typename FuncFixed, typename FuncDouble>
void bench(const char* name, int iterations, FuncFixed f_fixed, FuncDouble f_double) {
    // Warm-up
    for (int i = 0; i < 100; ++i) { f_fixed(); f_double(); }

    auto t1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) { f_fixed(); }
    auto t2 = std::chrono::high_resolution_clock::now();

    auto t3 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) { f_double(); }
    auto t4 = std::chrono::high_resolution_clock::now();

    double fixed_ms = std::chrono::duration<double, std::milli>(t2 - t1).count();
    double double_ms = std::chrono::duration<double, std::milli>(t4 - t3).count();

    BenchResult r{};
    r.name = name;
    r.fixed_time_ms = fixed_ms;
    r.double_time_ms = double_ms;
    r.speedup = (double_ms > 0.001) ? (fixed_ms / double_ms) : 0;
    g_results.push_back(r);
}

// Prevent compiler from optimizing away results
volatile uint64_t sink_u64 = 0;
volatile double     sink_d   = 0;
volatile bool       sink_b   = false;

// ============================================================================
// Test data generators
// ============================================================================

static double g_vals[1024];
static FixedType g_fixed_vals[1024];

void init_data() {
    for (int i = 0; i < 1024; ++i) {
        double v = (i * 17.0 + 3.14159) / 7.0 - 50.0;
        g_vals[i] = v;
        g_fixed_vals[i] = FixedType::FromDouble(v);
    }
}

// ============================================================================
// Individual benchmarks
// ============================================================================

void bm_add() {
    bench("add (array)", 2000,
        [](){
            FixedType acc = FixedType::Zero();
            for (int i = 0; i < 1023; ++i)
                acc = acc + g_fixed_vals[i];
            sink_u64 = static_cast<uint64_t>((int64_t)acc);
        },
        [](){
            double acc = 0.0;
            for (int i = 0; i < 1023; ++i)
                acc = acc + g_vals[i];
            sink_d = acc;
        });
}

void bm_sub() {
    bench("sub (array)", 2000,
        [](){
            FixedType acc = g_fixed_vals[0];
            for (int i = 1; i < 1024; ++i)
                acc = acc - g_fixed_vals[i];
            sink_u64 = static_cast<uint64_t>((int64_t)acc);
        },
        [](){
            double acc = g_vals[0];
            for (int i = 1; i < 1024; ++i)
                acc = acc - g_vals[i];
            sink_d = acc;
        });
}

void bm_mul() {
    bench("mul (array)", 1000,
        [](){
            FixedType acc = FixedType::FromDouble(1.0);
            for (int i = 0; i < 1024; ++i) {
                FixedType v = g_fixed_vals[i];
                if (v == FixedType::Zero()) continue;
                acc = acc * v;
                if ((int64_t)acc > 1e10 || (int64_t)acc < -1e10)
                    acc = FixedType::FromDouble(1.0);
            }
            sink_u64 = static_cast<uint64_t>((int64_t)acc);
        },
        [](){
            double acc = 1.0;
            for (int i = 0; i < 1024; ++i) {
                double v = g_vals[i];
                if (v == 0.0) continue;
                acc = acc * v;
                if (acc > 1e10 || acc < -1e10)
                    acc = 1.0;
            }
            sink_d = acc;
        });
}

void bm_div() {
    bench("div (array)", 1000,
        [](){
            FixedType acc = FixedType::FromDouble(1000000.0);
            for (int i = 0; i < 1024; ++i) {
                FixedType v = g_fixed_vals[i];
                if (v == FixedType::Zero()) continue;
                acc = acc / v;
                if ((int64_t)acc > 1e8) acc = FixedType::FromDouble(1000.0);
                if ((int64_t)acc < -1e8) acc = FixedType::FromDouble(1000.0);
            }
            sink_u64 = static_cast<uint64_t>((int64_t)acc);
        },
        [](){
            double acc = 1000000.0;
            for (int i = 0; i < 1024; ++i) {
                double v = g_vals[i];
                if (v == 0.0) continue;
                acc = acc / v;
                if (acc > 1e8) acc = 1000.0;
                if (acc < -1e8) acc = 1000.0;
            }
            sink_d = acc;
        });
}

void bm_mod() {
    bench("mod (array)", 1000,
        [](){
            FixedType acc = FixedType::FromDouble(12345.678);
            for (int i = 0; i < 1024; ++i) {
                FixedType v = g_fixed_vals[i].abs() + FixedType(1);
                acc = acc % v;
            }
            sink_u64 = static_cast<uint64_t>((int64_t)acc);
        },
        [](){
            double acc = 12345.678;
            for (int i = 0; i < 1024; ++i) {
                double v = std::fabs(g_vals[i]) + 1.0;
                acc = std::fmod(acc, v);
            }
            sink_d = acc;
        });
}

void bm_neg() {
    bench("negate", 20000,
        [](){
            FixedType acc = FixedType::Zero();
            for (int i = 0; i < 1024; ++i)
                acc = acc - g_fixed_vals[i];
            sink_u64 = static_cast<uint64_t>((int64_t)acc);
        },
        [](){
            double acc = 0.0;
            for (int i = 0; i < 1024; ++i)
                acc = acc - g_vals[i];
            sink_d = acc;
        });
}

void bm_comp_eq() {
    bench("compare ==" , 2000,
        [](){
            int c = 0;
            for (int i = 0; i < 1024; ++i)
                c += (g_fixed_vals[i] == g_fixed_vals[(i+1)&1023]);
            sink_b = (c > 0);
        },
        [](){
            int c = 0;
            for (int i = 0; i < 1024; ++i)
                c += (g_vals[i] == g_vals[(i+1)&1023]);
            sink_b = (c > 0);
        });
}

void bm_comp_lt() {
    bench("compare <", 2000,
        [](){
            int c = 0;
            for (int i = 0; i < 1024; ++i)
                c += (g_fixed_vals[i] < g_fixed_vals[(i+1)&1023]);
            sink_b = (c > 0);
        },
        [](){
            int c = 0;
            for (int i = 0; i < 1024; ++i)
                c += (g_vals[i] < g_vals[(i+1)&1023]);
            sink_b = (c > 0);
        });
}

void bm_comp_le() {
    bench("compare <=", 2000,
        [](){
            int c = 0;
            for (int i = 0; i < 1024; ++i)
                c += (g_fixed_vals[i] <= g_fixed_vals[(i+1)&1023]);
            sink_b = (c > 0);
        },
        [](){
            int c = 0;
            for (int i = 0; i < 1024; ++i)
                c += (g_vals[i] <= g_vals[(i+1)&1023]);
            sink_b = (c > 0);
        });
}

void bm_comp_gt() {
    bench("compare >", 2000,
        [](){
            int c = 0;
            for (int i = 0; i < 1024; ++i)
                c += (g_fixed_vals[i] > g_fixed_vals[(i+1)&1023]);
            sink_b = (c > 0);
        },
        [](){
            int c = 0;
            for (int i = 0; i < 1024; ++i)
                c += (g_vals[i] > g_vals[(i+1)&1023]);
            sink_b = (c > 0);
        });
}

void bm_comp_ge() {
    bench("compare >=", 2000,
        [](){
            int c = 0;
            for (int i = 0; i < 1024; ++i)
                c += (g_fixed_vals[i] >= g_fixed_vals[(i+1)&1023]);
            sink_b = (c > 0);
        },
        [](){
            int c = 0;
            for (int i = 0; i < 1024; ++i)
                c += (g_vals[i] >= g_vals[(i+1)&1023]);
            sink_b = (c > 0);
        });
}

void bm_abs() {
    bench("abs", 10000,
        [](){
            FixedType acc = FixedType::Zero();
            for (int i = 0; i < 1024; ++i)
                acc = acc + abs(g_fixed_vals[i]);
            sink_u64 = static_cast<uint64_t>((int64_t)acc);
        },
        [](){
            double acc = 0.0;
            for (int i = 0; i < 1024; ++i)
                acc = acc + std::fabs(g_vals[i]);
            sink_d = acc;
        });
}

void bm_floor() {
    bench("floor", 10000,
        [](){
            FixedType acc = FixedType::Zero();
            for (int i = 0; i < 1024; ++i)
                acc = acc + floor(g_fixed_vals[i]);
            sink_u64 = static_cast<uint64_t>((int64_t)acc);
        },
        [](){
            double acc = 0.0;
            for (int i = 0; i < 1024; ++i)
                acc = acc + std::floor(g_vals[i]);
            sink_d = acc;
        });
}

void bm_ceil() {
    bench("ceil", 10000,
        [](){
            FixedType acc = FixedType::Zero();
            for (int i = 0; i < 1024; ++i)
                acc = acc + ceil(g_fixed_vals[i]);
            sink_u64 = static_cast<uint64_t>((int64_t)acc);
        },
        [](){
            double acc = 0.0;
            for (int i = 0; i < 1024; ++i)
                acc = acc + std::ceil(g_vals[i]);
            sink_d = acc;
        });
}

void bm_round() {
    bench("round", 10000,
        [](){
            FixedType acc = FixedType::Zero();
            for (int i = 0; i < 1024; ++i)
                acc = acc + round(g_fixed_vals[i]);
            sink_u64 = static_cast<uint64_t>((int64_t)acc);
        },
        [](){
            double acc = 0.0;
            for (int i = 0; i < 1024; ++i)
                acc = acc + std::round(g_vals[i]);
            sink_d = acc;
        });
}

void bm_sqrt() {
    bench("sqrt", 2000,
        [](){
            FixedType acc = FixedType::Zero();
            for (int i = 0; i < 1024; ++i) {
                FixedType v = g_fixed_vals[i].abs() + FixedType(1);
                acc = acc + sqrt(v);
            }
            sink_u64 = static_cast<uint64_t>((int64_t)acc);
        },
        [](){
            double acc = 0.0;
            for (int i = 0; i < 1024; ++i) {
                double v = std::fabs(g_vals[i]) + 1.0;
                acc = acc + std::sqrt(v);
            }
            sink_d = acc;
        });
}

void bm_pow_pos() {
    bench("pow (pos exp)", 1000,
        [](){
            FixedType acc = FixedType::Zero();
            for (int i = 0; i < 256; ++i) {
                FixedType base = g_fixed_vals[i % 256].abs() + FixedType(1);
                FixedType r = pow(base, 5);
                acc = acc + r;
            }
            sink_u64 = static_cast<uint64_t>((int64_t)acc);
        },
        [](){
            double acc = 0.0;
            for (int i = 0; i < 256; ++i) {
                double base = std::fabs(g_vals[i % 256]) + 1.0;
                double r = std::pow(base, 5.0);
                acc = acc + r;
            }
            sink_d = acc;
        });
}

void bm_pow_neg() {
    bench("pow (neg exp)", 1000,
        [](){
            FixedType acc = FixedType::Zero();
            for (int i = 0; i < 256; ++i) {
                FixedType base = g_fixed_vals[i % 256].abs() + FixedType(2);
                FixedType r = pow(base, -3);
                acc = acc + r;
            }
            sink_u64 = static_cast<uint64_t>((int64_t)acc);
        },
        [](){
            double acc = 0.0;
            for (int i = 0; i < 256; ++i) {
                double base = std::fabs(g_vals[i % 256]) + 2.0;
                double r = std::pow(base, -3.0);
                acc = acc + r;
            }
            sink_d = acc;
        });
}

void bm_atan2() {
    bench("atan2", 500,
        [](){
            FixedType acc = FixedType::Zero();
            for (int i = 0; i < 256; ++i) {
                FixedType y = g_fixed_vals[i % 256];
                FixedType x = g_fixed_vals[(i + 128) % 256];
                FixedType r = atan2(y, x);
                acc = acc + r;
            }
            sink_u64 = static_cast<uint64_t>((int64_t)acc);
        },
        [](){
            double acc = 0.0;
            for (int i = 0; i < 256; ++i) {
                double y = g_vals[i % 256];
                double x = g_vals[(i + 128) % 256];
                double r = std::atan2(y, x);
                acc = acc + r;
            }
            sink_d = acc;
        });
}

void bm_mixed_expr() {
    bench("mixed expression", 1000,
        [](){
            FixedType acc = FixedType::Zero();
            for (int i = 0; i < 256; ++i) {
                FixedType a = g_fixed_vals[i];
                FixedType b = g_fixed_vals[(i+1)&255];
                FixedType c = g_fixed_vals[(i+2)&255];
                // Complex expression using multiple ops
                FixedType t = (a + b) * (b - c) / (c.abs() + FixedType(1));
                t = t * t;
                t = sqrt(t + FixedType(1));
                t = t + pow(a.abs() + FixedType(1), 2);
                t = t - floor(b);
                t = t + ceil(c);
                t = t - round(a);
                acc = acc + t;
            }
            sink_u64 = static_cast<uint64_t>((int64_t)acc);
        },
        [](){
            double acc = 0.0;
            for (int i = 0; i < 256; ++i) {
                double a = g_vals[i];
                double b = g_vals[(i+1)&255];
                double c = g_vals[(i+2)&255];
                double t = (a + b) * (b - c) / (std::fabs(c) + 1.0);
                t = t * t;
                t = std::sqrt(t + 1.0);
                t = t + std::pow(std::fabs(a) + 1.0, 2.0);
                t = t - std::floor(b);
                t = t + std::ceil(c);
                t = t - std::round(a);
                acc = acc + t;
            }
            sink_d = acc;
        });
}

void bm_from_double() {
    bench("FromDouble", 20000,
        [](){
            FixedType acc = FixedType::Zero();
            for (int i = 0; i < 1024; ++i)
                acc = acc + FixedType::FromDouble(g_vals[i]);
            sink_u64 = static_cast<uint64_t>((int64_t)acc);
        },
        [](){
            double acc = 0.0;
            for (int i = 0; i < 1024; ++i)
                acc = acc + g_vals[i];
            sink_d = acc;
        });
}

void bm_to_double() {
    bench("to double conv", 20000,
        [](){
            double acc = 0.0;
            for (int i = 0; i < 1024; ++i)
                acc = acc + static_cast<double>(g_fixed_vals[i]);
            sink_d = acc;
        },
        [](){
            double acc = 0.0;
            for (int i = 0; i < 1024; ++i)
                acc = acc + g_vals[i];
            sink_d = acc;
        });
}

void bm_to_int() {
    bench("to int64 conv", 20000,
        [](){
            int64_t acc = 0;
            for (int i = 0; i < 1024; ++i)
                acc = acc + static_cast<int64_t>(g_fixed_vals[i]);
            sink_u64 = static_cast<uint64_t>(acc);
        },
        [](){
            int64_t acc = 0;
            for (int i = 0; i < 1024; ++i)
                acc = acc + static_cast<int64_t>(g_vals[i]);
            sink_u64 = static_cast<uint64_t>(acc);
        });
}

void bm_compound_add() {
    bench("compound +=", 2000,
        [](){
            FixedType acc = FixedType::Zero();
            for (int i = 0; i < 1023; ++i)
                acc += g_fixed_vals[i];
            sink_u64 = static_cast<uint64_t>((int64_t)acc);
        },
        [](){
            double acc = 0.0;
            for (int i = 0; i < 1023; ++i)
                acc += g_vals[i];
            sink_d = acc;
        });
}

void bm_compound_mul() {
    bench("compound *=", 2000,
        [](){
            FixedType acc = FixedType::FromDouble(1.0);
            for (int i = 0; i < 1024; ++i) {
                FixedType v = g_fixed_vals[i];
                if (v == FixedType::Zero()) continue;
                acc *= v;
                if ((int64_t)acc > 1e10 || (int64_t)acc < -1e10)
                    acc = FixedType::FromDouble(1.0);
            }
            sink_u64 = static_cast<uint64_t>((int64_t)acc);
        },
        [](){
            double acc = 1.0;
            for (int i = 0; i < 1024; ++i) {
                double v = g_vals[i];
                if (v == 0.0) continue;
                acc *= v;
                if (acc > 1e10 || acc < -1e10)
                    acc = 1.0;
            }
            sink_d = acc;
        });
}

void bm_increment() {
    bench("pre-increment ++", 10000,
        [](){
            FixedType acc = FixedType::Zero();
            for (int i = 0; i < 1024; ++i) {
                ++acc;
            }
            sink_u64 = static_cast<uint64_t>((int64_t)acc);
        },
        [](){
            double acc = 0.0;
            for (int i = 0; i < 1024; ++i) {
                acc += 1.0;
            }
            sink_d = acc;
        });
}

void bm_decrement() {
    bench("pre-decrement --", 50000,
        [](){
            FixedType acc = FixedType::FromDouble(1024.0);
            for (int i = 0; i < 1024; ++i) {
                --acc;
            }
            sink_u64 = static_cast<uint64_t>((int64_t)acc);
        },
        [](){
            double acc = 1024.0;
            for (int i = 0; i < 1024; ++i) {
                acc -= 1.0;
            }
            sink_d = acc;
        });
}

void bm_trunc() {
    bench("trunc", 50000,
        [](){
            FixedType acc = FixedType::Zero();
            for (int i = 0; i < 1024; ++i)
                acc = acc + g_fixed_vals[i].trunc();
            sink_u64 = static_cast<uint64_t>((int64_t)acc);
        },
        [](){
            double acc = 0.0;
            for (int i = 0; i < 1024; ++i)
                acc = acc + std::trunc(g_vals[i]);
            sink_d = acc;
        });
}

void bm_mul_int() {
    bench("mul by int", 50000,
        [](){
            FixedType acc = FixedType::Zero();
            for (int i = 0; i < 1024; ++i)
                acc = acc + g_fixed_vals[i] * 3;
            sink_u64 = static_cast<uint64_t>((int64_t)acc);
        },
        [](){
            double acc = 0.0;
            for (int i = 0; i < 1024; ++i)
                acc = acc + g_vals[i] * 3.0;
            sink_d = acc;
        });
}

void bm_div_int() {
    bench("div by int", 50000,
        [](){
            FixedType acc = FixedType::Zero();
            for (int i = 0; i < 1024; ++i) {
                FixedType v = g_fixed_vals[i];
                if (v == FixedType::Zero()) continue;
                acc = acc + v / 3;
            }
            sink_u64 = static_cast<uint64_t>((int64_t)acc);
        },
        [](){
            double acc = 0.0;
            for (int i = 0; i < 1024; ++i) {
                double v = g_vals[i];
                if (v == 0.0) continue;
                acc = acc + v / 3.0;
            }
            sink_d = acc;
        });
}

void bm_constants() {
    bench("constants (pi,e,min,max)", 100000,
        [](){
            FixedType acc = FixedType::Zero();
            acc = acc + FixedType::pi();
            acc = acc + FixedType::e();
            acc = acc + FixedType::min();
            acc = acc + FixedType::max();
            acc = acc + FixedType::epsilon();
            sink_u64 = static_cast<uint64_t>((int64_t)acc);
        },
        [](){
            double acc = 0.0;
            acc = acc + 3.14159265358979323846;
            acc = acc + 2.71828182845904523536;
            acc = acc + (-1.7014118346046923173e38);
            acc = acc + 1.7014118346046923173e38;
            acc = acc + 1e-38;
            sink_d = acc;
        });
}

void bm_scalar_add() {
    bench("add scalar int", 50000,
        [](){
            FixedType acc = FixedType::Zero();
            for (int i = 0; i < 1024; ++i)
                acc = acc + 5;
            sink_u64 = static_cast<uint64_t>((int64_t)acc);
        },
        [](){
            double acc = 0.0;
            for (int i = 0; i < 1024; ++i)
                acc = acc + 5.0;
            sink_d = acc;
        });
}

void bm_scalar_sub() {
    bench("sub scalar int", 50000,
        [](){
            FixedType acc = FixedType::FromDouble(5120.0);
            for (int i = 0; i < 1024; ++i)
                acc = acc - 5;
            sink_u64 = static_cast<uint64_t>((int64_t)acc);
        },
        [](){
            double acc = 5120.0;
            for (int i = 0; i < 1024; ++i)
                acc = acc - 5.0;
            sink_d = acc;
        });
}

void bm_post_inc() {
    bench("post-increment i++", 50000,
        [](){
            FixedType acc = FixedType::Zero();
            for (int i = 0; i < 1024; ++i) {
                FixedType t = acc++;
                sink_u64 = static_cast<uint64_t>((int64_t)t);
            }
        },
        [](){
            double acc = 0.0;
            for (int i = 0; i < 1024; ++i) {
                double t = acc;
                acc += 1.0;
                sink_d = t;
            }
        });
}

// ============================================================================
// Correctness verification
// ============================================================================

bool verify_correctness() {
    bool ok = true;

    // Test basic arithmetic
    FixedType fa = FixedType::FromDouble(3.5);
    FixedType fb = FixedType::FromDouble(2.0);
    double da = 3.5, db = 2.0;

    if (static_cast<double>(fa + fb) != da + db) { printf("FAIL: add\n"); ok = false; }
    if (static_cast<double>(fa - fb) != da - db) { printf("FAIL: sub\n"); ok = false; }
    if (static_cast<double>(fa * fb) != da * db) { printf("FAIL: mul\n"); ok = false; }
    if (static_cast<double>(fa / fb) != da / db) { printf("FAIL: div\n"); ok = false; }

    // Test comparisons
    if (!(fa > fb)) { printf("FAIL: >\n"); ok = false; }
    if (!(fa >= fb)) { printf("FAIL: >=\n"); ok = false; }
    if (!(fb < fa)) { printf("FAIL: <\n"); ok = false; }
    if (!(fb <= fa)) { printf("FAIL: <=\n"); ok = false; }
    if (!(fa == fa)) { printf("FAIL: ==\n"); ok = false; }

    // Test abs
    FixedType fneg = FixedType::FromDouble(-5.5);
    if (static_cast<double>(fneg.abs()) != 5.5) { printf("FAIL: abs\n"); ok = false; }

    // Test floor/ceil/round/trunc
    FixedType fpos = FixedType::FromDouble(3.7);
    FixedType fneg2 = FixedType::FromDouble(-3.7);
    if (static_cast<double>(floor(fpos)) != 3.0) { printf("FAIL: floor pos\n"); ok = false; }
    if (static_cast<double>(ceil(fpos)) != 4.0) { printf("FAIL: ceil pos\n"); ok = false; }
    if (static_cast<double>(round(fpos)) != 4.0) { printf("FAIL: round pos\n"); ok = false; }
    if (static_cast<double>(fpos.trunc()) != 3.0) { printf("FAIL: trunc pos\n"); ok = false; }
    if (static_cast<double>(floor(fneg2)) != -4.0) { printf("FAIL: floor neg\n"); ok = false; }
    if (static_cast<double>(ceil(fneg2)) != -3.0) { printf("FAIL: ceil neg\n"); ok = false; }
    if (static_cast<double>(round(fneg2)) != -4.0) { printf("FAIL: round neg\n"); ok = false; }
    if (static_cast<double>(fneg2.trunc()) != -3.0) { printf("FAIL: trunc neg\n"); ok = false; }

    // Test sqrt
    FixedType fsqr = FixedType::FromDouble(16.0);
    double sqr_sqrt = static_cast<double>(sqrt(fsqr));
    if (std::fabs(sqr_sqrt - 4.0) > 0.01) { printf("FAIL: sqrt (got %f)\n", sqr_sqrt); ok = false; }

    // Test pow
    FixedType fpow = FixedType::FromDouble(2.0);
    double pow_result = static_cast<double>(pow(fpow, 10));
    if (std::fabs(pow_result - 1024.0) > 1.0) { printf("FAIL: pow 2^10 (got %f)\n", pow_result); ok = false; }

    // Test pow negative exponent
    FixedType fpow2 = FixedType::FromDouble(2.0);
    double pow_neg = static_cast<double>(pow(fpow2, -2));
    if (std::fabs(pow_neg - 0.25) > 0.01) { printf("FAIL: pow 2^-2 (got %f)\n", pow_neg); ok = false; }

    // Test atan2
    FixedType fy = FixedType::FromDouble(1.0);
    FixedType fx = FixedType::FromDouble(1.0);
    double atan2_result = static_cast<double>(atan2(fy, fx));
    if (std::fabs(atan2_result - 0.785398) > 0.01) { printf("FAIL: atan2(1,1) (got %f)\n", atan2_result); ok = false; }

    // Test constants
    double pi_val = static_cast<double>(FixedType::pi());
    if (std::fabs(pi_val - 3.14159265358979) > 1e-6) { printf("FAIL: pi (got %f)\n", pi_val); ok = false; }
    double e_val = static_cast<double>(FixedType::e());
    if (std::fabs(e_val - 2.71828182845904) > 1e-6) { printf("FAIL: e (got %f)\n", e_val); ok = false; }

    // Test FromRaw / raw access
    FixedType fr = FixedType::FromRaw(0x100000000LL); // 1.0 in Q32
    if (static_cast<double>(fr) < 0.9 || static_cast<double>(fr) > 1.1) { printf("FAIL: FromRaw\n"); ok = false; }

    return ok;
}

// ============================================================================
// Main
// ============================================================================

int main() {
    printf("=============================================================\n");
    printf("  Fixed<Int128, int64_t, 32> vs double Performance Benchmark\n");
    printf("=============================================================\n\n");

    printf("Initializing test data...\n");
    init_data();

    printf("Running correctness verification...\n");
    if (!verify_correctness()) {
        printf("\n*** CORRECTNESS CHECK FAILED ***\n");
        return 1;
    }
    printf("Correctness OK.\n\n");

    printf("Running benchmarks...\n\n");

    // Arithmetic
    bm_add();
    bm_sub();
    bm_mul();
    bm_div();
    bm_mod();
    bm_neg();

    // Scalar operations
    bm_mul_int();
    bm_div_int();
    bm_scalar_add();
    bm_scalar_sub();

    // Compound assignment
    bm_compound_add();
    bm_compound_mul();

    // Increment/decrement
    bm_increment();
    bm_decrement();
    bm_post_inc();

    // Comparisons
    bm_comp_eq();
    bm_comp_lt();
    bm_comp_le();
    bm_comp_gt();
    bm_comp_ge();

    // Conversions
    bm_from_double();
    bm_to_double();
    bm_to_int();

    // Math functions
    bm_abs();
    bm_floor();
    bm_ceil();
    bm_round();
    bm_trunc();
    bm_sqrt();
    bm_pow_pos();
    bm_pow_neg();
    bm_atan2();

    // Mixed expression
    bm_mixed_expr();

    // Constants
    bm_constants();

    // Print results table
    printf("\n");
    printf("=============================================================\n");
    printf("  Results (time in milliseconds, lower is better)\n");
    printf("=============================================================\n");
    printf("%-28s %12s %12s %12s\n", "Test", "Fixed(ms)", "Double(ms)", "Ratio(F/D)");
    printf("-------------------------------------------------------------\n");

    for (const auto& r : g_results) {
        printf("%-28s %12.3f %12.3f %12.3f",
               r.name.c_str(), r.fixed_time_ms, r.double_time_ms, r.speedup);
        if (r.speedup > 10.0)
            printf("  [SLOW!]");
        else if (r.speedup < 0.5)
            printf("  [FAST!]");
        printf("\n");
    }

    printf("=============================================================\n");

    // Summary statistics
    double total_fixed = 0, total_double = 0;
    int count = 0;
    int slow_count = 0;
    int fast_count = 0;
    for (const auto& r : g_results) {
        total_fixed += r.fixed_time_ms;
        total_double += r.double_time_ms;
        count++;
        if (r.speedup > 2.0) slow_count++;
        if (r.speedup < 0.5) fast_count++;
    }

    printf("\nSummary:\n");
    printf("  Total tests:      %d\n", count);
    printf("  Fixed total:      %.3f ms\n", total_fixed);
    printf("  Double total:     %.3f ms\n", total_double);
    printf("  Overall ratio:    %.3f (Fixed/Double)\n", total_fixed / (total_double > 0 ? total_double : 0.001));
    printf("  Slow tests (>2x): %d\n", slow_count);
    printf("  Fast tests (<0.5x): %d\n", fast_count);
    printf("=============================================================\n");

    return 0;
}
