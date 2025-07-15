/* Fibonacci sequence - Tests recursive and iterative performance */

int fibonacci_recursive(int n) {
  if (n <= 1) {
    return n;
  }
  return fibonacci_recursive(n - 1) + fibonacci_recursive(n - 2);
}

int fibonacci_iterative(int n) {
  int a = 0, b = 1, temp;
  int i;

  if (n <= 1) {
    return n;
  }

  for (i = 2; i <= n; i++) {
    temp = a + b;
    a = b;
    b = temp;
  }

  return b;
}

int test_fibonacci_comparison() {
  int n = 15;
  int recursive_result = fibonacci_recursive(n); /* Expected: 610 */
  int iterative_result = fibonacci_iterative(n); /* Expected: 610 */

  /* Verify both methods give same result */
  if (recursive_result == iterative_result) {
    return recursive_result;
  } else {
    return -1; /* Error indicator */
  }
}

int main() { return test_fibonacci_comparison(); /* Expected: 610 */ }