/* Fixed-point arithmetic simulation (since we don't have real floats) */

/* Fixed-point representation: 16 bits integer, 16 bits fractional */
/* Value = integer_part + (fractional_part / 65536) */

struct FixedPoint {
    int integer_part;
    int fractional_part;  /* 0-65535 representing 0.0-0.999... */
};

struct FixedPoint fixed_create(int integer, int frac_numerator, int frac_denominator) {
    struct FixedPoint result;
    result.integer_part = integer;
    /* Convert fraction to fixed-point representation */
    result.fractional_part = (frac_numerator * 65536) / frac_denominator;
    if (result.fractional_part >= 65536) {
        result.integer_part += result.fractional_part / 65536;
        result.fractional_part = result.fractional_part % 65536;
    }
    return result;
}

struct FixedPoint fixed_add(struct FixedPoint a, struct FixedPoint b) {
    struct FixedPoint result;
    result.fractional_part = a.fractional_part + b.fractional_part;
    result.integer_part = a.integer_part + b.integer_part;
    
    if (result.fractional_part >= 65536) {
        result.integer_part++;
        result.fractional_part -= 65536;
    }
    
    return result;
}

struct FixedPoint fixed_multiply(struct FixedPoint a, struct FixedPoint b) {
    struct FixedPoint result;
    
    /* Simplified multiplication - not fully accurate but demonstrates concept */
    int total_a = a.integer_part * 65536 + a.fractional_part;
    int total_b = b.integer_part * 65536 + b.fractional_part;
    
    /* Divide by 65536 to maintain fixed-point scale */
    int total_result = (total_a * total_b) / 65536;
    
    result.integer_part = total_result / 65536;
    result.fractional_part = total_result % 65536;
    
    return result;
}

int fixed_to_scaled_int(struct FixedPoint fp, int scale) {
    /* Convert to integer scaled by given factor for easier testing */
    return fp.integer_part * scale + (fp.fractional_part * scale) / 65536;
}

int test_fixed_point_arithmetic() {
    /* Create 2.5 and 3.25 */
    struct FixedPoint a = fixed_create(2, 1, 2);    /* 2.5 */
    struct FixedPoint b = fixed_create(3, 1, 4);    /* 3.25 */
    
    /* Test addition: 2.5 + 3.25 = 5.75 */
    struct FixedPoint sum = fixed_add(a, b);
    int sum_scaled = fixed_to_scaled_int(sum, 100);  /* Scale by 100: should be 575 */
    
    /* Test multiplication: 2.5 * 3.25 = 8.125 */
    struct FixedPoint product = fixed_multiply(a, b);
    int product_scaled = fixed_to_scaled_int(product, 100);  /* Should be ~812 */
    
    return sum_scaled;  /* Expected: 575 */
}

int test_integer_math_precision() {
    /* Test precise integer operations that simulate floating point */
    
    /* Calculate area of circle with radius 5 using integer approximation */
    /* π ≈ 22/7, Area = π * r² */
    int radius = 5;
    int radius_squared = radius * radius;  /* 25 */
    int pi_numerator = 22;
    int pi_denominator = 7;
    
    /* Area = (22 * 25) / 7 = 550 / 7 ≈ 78.57, scaled by 100 = 7857 */
    int area_scaled = (pi_numerator * radius_squared * 100) / pi_denominator;
    
    return area_scaled;  /* Expected: ~7857 */
}

int test_trigonometry_approximation() {
    /* Simple trigonometry using lookup tables and linear interpolation */
    
    /* Sine approximation using Taylor series for small angles */
    /* sin(x) ≈ x - x³/6 + x⁵/120 for small x */
    /* Using x = 0.5 radians, scaled by 1000 for integer math */
    
    int x_scaled = 500;  /* 0.5 * 1000 */
    int x_cubed = (x_scaled * x_scaled * x_scaled) / (1000 * 1000);  /* x³ */
    int x_fifth = (x_cubed * x_scaled * x_scaled) / (1000 * 1000);   /* x⁵ */
    
    /* sin(0.5) ≈ 0.5 - 0.125/6 + very_small_term */
    int sin_approx = x_scaled - x_cubed / 6 + x_fifth / 120;
    
    return sin_approx;  /* Should be close to sin(0.5) * 1000 ≈ 479 */
}

int main() {
    int test1 = test_fixed_point_arithmetic();      /* Expected: 575 */
    int test2 = test_integer_math_precision();      /* Expected: ~7857 */
    int test3 = test_trigonometry_approximation();  /* Expected: ~479 */
    
    return test1;  /* Expected: 575 */
}