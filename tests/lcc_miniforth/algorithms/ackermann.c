/* Ackermann function - Extremely recursive stress test */

int ackermann(int m, int n) {
  if (m == 0) {
    return n + 1;
  }
  if (n == 0) {
    return ackermann(m - 1, 1);
  }
  return ackermann(m - 1, ackermann(m, n - 1));
}

int test_ackermann_small() {
  return ackermann(2, 2); // Expected: 7
}

int test_ackermann_medium() {
  return ackermann(3, 2); // Expected: 29
}

int main() {
  /* Start with smaller values - Ackermann grows extremely fast */
  return test_ackermann_small(); // Expected: 7
}