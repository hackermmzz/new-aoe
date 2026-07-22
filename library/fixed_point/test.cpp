// ============================================================================
// 定点数库全面功能测试
// 覆盖：构造、转换、算术、比较、取整、math函数、边界值
// ============================================================================

#include "include.h"
#include <cstdio>
#include <cmath>
#include <cstring>

using FixedType = Fixed<Int128, int64_t, 32>;

static int g_pass = 0;
static int g_fail = 0;
static const double EPS = 1e-6;

#define TEST(name) static void test_##name()
#define RUN(name) do { printf("  [%s]... ", #name); test_##name(); } while(0)

// 浮点比较（允许误差）
bool approx(double a, double b, double eps = EPS) {
    return std::fabs(a - b) < eps;
}

void PASS() {
    printf("\033[32mPASS\033[0m\n");
    g_pass++;
}

void FAIL(const char* reason) {
    printf("\033[31mFAIL: %s\033[0m\n", reason);
    g_fail++;
}

// ============================================================================
// 1. 构造函数测试
// ============================================================================

TEST(constructor_default) {
    FixedType f;
    if (f.raw_ != 0) { FAIL("default not zero"); return; }
    PASS();
}

TEST(constructor_int) {
    FixedType f1(5);
    FixedType f2(-3);
    FixedType f3(0);
    bool ok = true;
    if (!approx((double)f1, 5.0)) { ok = false; FAIL("int 5"); }
    if (!approx((double)f2, -3.0)) { ok = false; FAIL("int -3"); }
    if (!approx((double)f3, 0.0)) { ok = false; FAIL("int 0"); }
    if (ok) PASS();
}

TEST(constructor_double) {
    FixedType f1 = FixedType::FromDouble(3.14159);
    FixedType f2 = FixedType::FromDouble(-2.71828);
    FixedType f3 = FixedType::FromDouble(0.0);
    bool ok = true;
    if (!approx((double)f1, 3.14159, 1e-5)) { ok = false; FAIL("double 3.14159"); }
    if (!approx((double)f2, -2.71828, 1e-5)) { ok = false; FAIL("double -2.71828"); }
    if (!approx((double)f3, 0.0)) { ok = false; FAIL("double 0"); }
    if (ok) PASS();
}

TEST(constructor_float) {
    FixedType f = FixedType::FromFloat(1.5f);
    if (!approx((double)f, 1.5)) { FAIL("float 1.5"); return; }
    PASS();
}

TEST(constructor_fromraw) {
    // raw_ = Scale * 2 => value = 2.0
    int64_t raw = (int64_t)FixedType::Scale * 2;
    FixedType f = FixedType::FromRaw(raw);
    if (!approx((double)f, 2.0)) { FAIL("FromRaw 2.0"); return; }
    PASS();
}

TEST(constructor_string_pos) {
    FixedType f("3.14159");
    if (!approx((double)f, 3.14159, 1e-4)) { FAIL("string 3.14159"); return; }
    PASS();
}

TEST(constructor_string_neg) {
    FixedType f("-2.5");
    if (!approx((double)f, -2.5)) { FAIL("string -2.5"); return; }
    PASS();
}

TEST(constructor_string_int) {
    FixedType f("42");
    if (!approx((double)f, 42.0)) { FAIL("string 42"); return; }
    PASS();
}

TEST(constructor_string_zero) {
    FixedType f("0");
    if (!approx((double)f, 0.0)) { FAIL("string 0"); return; }
    PASS();
}

TEST(constructor_large_int) {
    // 大整数：接近 int64 范围
    FixedType f(1000000);
    if (!approx((double)f, 1000000.0)) { FAIL("large int 1000000"); return; }
    PASS();
}

TEST(constructor_small_fraction) {
    // 非常小的分数
    FixedType f = FixedType::FromDouble(0.00001);
    double v = (double)f;
    if (!approx(v, 0.00001, 1e-5)) { FAIL("small fraction"); return; }
    PASS();
}

// ============================================================================
// 2. 类型转换测试
// ============================================================================

TEST(conv_to_int) {
    FixedType f1 = FixedType::FromDouble(3.9);
    FixedType f2 = FixedType::FromDouble(-3.9);
    int i1 = (int)f1;
    int i2 = (int)f2;
    bool ok = true;
    if (i1 != 3) { ok = false; FAIL("int conv 3.9->3"); }
    if (i2 != -3) { ok = false; FAIL("int conv -3.9->-3"); }
    if (ok) PASS();
}

TEST(conv_to_int64) {
    FixedType f = FixedType::FromDouble(12345.6789);
    int64_t i = (int64_t)f;
    if (i != 12345) { FAIL("int64 conv"); return; }
    PASS();
}

TEST(conv_to_char) {
    FixedType f(65);
    char c = f;
    if (c != 'A') { FAIL("char conv 65->A"); return; }
    PASS();
}

TEST(conv_to_uchar) {
    FixedType f(255);
    unsigned char c = f;
    if (c != 255) { FAIL("unsigned char conv 255"); return; }
    PASS();
}

