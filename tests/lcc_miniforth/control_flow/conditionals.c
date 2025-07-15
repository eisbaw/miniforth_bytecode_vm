/* Test if/else conditional statements */

int test_simple_if() {
  int x = 5;
  if (x > 3) {
    return 10;
  }
  return 0; // Should not reach here
}

int test_if_else() {
  int x = 2;
  if (x > 5) {
    return 1;
  } else {
    return 42; // Expected: 42
  }
}

int test_nested_if() {
  int x = 7;
  if (x > 5) {
    if (x < 10) {
      return 99; // Expected: 99
    } else {
      return 1;
    }
  } else {
    return 0;
  }
}

int test_multiple_conditions() {
  int a = 3, b = 4;
  if (a < b && b > 2) {
    return 77; // Expected: 77
  } else {
    return 0;
  }
}

int main() {
  return test_nested_if(); // Expected: 99
}