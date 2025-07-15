/* Test recursive functions */

int factorial(int n) {
  if (n <= 1) {
    return 1;
  }
  return n * factorial(n - 1);
}

int fibonacci(int n) {
  if (n <= 1) {
    return n;
  }
  return fibonacci(n - 1) + fibonacci(n - 2);
}

int countdown(int n) {
  if (n <= 0) {
    return 0;
  }
  return n + countdown(n - 1);
}

int gcd(int a, int b) {
  if (b == 0) {
    return a;
  }
  return gcd(b, a % b);
}

int power(int base, int exp) {
  if (exp == 0) {
    return 1;
  }
  return base * power(base, exp - 1);
}

int main() {
  return factorial(5); // Expected: 120
}