TEST(conv_to_double_pos) {
    FixedType f = FixedType::FromDouble(123.456789);
    double d = f;
    if (!approx(d, 123.456789, 1e-5)) { FAIL("double pos"); return; }
    PASS();
}

TEST(conv_to_double_neg) {
    FixedType f = FixedType::FromDouble(-123.456789);
    double d = (double)f;
    if (!approx(d, -123.456789, 1e-5)) { FAIL("double neg"); return; }
    PASS();
}

TEST(conv_to_float) {
    FixedType f = FixedType::FromDouble(1.5);
    float fl = (float)f;
    if (std::fabs(fl - 1.5f) > 1e-5f) { FAIL("float conv"); return; }
    PASS();
}

// ============================================================================
// 3. 常量测试
// ============================================================================

TEST(constant_zero) {
    FixedType z = FixedType::Zero();
    if (z.raw_ != 0) { FAIL("Zero not 0"); return; }
    PASS();
}

TEST(constant_pi) {
    FixedType p = FixedType::pi();
    if (!approx((double)p, 3.14159265358979, 1e-6)) { FAIL("pi"); return; }
    PASS();
}

TEST(constant_e) {
    FixedType e = FixedType::e();
    if (!approx((double)e, 2.71828182845904, 1e-6)) { FAIL("e"); return; }
    PASS();
}

TEST(constant_epsilon) {
    FixedType ep = FixedType::epsilon();
    // epsilon = 1 / 2^32 ≈ 2.328e-10
    double d = (double)ep;
    if (d <= 0.0 || d > 1e-9) { FAIL("epsilon range"); return; }
    PASS();
}

// ============================================================================
// 4. 一元运算符测试
// ============================================================================

TEST(unary_plus) {
    FixedType f = FixedType::FromDouble(-5.5);
    FixedType g = +f;
    if (g.raw_ != f.raw_) { FAIL("unary +"); return; }
    PASS();
}

TEST(unary_minus_pos) {
    FixedType f = FixedType::FromDouble(5.5);
    FixedType g = -f;
    if (!approx((double)g, -5.5)) { FAIL("unary - pos"); return; }
    PASS();
}

TEST(unary_minus_neg) {
    FixedType f = FixedType::FromDouble(-5.5);
    FixedType g = -f;
    if (!approx((double)g, 5.5)) { FAIL("unary - neg"); return; }
    PASS();
}

TEST(unary_minus_zero) {
    FixedType z = FixedType::Zero();
    FixedType nz = -z;
    if (nz.raw_ != 0) { FAIL("unary - zero"); return; }
    PASS();
}

// ============================================================================
// 5. 算术运算测试 (Fixed OP Fixed)
// ============================================================================

TEST(add_pos_pos) {
    FixedType a = FixedType::FromDouble(1.5);
    FixedType b = FixedType::FromDouble(2.5);
    FixedType c = a + b;
    if (!approx((double)c, 4.0)) { FAIL("1.5+2.5=4"); return; }
    PASS();
}

TEST(add_neg_neg) {
    FixedType a = FixedType::FromDouble(-1.5);
    FixedType b = FixedType::FromDouble(-2.5);
    FixedType c = a + b;
    if (!approx((double)c, -4.0)) { FAIL("-1.5+-2.5=-4"); return; }
    PASS();
}

TEST(add_pos_neg) {
    FixedType a = FixedType::FromDouble(5.0);
    FixedType b = FixedType::FromDouble(-3.0);
    FixedType c = a + b;
    if (!approx((double)c, 2.0)) { FAIL("5+-3=2"); return; }
    PASS();
}

TEST(sub_pos_pos) {
    FixedType a = FixedType::FromDouble(5.0);
    FixedType b = FixedType::FromDouble(2.0);
    FixedType c = a - b;
    if (!approx((double)c, 3.0)) { FAIL("5-2=3"); return; }
    PASS();
}

TEST(sub_neg_neg) {
    FixedType a = FixedType::FromDouble(-2.0);
    FixedType b = FixedType::FromDouble(-5.0);
    FixedType c = a - b;
    if (!approx((double)c, 3.0)) { FAIL("-2-(-5)=3"); return; }
    PASS();
}

TEST(mul_pos_pos) {
    FixedType a = FixedType::FromDouble(2.5);
    FixedType b = FixedType::FromDouble(4.0);
    FixedType c = a * b;
    if (!approx((double)c, 10.0, 1e-4)) { FAIL("2.5*4=10"); return; }
    PASS();
}

TEST(mul_neg_pos) {
    FixedType a = FixedType::FromDouble(-2.5);
    FixedType b = FixedType::FromDouble(4.0);
    FixedType c = a * b;
    if (!approx((double)c, -10.0, 1e-4)) { FAIL("-2.5*4=-10"); return; }
    PASS();
}

TEST(mul_neg_neg) {
    FixedType a = FixedType::FromDouble(-2.5);
    FixedType b = FixedType::FromDouble(-4.0);
    FixedType c = a * b;
    if (!approx((double)c, 10.0, 1e-4)) { FAIL("-2.5*-4=10"); return; }
    PASS();
}

