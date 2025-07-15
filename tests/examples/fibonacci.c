/*
 * Fibonacci example program
 * This will be used to test the C-to-Forth compiler
 */

int fibonacci(int n) {
  if (n <= 1) {
    return n;
  }
  return fibonacci(n - 1) + fibonacci(n - 2);
}

int main() {
  return fibonacci(6); // Should return 8
}