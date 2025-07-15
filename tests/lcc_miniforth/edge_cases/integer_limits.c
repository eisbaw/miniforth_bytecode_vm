/* Integer limits and overflow behavior tests */

int test_integer_overflow() {
  int max_positive = 32767; /* Typical 16-bit signed max */
  int near_overflow = max_positive - 1;

  /* Test arithmetic near limits */
  int result1 = near_overflow + 1; /* Should equal max_positive */
  int result2 = max_positive + 1;  /* May overflow to negative */

  /* Test with smaller known values to avoid undefined behavior */
  int safe_max = 1000;
  int safe_result = safe_max + safe_max; /* Expected: 2000 */

  return safe_result;
}

int test_division_edge_cases() {
  int result = 0;

  /* Division by various values */
  result += 100 / 10;    /* Expected: 10 */
  result += 100 / 3;     /* Expected: 33 (integer division) */
  result += 1 / 2;       /* Expected: 0 (integer division) */
  result += (-100) / 10; /* Expected: -10 */

  /* Avoid division by zero - undefined behavior */

  return result; /* Expected: 33 */
}

int test_modulo_edge_cases() {
  int result = 0;

  result += 100 % 10; /* Expected: 0 */
  result += 100 % 3;  /* Expected: 1 */
  result += 7 % 2;    /* Expected: 1 */
  result += 15 % 4;   /* Expected: 3 */

  return result; /* Expected: 5 */
}

int test_zero_operations() {
  int result = 0;

  result += 0 + 5;   /* Expected: 5 */
  result += 0 * 100; /* Expected: 0 */
  result += 10 - 0;  /* Expected: 10 */
  result += 0 - 5;   /* Expected: -5 */

  return result; /* Expected: 10 */
}

int main() {
  int t1 = test_integer_overflow();    /* Expected: 2000 */
  int t2 = test_division_edge_cases(); /* Expected: 33 */
  int t3 = test_modulo_edge_cases();   /* Expected: 5 */
  int t4 = test_zero_operations();     /* Expected: 10 */

  return t1 / 100; /* Expected: 20 */
}