TEST(mul_small_frac) {
    FixedType a = FixedType::FromDouble(0.5);
    FixedType b = FixedType::FromDouble(0.5);
    FixedType c = a * b;
    if (!approx((double)c, 0.25, 1e-4)) { FAIL("0.5*0.5=0.25"); return; }
    PASS();
}

TEST(div_pos_pos) {
    FixedType a = FixedType::FromDouble(10.0);
    FixedType b = FixedType::FromDouble(2.0);
    FixedType c = a / b;
    if (!approx((double)c, 5.0, 1e-4)) { FAIL("10/2=5"); return; }
    PASS();
}

TEST(div_neg_pos) {
    FixedType a = FixedType::FromDouble(-10.0);
    FixedType b = FixedType::FromDouble(2.0);
    FixedType c = a / b;
    if (!approx((double)c, -5.0, 1e-4)) { FAIL("-10/2=-5"); return; }
    PASS();
}

TEST(div_neg_neg) {
    FixedType a = FixedType::FromDouble(-10.0);
    FixedType b = FixedType::FromDouble(-2.0);
    FixedType c = a / b;
    if (!approx((double)c, 5.0, 1e-4)) { FAIL("-10/-2=5"); return; }
    PASS();
}

TEST(mod_pos) {
    FixedType a = FixedType::FromDouble(10.5);
    FixedType b = FixedType::FromDouble(3.0);
    FixedType c = a % b;
    // 10.5 mod 3.0 = 1.5 (raw remainder scaled)
    if (!approx((double)c, 1.5, 1e-3)) { FAIL("10.5%3=1.5"); return; }
    PASS();
}

TEST(mod_neg) {
    FixedType a = FixedType::FromDouble(-10.0);
    FixedType b = FixedType::FromDouble(3.0);
    FixedType c = a % b;
    // C++: -10 % 3 = -1
    if (!approx((double)c, -1.0, 1e-3)) { FAIL("-10%3=-1"); return; }
    PASS();
}

// ============================================================================
// 6. 算术运算测试 (Fixed OP Integer / Integer OP Fixed)
// ============================================================================

TEST(add_fixed_int) {
    FixedType a = FixedType::FromDouble(5.5);
    FixedType c = a + 3;
    if (!approx((double)c, 8.5)) { FAIL("5.5+3=8.5"); return; }
    PASS();
}

TEST(add_int_fixed) {
    FixedType a = FixedType::FromDouble(5.5);
    FixedType c = 3 + a;
    if (!approx((double)c, 8.5)) { FAIL("3+5.5=8.5"); return; }
    PASS();
}

TEST(sub_fixed_int) {
    FixedType a = FixedType::FromDouble(5.5);
    FixedType c = a - 2;
    if (!approx((double)c, 3.5)) { FAIL("5.5-2=3.5"); return; }
    PASS();
}

TEST(sub_int_fixed) {
    FixedType a = FixedType::FromDouble(5.5);
    FixedType c = 10 - a;
    if (!approx((double)c, 4.5)) { FAIL("10-5.5=4.5"); return; }
    PASS();
}

TEST(mul_fixed_int) {
    FixedType a = FixedType::FromDouble(2.5);
    FixedType c = a * 4;
    if (!approx((double)c, 10.0, 1e-4)) { FAIL("2.5*4=10"); return; }
    PASS();
}

TEST(mul_int_fixed) {
    FixedType a = FixedType::FromDouble(2.5);
    FixedType c = 4 * a;
    if (!approx((double)c, 10.0, 1e-4)) { FAIL("4*2.5=10"); return; }
    PASS();
}

TEST(div_fixed_int) {
    FixedType a = FixedType::FromDouble(10.0);
    FixedType c = a / 2;
    if (!approx((double)c, 5.0, 1e-4)) { FAIL("10/2=5"); return; }
    PASS();
}

TEST(div_int_fixed) {
    FixedType a = FixedType::FromDouble(2.0);
    FixedType c = 10 / a;
    if (!approx((double)c, 5.0, 1e-4)) { FAIL("10/2=5"); return; }
    PASS();
}

// ============================================================================
// 7. 复合赋值测试
// ============================================================================

TEST(compound_add) {
    FixedType a = FixedType::FromDouble(5.0);
    FixedType b = FixedType::FromDouble(3.0);
    a += b;
    if (!approx((double)a, 8.0)) { FAIL("+= 5+3=8"); return; }
    PASS();
}

TEST(compound_sub) {
    FixedType a = FixedType::FromDouble(5.0);
    FixedType b = FixedType::FromDouble(3.0);
    a -= b;
    if (!approx((double)a, 2.0)) { FAIL("-= 5-3=2"); return; }
    PASS();
}

TEST(compound_mul) {
    FixedType a = FixedType::FromDouble(2.5);
    FixedType b = FixedType::FromDouble(4.0);
    a *= b;
    if (!approx((double)a, 10.0, 1e-4)) { FAIL("*= 2.5*4=10"); return; }
    PASS();
}

