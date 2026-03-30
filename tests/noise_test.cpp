#include "FastNoise.h"
#include <iostream>
#include <cmath>
#include <cstdlib>
using namespace std;

static int failures = 0;

static void check(bool cond, const char* msg) {
    if (!cond) {
        cerr << "FAIL: " << msg << "\n";
        failures++;
    }
}

// Value, Gradient, Simplex are bounded to [-1, 1].
// WhiteNoise returns raw hash output and is NOT bounded — excluded from range checks.
// Fractal types use approximate bounding and may slightly exceed [-1, 1].
static void test_range() {
    FastNoise fn(42);
    fn.SetFrequency(0.1f);

    float bounded_2d[] = {
        fn.GetValue(1.0f, 2.0f),
        fn.GetGradient(1.0f, 2.0f),
        fn.GetSimplex(1.0f, 2.0f),
    };
    for (float v : bounded_2d) {
        check(v >= -1.0f && v <= 1.0f, "2D noise out of [-1, 1] range");
    }

    float bounded_3d[] = {
        fn.GetValue(1.0f, 2.0f, 3.0f),
        fn.GetGradient(1.0f, 2.0f, 3.0f),
        fn.GetSimplex(1.0f, 2.0f, 3.0f),
    };
    for (float v : bounded_3d) {
        check(v >= -1.0f && v <= 1.0f, "3D base noise out of [-1, 1] range");
    }

    // Fractal types use approximate bounding — allow slight overshoot
    float fractal[] = {
        fn.GetValueFractal(1.0f, 2.0f),
        fn.GetGradientFractal(1.0f, 2.0f),
        fn.GetSimplexFractal(1.0f, 2.0f),
        fn.GetValueFractal(1.0f, 2.0f, 3.0f),
        fn.GetGradientFractal(1.0f, 2.0f, 3.0f),
        fn.GetSimplexFractal(1.0f, 2.0f, 3.0f),
    };
    for (float v : fractal) {
        check(v >= -1.1f && v <= 1.1f, "Fractal noise far out of expected range");
    }
}

// Same seed + same input must always return the same value
static void test_determinism() {
    FastNoise a(100), b(100);
    a.SetFrequency(0.05f);
    b.SetFrequency(0.05f);

    check(a.GetSimplex(3.0f, 7.0f)       == b.GetSimplex(3.0f, 7.0f),       "Simplex 2D not deterministic");
    check(a.GetSimplex(3.0f, 7.0f, 2.0f) == b.GetSimplex(3.0f, 7.0f, 2.0f), "Simplex 3D not deterministic");
    check(a.GetValue(3.0f, 7.0f)         == b.GetValue(3.0f, 7.0f),          "Value 2D not deterministic");
    check(a.GetCellular(3.0f, 7.0f)      == b.GetCellular(3.0f, 7.0f),       "Cellular 2D not deterministic");
}

// Different seeds should produce different output
static void test_seed_independence() {
    FastNoise a(1), b(2);
    check(a.GetSimplex(5.0f, 5.0f) != b.GetSimplex(5.0f, 5.0f), "Different seeds produced identical Simplex output");
    check(a.GetValue(5.0f, 5.0f)   != b.GetValue(5.0f, 5.0f),   "Different seeds produced identical Value output");
}

// Nearby points should have nearby values (continuity check)
static void test_continuity() {
    FastNoise fn(42);
    fn.SetFrequency(0.1f);

    float v0 = fn.GetSimplex(0.0f, 0.0f);
    float v1 = fn.GetSimplex(0.001f, 0.0f);
    check(fabs(v0 - v1) < 0.1f, "Simplex not continuous: large jump between nearby points");

    float v2 = fn.GetValue(0.0f, 0.0f);
    float v3 = fn.GetValue(0.001f, 0.0f);
    check(fabs(v2 - v3) < 0.1f, "Value noise not continuous: large jump between nearby points");
}

// Sweep many points and verify range holds broadly
static void test_range_sweep() {
    FastNoise fn(999);
    fn.SetFrequency(0.1f);

    for (int i = 0; i < 200; i++) {
        float x = (float)(i - 100) * 0.5f;
        for (int j = 0; j < 200; j++) {
            float y = (float)(j - 100) * 0.5f;
            float v = fn.GetSimplex(x, y);
            if (v < -1.0f || v > 1.0f) {
                check(false, "Simplex out of range in sweep");
                return;
            }
        }
    }
    check(true, "Simplex range sweep passed");
}

int main() {
    test_range();
    test_determinism();
    test_seed_independence();
    test_continuity();
    test_range_sweep();

    if (failures == 0) {
        cout << "All noise tests passed.\n";
        return 0;
    } else {
        cerr << failures << " test(s) failed.\n";
        return 1;
    }
}
