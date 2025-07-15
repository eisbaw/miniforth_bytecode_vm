/* Sieve of Eratosthenes - Tests array manipulation and loops */

int sieve_of_eratosthenes(int limit) {
    int is_prime[100];  /* Array to track prime numbers up to 100 */
    int i, j;
    int prime_count = 0;
    
    /* Initialize all numbers as potentially prime */
    for (i = 2; i <= limit; i++) {
        is_prime[i] = 1;
    }
    
    /* Sieve algorithm */
    for (i = 2; i * i <= limit; i++) {
        if (is_prime[i]) {
            /* Mark multiples of i as not prime */
            for (j = i * i; j <= limit; j += i) {
                is_prime[j] = 0;
            }
        }
    }
    
    /* Count primes */
    for (i = 2; i <= limit; i++) {
        if (is_prime[i]) {
            prime_count++;
        }
    }
    
    return prime_count;
}

int test_prime_factorization(int n) {
    int factors = 0;
    int i;
    
    /* Count prime factors */
    for (i = 2; i * i <= n; i++) {
        while (n % i == 0) {
            factors++;
            n = n / i;
        }
    }
    
    if (n > 1) {
        factors++;
    }
    
    return factors;
}

int main() {
    int primes_up_to_50 = sieve_of_eratosthenes(50);    /* Expected: 15 primes */
    int factors_of_60 = test_prime_factorization(60);    /* Expected: 6 factors (2,2,3,5) */
    
    return primes_up_to_50;  /* Expected: 15 */
}