TEST(compound_div) {
    FixedType a = FixedType::FromDouble(10.0);
    FixedType b = FixedType::FromDouble(2.0);
    a /= b;
    if (!approx((double)a, 5.0, 1e-4)) { FAIL("/= 10/2=5"); return; }
    PASS();
}

TEST(compound_mod) {
    FixedType a = FixedType::FromDouble(10.0);
    FixedType b = FixedType::FromDouble(3.0);
    a %= b;
    if (!approx((double)a, 1.0, 1e-3)) { FAIL("%= 10%3=1"); return; }
    PASS();
}

// ============================================================================
// 8. 自增自减测试
// ============================================================================

TEST(pre_increment) {
    FixedType a = FixedType::FromDouble(5.0);
    FixedType b = ++a;
    if (!approx((double)a, 6.0)) { FAIL("++a not 6"); return; }
    if (!approx((double)b, 6.0)) { FAIL("result of ++a not 6"); return; }
    PASS();
}

TEST(post_increment) {
    FixedType a = FixedType::FromDouble(5.0);
    FixedType b = a++;
    if (!approx((double)a, 6.0)) { FAIL("a++ not 6"); return; }
    if (!approx((double)b, 5.0)) { FAIL("result of a++ not 5"); return; }
    PASS();
}

TEST(pre_decrement) {
    FixedType a = FixedType::FromDouble(5.0);
    FixedType b = --a;
    if (!approx((double)a, 4.0)) { FAIL("--a not 4"); return; }
    if (!approx((double)b, 4.0)) { FAIL("result of --a not 4"); return; }
    PASS();
}

TEST(post_decrement) {
    FixedType a = FixedType::FromDouble(5.0);
    FixedType b = a--;
    if (!approx((double)a, 4.0)) { FAIL("a-- not 4"); return; }
    if (!approx((double)b, 5.0)) { FAIL("result of a-- not 5"); return; }
    PASS();
}

// ============================================================================
// 9. 比较运算测试
// ============================================================================

TEST(cmp_eq_pos) {
    FixedType a = FixedType::FromDouble(3.5);
    FixedType b = FixedType::FromDouble(3.5);
    FixedType c = FixedType::FromDouble(3.6);
    if (!(a == b)) { FAIL("3.5==3.5"); return; }
    if (a == c) { FAIL("3.5!=3.6"); return; }
    PASS();
}

TEST(cmp_eq_neg) {
    FixedType a = FixedType::FromDouble(-3.5);
    FixedType b = FixedType::FromDouble(-3.5);
    if (!(a == b)) { FAIL("-3.5==-3.5"); return; }
    PASS();
}

TEST(cmp_ne) {
    FixedType a = FixedType::FromDouble(3.5);
    FixedType b = FixedType::FromDouble(4.5);
    if (!(a != b)) { FAIL("3.5!=4.5"); return; }
    if (a != a) { FAIL("3.5==3.5"); return; }
    PASS();
}

TEST(cmp_lt_pos) {
    FixedType a = FixedType::FromDouble(3.0);
    FixedType b = FixedType::FromDouble(5.0);
    if (!(a < b)) { FAIL("3<5"); return; }
    if (b < a) { FAIL("!(5<3)"); return; }
    PASS();
}

TEST(cmp_lt_neg) {
    FixedType a = FixedType::FromDouble(-5.0);
    FixedType b = FixedType::FromDouble(-3.0);
    if (!(a < b)) { FAIL("-5<-3"); return; }
    PASS();
}

TEST(cmp_gt) {
    FixedType a = FixedType::FromDouble(5.0);
    FixedType b = FixedType::FromDouble(3.0);
    if (!(a > b)) { FAIL("5>3"); return; }
    PASS();
}

TEST(cmp_le) {
    FixedType a = FixedType::FromDouble(3.0);
    FixedType b = FixedType::FromDouble(3.0);
    FixedType c = FixedType::FromDouble(5.0);
    if (!(a <= b)) { FAIL("3<=3"); return; }
    if (!(a <= c)) { FAIL("3<=5"); return; }
    if (c <= a) { FAIL("!(5<=3)"); return; }
    PASS();
}

TEST(cmp_ge) {
    FixedType a = FixedType::FromDouble(5.0);
    FixedType b = FixedType::FromDouble(5.0);
    FixedType c = FixedType::FromDouble(3.0);
    if (!(a >= b)) { FAIL("5>=5"); return; }
    if (!(a >= c)) { FAIL("5>=3"); return; }
    PASS();
}

TEST(cmp_mixed_sign) {
    FixedType pos = FixedType::FromDouble(1.0);
    FixedType neg = FixedType::FromDouble(-1.0);
    FixedType zero = FixedType::Zero();
    bool ok = true;
    if (!(pos > zero)) { ok = false; FAIL("1>0"); }
    if (!(neg < zero)) { ok = false; FAIL("-1<0"); }
    if (!(pos > neg)) { ok = false; FAIL("1>-1"); }
    if (ok) PASS();
}

