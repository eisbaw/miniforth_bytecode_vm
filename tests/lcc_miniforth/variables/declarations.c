/* Test variable declarations and assignments */

int global_var = 100;

int test_local_variables() {
  int x = 5;
  int y = 10;
  int z = x + y;
  return z; // Expected: 15
}

int test_global_access() {
  return global_var; // Expected: 100
}

int test_variable_assignment() {
  int x = 5;
  x = 10;
  x = x + 3;
  return x; // Expected: 13
}

int test_multiple_assignments() {
  int a, b, c;
  a = 1;
  b = 2;
  c = 3;
  return a + b + c; // Expected: 6
}

int test_assignment_in_expression() {
  int x = 5;
  int y = (x = 10) + 5;
  return y; // Expected: 15 (x becomes 10, then 10+5)
}

int test_compound_assignment() {
  int x = 10;
  x += 5;   // x = x + 5
  x -= 3;   // x = x - 3
  x *= 2;   // x = x * 2
  return x; // Expected: 24 ((10+5-3)*2 = 12*2)
}

int main() {
  return test_local_variables() + test_global_access(); // Expected: 115
}