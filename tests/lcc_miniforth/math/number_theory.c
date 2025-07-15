/* Number theory and mathematical algorithms */

int gcd(int a, int b) {
    /* Euclidean algorithm for greatest common divisor */
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

int lcm(int a, int b) {
    /* Least common multiple using GCD */
    return (a * b) / gcd(a, b);
}

int power_mod(int base, int exp, int mod) {
    /* Fast modular exponentiation */
    int result = 1;
    base = base % mod;
    
    while (exp > 0) {
        if (exp % 2 == 1) {
            result = (result * base) % mod;
        }
        exp = exp / 2;
        base = (base * base) % mod;
    }
    
    return result;
}

int is_prime_simple(int n) {
    /* Simple primality test */
    int i;
    
    if (n <= 1) return 0;
    if (n <= 3) return 1;
    if (n % 2 == 0 || n % 3 == 0) return 0;
    
    for (i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) {
            return 0;
        }
    }
    
    return 1;
}

int count_primes_up_to(int limit) {
    int count = 0;
    int i;
    
    for (i = 2; i <= limit; i++) {
        if (is_prime_simple(i)) {
            count++;
        }
    }
    
    return count;
}

int fibonacci_mod(int n, int mod) {
    /* Fibonacci sequence with modular arithmetic */
    int a = 0, b = 1, temp;
    int i;
    
    if (n == 0) return 0;
    if (n == 1) return 1;
    
    for (i = 2; i <= n; i++) {
        temp = (a + b) % mod;
        a = b;
        b = temp;
    }
    
    return b;
}

int test_number_theory_basics() {
    int gcd_48_18 = gcd(48, 18);        /* Expected: 6 */
    int lcm_12_8 = lcm(12, 8);          /* Expected: 24 */
    int gcd_17_13 = gcd(17, 13);        /* Expected: 1 (coprime) */
    
    return gcd_48_18 + lcm_12_8 + gcd_17_13;  /* Expected: 6 + 24 + 1 = 31 */
}

int test_modular_arithmetic() {
    int pow_2_10_mod_1000 = power_mod(2, 10, 1000);    /* 2^10 = 1024, mod 1000 = 24 */
    int pow_3_5_mod_7 = power_mod(3, 5, 7);            /* 3^5 = 243, mod 7 = 5 */
    int pow_5_3_mod_13 = power_mod(5, 3, 13);          /* 5^3 = 125, mod 13 = 8 */
    
    return pow_2_10_mod_1000 + pow_3_5_mod_7 + pow_5_3_mod_13;  /* Expected: 24 + 5 + 8 = 37 */
}

int test_prime_functions() {
    int prime_count_20 = count_primes_up_to(20);       /* Primes: 2,3,5,7,11,13,17,19 = 8 */
    int is_17_prime = is_prime_simple(17);             /* Expected: 1 (true) */
    int is_15_prime = is_prime_simple(15);             /* Expected: 0 (false) */
    int is_2_prime = is_prime_simple(2);               /* Expected: 1 (true) */
    
    return prime_count_20 + is_17_prime + is_15_prime + is_2_prime;  /* Expected: 8 + 1 + 0 + 1 = 10 */
}

int test_fibonacci_modular() {
    int fib_10_mod_100 = fibonacci_mod(10, 100);       /* F(10) = 55, mod 100 = 55 */
    int fib_15_mod_1000 = fibonacci_mod(15, 1000);     /* F(15) = 610, mod 1000 = 610 */
    int fib_12_mod_50 = fibonacci_mod(12, 50);         /* F(12) = 144, mod 50 = 44 */
    
    return fib_10_mod_100 + fib_12_mod_50;  /* Expected: 55 + 44 = 99 */
}

int test_extended_gcd_simulation() {
    /* Simulate extended Euclidean algorithm results */
    int a = 35, b = 15;
    int gcd_result = gcd(a, b);  /* Should be 5 */
    
    /* Manual verification: 35 = 2*15 + 5, 15 = 3*5 + 0 */
    /* So gcd(35,15) = 5, and 5 = 35 - 2*15 */
    /* Check: 35*1 + 15*(-2) = 35 - 30 = 5 */
    int verification = a * 1 + b * (-2);
    
    int result_matches = (verification == gcd_result) ? 1 : 0;
    
    return gcd_result * 10 + result_matches;  /* Expected: 5*10 + 1 = 51 */
}

int main() {
    int test1 = test_number_theory_basics();    /* Expected: 31 */
    int test2 = test_modular_arithmetic();      /* Expected: 37 */
    int test3 = test_prime_functions();         /* Expected: 10 */
    int test4 = test_fibonacci_modular();       /* Expected: 99 */
    int test5 = test_extended_gcd_simulation(); /* Expected: 51 */
    
    return test3;  /* Expected: 10 */
}