// ============================================================================
// 10. abs 测试
// ============================================================================

TEST(abs_pos) {
    FixedType f = FixedType::FromDouble(5.5);
    FixedType a = f.abs();
    if (!approx((double)a, 5.5)) { FAIL("abs(5.5)"); return; }
    PASS();
}

TEST(abs_neg) {
    FixedType f = FixedType::FromDouble(-5.5);
    FixedType a = f.abs();
    if (!approx((double)a, 5.5)) { FAIL("abs(-5.5)"); return; }
    PASS();
}

TEST(abs_zero) {
    FixedType z = FixedType::Zero();
    FixedType a = z.abs();
    if (a.raw_ != 0) { FAIL("abs(0)"); return; }
    PASS();
}

// ============================================================================
// 11. 取整函数测试 (floor, ceil, round, trunc)
// ============================================================================

TEST(floor_pos) {
    FixedType f = FixedType::FromDouble(3.7);
    if (!approx((double)f.floor(), 3.0)) { FAIL("floor(3.7)=3"); return; }
    PASS();
}

TEST(floor_neg) {
    FixedType f = FixedType::FromDouble(-3.7);
    if (!approx((double)f.floor(), -4.0)) { FAIL("floor(-3.7)=-4"); return; }
    PASS();
}

TEST(floor_int) {
    FixedType f = FixedType::FromDouble(5.0);
    if (!approx((double)f.floor(), 5.0)) { FAIL("floor(5)=5"); return; }
    PASS();
}

TEST(ceil_pos) {
    FixedType f = FixedType::FromDouble(3.2);
    if (!approx((double)f.ceil(), 4.0)) { FAIL("ceil(3.2)=4"); return; }
    PASS();
}

TEST(ceil_neg) {
    FixedType f = FixedType::FromDouble(-3.2);
    if (!approx((double)f.ceil(), -3.0)) { FAIL("ceil(-3.2)=-3"); return; }
    PASS();
}

TEST(ceil_int) {
    FixedType f = FixedType::FromDouble(5.0);
    if (!approx((double)f.ceil(), 5.0)) { FAIL("ceil(5)=5"); return; }
    PASS();
}

TEST(round_pos_up) {
    FixedType f = FixedType::FromDouble(3.7);
    if (!approx((double)f.round(), 4.0)) { FAIL("round(3.7)=4"); return; }
    PASS();
}

TEST(round_pos_down) {
    FixedType f = FixedType::FromDouble(3.2);
    if (!approx((double)f.round(), 3.0)) { FAIL("round(3.2)=3"); return; }
    PASS();
}

TEST(round_neg_up) {
    FixedType f = FixedType::FromDouble(-3.7);
    if (!approx((double)f.round(), -4.0)) { FAIL("round(-3.7)=-4"); return; }
    PASS();
}

TEST(round_neg_down) {
    FixedType f = FixedType::FromDouble(-3.2);
    if (!approx((double)f.round(), -3.0)) { FAIL("round(-3.2)=-3"); return; }
    PASS();
}

TEST(trunc_pos) {
    FixedType f = FixedType::FromDouble(3.9);
    if (!approx((double)f.trunc(), 3.0)) { FAIL("trunc(3.9)=3"); return; }
    PASS();
}

TEST(trunc_neg) {
    FixedType f = FixedType::FromDouble(-3.9);
    if (!approx((double)f.trunc(), -3.0)) { FAIL("trunc(-3.9)=-3"); return; }
    PASS();
}

TEST(trunc_int) {
    FixedType f = FixedType::FromDouble(5.0);
    if (!approx((double)f.trunc(), 5.0)) { FAIL("trunc(5)=5"); return; }
    PASS();
}

// ============================================================================
// 12. math.hpp 函数测试
// ============================================================================

TEST(math_pow_pos_exp) {
    FixedType base = FixedType::FromDouble(2.0);
    FixedType r = pow(base, 10);
    if (!approx((double)r, 1024.0, 1e-3)) { FAIL("pow(2,10)=1024"); return; }
    PASS();
}

TEST(math_pow_zero_exp) {
    FixedType base = FixedType::FromDouble(5.0);
    FixedType r = pow(base, 0);
    if (!approx((double)r, 1.0, 1e-4)) { FAIL("pow(5,0)=1"); return; }
    PASS();
}

TEST(math_pow_one_exp) {
    FixedType base = FixedType::FromDouble(3.5);
    FixedType r = pow(base, 1);
    if (!approx((double)r, 3.5, 1e-4)) { FAIL("pow(3.5,1)=3.5"); return; }
    PASS();
}

TEST(math_pow_neg_exp) {
    FixedType base = FixedType::FromDouble(2.0);
    FixedType r = pow(base, -2);
    // 2^(-2) = 1/4 = 0.25
    if (!approx((double)r, 0.25, 1e-3)) { FAIL("pow(2,-2)=0.25"); return; }
    PASS();
}

TEST(math_pow_frac_base) {
    FixedType base = FixedType::FromDouble(0.5);
    FixedType r = pow(base, 3);
    // 0.5^3 = 0.125
    if (!approx((double)r, 0.125, 1e-3)) { FAIL("pow(0.5,3)=0.125"); return; }
    PASS();
}

TEST(math_sqrt_perfect) {
    FixedType x = FixedType::FromDouble(16.0);
    FixedType r = sqrt(x, 10);
    if (!approx((double)r, 4.0, 1e-3)) { FAIL("sqrt(16)=4"); return; }
    PASS();
}

TEST(math_sqrt_non_perfect) {
    FixedType x = FixedType::FromDouble(2.0);
    FixedType r = sqrt(x, 10);
    if (!approx((double)r, 1.41421356, 1e-3)) { FAIL("sqrt(2)=1.414"); return; }
    PASS();
}

TEST(math_sqrt_one) {
    FixedType x = FixedType::FromDouble(1.0);
    FixedType r = sqrt(x, 10);
    if (!approx((double)r, 1.0, 1e-4)) { FAIL("sqrt(1)=1"); return; }
    PASS();
}

TEST(math_sqrt_zero) {
    FixedType x = FixedType::FromDouble(0.0);
    FixedType r = sqrt(x, 10);
    if (!approx((double)r, 0.0, 1e-10)) { FAIL("sqrt(0)=0"); return; }
    PASS();
}

TEST(math_sqrt_small) {
    FixedType x = FixedType::FromDouble(0.25);
    FixedType r = sqrt(x, 10);
    if (!approx((double)r, 0.5, 1e-3)) { FAIL("sqrt(0.25)=0.5"); return; }
    PASS();
}

TEST(math_sqrt_large_default_iterations) {
    FixedType x(100000000);
    FixedType r = sqrt(x);
    if (!approx((double)r, 10000.0, 1e-3)) { FAIL("sqrt(100000000)=10000"); return; }
    PASS();
}

TEST(math_sqrt_tiny_default_iterations) {
    FixedType x = FixedType::FromDouble(0.0001);
    FixedType r = sqrt(x);
    if (!approx((double)r, 0.01, 1e-5)) { FAIL("sqrt(0.0001)=0.01"); return; }
    PASS();
}

TEST(math_abs_free_func) {
    FixedType f = FixedType::FromDouble(-7.5);
    FixedType r = abs(f);
    if (!approx((double)r, 7.5)) { FAIL("abs(-7.5)=7.5"); return; }
    PASS();
}

TEST(math_floor_free_func) {
    FixedType f = FixedType::FromDouble(3.7);
    FixedType r = floor(f);
    if (!approx((double)r, 3.0)) { FAIL("free floor(3.7)=3"); return; }
    PASS();
}

TEST(math_ceil_free_func) {
    FixedType f = FixedType::FromDouble(3.2);
    FixedType r = ceil(f);
    if (!approx((double)r, 4.0)) { FAIL("free ceil(3.2)=4"); return; }
    PASS();
}

TEST(math_round_free_func) {
    FixedType f = FixedType::FromDouble(3.7);
    FixedType r = round(f);
    if (!approx((double)r, 4.0)) { FAIL("free round(3.7)=4"); return; }
    PASS();
}

TEST(math_atan2_basic) {
    // atan2(1, 1) = π/4
    FixedType y = FixedType::FromDouble(1.0);
    FixedType x = FixedType::FromDouble(1.0);
    FixedType r = atan2(y, x);
    if (!approx((double)r, 0.785398, 1e-3)) { FAIL("atan2(1,1)=pi/4"); return; }
    PASS();
}

TEST(math_atan2_y_axis) {
    // atan2(1, 0) = π/2
    FixedType y = FixedType::FromDouble(1.0);
    FixedType x = FixedType::FromDouble(0.0);
    FixedType r = atan2(y, x);
    if (!approx((double)r, 1.570796, 1e-3)) { FAIL("atan2(1,0)=pi/2"); return; }
    PASS();
}

TEST(math_atan2_neg_y_axis) {
    // atan2(-1, 0) = -π/2
    FixedType y = FixedType::FromDouble(-1.0);
    FixedType x = FixedType::FromDouble(0.0);
    FixedType r = atan2(y, x);
    if (!approx((double)r, -1.570796, 1e-3)) { FAIL("atan2(-1,0)=-pi/2"); return; }
    PASS();
}

TEST(math_atan2_neg_x) {
    // atan2(0, -1) = π
    FixedType y = FixedType::FromDouble(0.0);
    FixedType x = FixedType::FromDouble(-1.0);
    FixedType r = atan2(y, x);
    if (!approx(std::fabs((double)r), 3.141593, 1e-3)) { FAIL("atan2(0,-1)=pi"); return; }
    PASS();
}

TEST(math_atan2_origin) {
    // atan2(0, 0) = 0
    FixedType y = FixedType::Zero();
    FixedType x = FixedType::Zero();
    FixedType r = atan2(y, x);
    if (!approx((double)r, 0.0, 1e-10)) { FAIL("atan2(0,0)=0"); return; }
    PASS();
}

TEST(math_atan2_quadrant3) {
    // atan2(-1, -1) = -3π/4
    FixedType y = FixedType::FromDouble(-1.0);
    FixedType x = FixedType::FromDouble(-1.0);
    FixedType r = atan2(y, x);
    if (!approx((double)r, -2.356194, 1e-3)) { FAIL("atan2(-1,-1)=-3pi/4"); return; }
    PASS();
}

// ============================================================================
// 13. 边界值与大数测试
// ============================================================================

TEST(large_add) {
    FixedType a = FixedType::FromDouble(1000000.0);
    FixedType b = FixedType::FromDouble(0.00001);
    FixedType c = a + b;
    if (!approx((double)c, 1000000.00001, 1e-4)) { FAIL("large+small"); return; }
    PASS();
}

TEST(large_mul) {
    FixedType a = FixedType::FromDouble(10000.0);
    FixedType b = FixedType::FromDouble(10000.0);
    FixedType c = a * b;
    if (!approx((double)c, 100000000.0, 1.0)) { FAIL("10000*10000"); return; }
    PASS();
}

TEST(neg_large) {
    FixedType a = FixedType::FromDouble(-1000000.0);
    FixedType b = FixedType::FromDouble(-0.5);
    FixedType c = a + b;
    if (!approx((double)c, -1000000.5, 1e-3)) { FAIL("neg large + neg small"); return; }
    PASS();
}

TEST(very_small) {
    FixedType a = FixedType::FromDouble(0.0001);
    FixedType b = FixedType::FromDouble(0.0001);
    FixedType c = a + b;
    if (!approx((double)c, 0.0002, 1e-5)) { FAIL("very small add"); return; }
    PASS();
}

TEST(zero_add_neg) {
    FixedType z = FixedType::Zero();
    FixedType n = FixedType::FromDouble(-5.5);
    FixedType c = z + n;
    if (!approx((double)c, -5.5)) { FAIL("0+(-5.5)"); return; }
    PASS();
}

TEST(neg_times_zero) {
    FixedType n = FixedType::FromDouble(-123.456);
    FixedType z = FixedType::Zero();
    FixedType c = n * z;
    if (c.raw_ != 0) { FAIL("-123.456*0=0"); return; }
    PASS();
}

// ============================================================================
// 14. FromString 运行时解析测试
// ============================================================================

TEST(fromstring_positive) {
    FixedType f = FixedType::FromString("123.456");
    if (!approx((double)f, 123.456, 1e-3)) { FAIL("FromString 123.456"); return; }
    PASS();
}

TEST(fromstring_negative) {
    FixedType f = FixedType::FromString("-99.99");
    if (!approx((double)f, -99.99, 1e-3)) { FAIL("FromString -99.99"); return; }
    PASS();
}

TEST(fromstring_integer_only) {
    FixedType f = FixedType::FromString("42");
    if (!approx((double)f, 42.0)) { FAIL("FromString 42"); return; }
    PASS();
}

TEST(fromstring_zero) {
    FixedType f = FixedType::FromString("0");
    if (!approx((double)f, 0.0)) { FAIL("FromString 0"); return; }
    PASS();
}

TEST(fromstring_with_plus) {
    FixedType f = FixedType::FromString("+7.5");
    if (!approx((double)f, 7.5)) { FAIL("FromString +7.5"); return; }
    PASS();
}

// ============================================================================
// 15. 复杂表达式测试
// ============================================================================

TEST(expr_combined) {
    // (a + b) * c / d - e
    FixedType a = FixedType::FromDouble(10.0);
    FixedType b = FixedType::FromDouble(5.0);
    FixedType c = FixedType::FromDouble(2.0);
    FixedType d = FixedType::FromDouble(3.0);
    FixedType e = FixedType::FromDouble(4.0);
    FixedType r = (a + b) * c / d - e;
    // (10+5)*2/3 - 4 = 15*2/3 - 4 = 30/3 - 4 = 10 - 4 = 6
    if (!approx((double)r, 6.0, 1e-3)) { FAIL("(10+5)*2/3-4=6"); return; }
    PASS();
}

TEST(expr_nested_neg) {
    // -((-a) + (-b)) where a=3, b=4 => -((-3)+(-4)) = -(-7) = 7
    FixedType a = FixedType::FromDouble(3.0);
    FixedType b = FixedType::FromDouble(4.0);
    FixedType r = -((-a) + (-b));
    if (!approx((double)r, 7.0)) { FAIL("-((-3)+(-4))=7"); return; }
    PASS();
}

// ============================================================================
// Main
// ============================================================================

int main() {
    printf("========================================\n");
    printf("  Fixed-Point Library Test Suite\n");
    printf("  Type: Fixed<Int128, int64_t, 32>\n");
    printf("========================================\n\n");

    printf("--- Construction ---\n");
    RUN(constructor_default);
    RUN(constructor_int);
    RUN(constructor_double);
    RUN(constructor_float);
    RUN(constructor_fromraw);
    RUN(constructor_string_pos);
    RUN(constructor_string_neg);
    RUN(constructor_string_int);
    RUN(constructor_string_zero);
    RUN(constructor_large_int);
    RUN(constructor_small_fraction);

    printf("\n--- Type Conversion ---\n");
    RUN(conv_to_int);
    RUN(conv_to_int64);
    RUN(conv_to_char);
    RUN(conv_to_uchar);
    RUN(conv_to_double_pos);
    RUN(conv_to_double_neg);
    RUN(conv_to_float);

    printf("\n--- Constants ---\n");
    RUN(constant_zero);
    RUN(constant_pi);
    RUN(constant_e);
    RUN(constant_epsilon);

    printf("\n--- Unary Operators ---\n");
    RUN(unary_plus);
    RUN(unary_minus_pos);
    RUN(unary_minus_neg);
    RUN(unary_minus_zero);

    printf("\n--- Arithmetic (Fixed OP Fixed) ---\n");
    RUN(add_pos_pos);
    RUN(add_neg_neg);
    RUN(add_pos_neg);
    RUN(sub_pos_pos);
    RUN(sub_neg_neg);
    RUN(mul_pos_pos);
    RUN(mul_neg_pos);
    RUN(mul_neg_neg);
    RUN(mul_small_frac);
    RUN(div_pos_pos);
    RUN(div_neg_pos);
    RUN(div_neg_neg);
    RUN(mod_pos);
    RUN(mod_neg);

    printf("\n--- Arithmetic (Fixed OP Int / Int OP Fixed) ---\n");
    RUN(add_fixed_int);
    RUN(add_int_fixed);
    RUN(sub_fixed_int);
    RUN(sub_int_fixed);
    RUN(mul_fixed_int);
    RUN(mul_int_fixed);
    RUN(div_fixed_int);
    RUN(div_int_fixed);

    printf("\n--- Compound Assignment ---\n");
    RUN(compound_add);
    RUN(compound_sub);
    RUN(compound_mul);
    RUN(compound_div);
    RUN(compound_mod);

    printf("\n--- Increment / Decrement ---\n");
    RUN(pre_increment);
    RUN(post_increment);
    RUN(pre_decrement);
    RUN(post_decrement);

    printf("\n--- Comparison ---\n");
    RUN(cmp_eq_pos);
    RUN(cmp_eq_neg);
    RUN(cmp_ne);
    RUN(cmp_lt_pos);
    RUN(cmp_lt_neg);
    RUN(cmp_gt);
    RUN(cmp_le);
    RUN(cmp_ge);
    RUN(cmp_mixed_sign);

    printf("\n--- Absolute Value ---\n");
    RUN(abs_pos);
    RUN(abs_neg);
    RUN(abs_zero);

    printf("\n--- Rounding Functions ---\n");
    RUN(floor_pos);
    RUN(floor_neg);
    RUN(floor_int);
    RUN(ceil_pos);
    RUN(ceil_neg);
    RUN(ceil_int);
    RUN(round_pos_up);
    RUN(round_pos_down);
    RUN(round_neg_up);
    RUN(round_neg_down);
    RUN(trunc_pos);
    RUN(trunc_neg);
    RUN(trunc_int);

    printf("\n--- Math Functions ---\n");
    RUN(math_pow_pos_exp);
    RUN(math_pow_zero_exp);
    RUN(math_pow_one_exp);
    RUN(math_pow_neg_exp);
    RUN(math_pow_frac_base);
    RUN(math_sqrt_perfect);
    RUN(math_sqrt_non_perfect);
    RUN(math_sqrt_one);
    RUN(math_sqrt_zero);
    RUN(math_sqrt_small);
    RUN(math_sqrt_large_default_iterations);
    RUN(math_sqrt_tiny_default_iterations);
    RUN(math_abs_free_func);
    RUN(math_floor_free_func);
    RUN(math_ceil_free_func);
    RUN(math_round_free_func);
    RUN(math_atan2_basic);
    RUN(math_atan2_y_axis);
    RUN(math_atan2_neg_y_axis);
    RUN(math_atan2_neg_x);
    RUN(math_atan2_origin);
    RUN(math_atan2_quadrant3);

    printf("\n--- Boundary & Large Values ---\n");
    RUN(large_add);
    RUN(large_mul);
    RUN(neg_large);
    RUN(very_small);
    RUN(zero_add_neg);
    RUN(neg_times_zero);

    printf("\n--- FromString Runtime ---\n");
    RUN(fromstring_positive);
    RUN(fromstring_negative);
    RUN(fromstring_integer_only);
    RUN(fromstring_zero);
    RUN(fromstring_with_plus);

    printf("\n--- Complex Expressions ---\n");
    RUN(expr_combined);
    RUN(expr_nested_neg);

    printf("\n========================================\n");
    printf("  Results: %d passed, %d failed, %d total\n",
           g_pass, g_fail, g_pass + g_fail);
    printf("========================================\n");

    return g_fail > 0 ? 1 : 0;
}
