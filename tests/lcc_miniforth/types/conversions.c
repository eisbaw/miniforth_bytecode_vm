/* Test type conversions and casts */

int test_implicit_conversion() {
  int x = 10;
  unsigned int y = x; // int to unsigned
  return y;           // Expected: 10
}

int test_explicit_cast() {
  int x = -5;
  unsigned int y = (unsigned int)x;
  return (int)y; // Expected: -5 (or large positive due to wrap)
}

int test_char_to_int() {
  char c = 'A';
  int x = c; // Implicit conversion
  return x;  // Expected: 65 (ASCII value of 'A')
}

int test_int_to_char() {
  int x = 66;
  char c = (char)x;
  return (int)c; // Expected: 66 ('B' back to int)
}

int test_pointer_arithmetic_types() {
  int arr[5] = {1, 2, 3, 4, 5};
  int *ptr = arr;
  char *char_ptr = (char *)ptr; // Pointer cast
  int *back_ptr = (int *)char_ptr;
  return *back_ptr; // Expected: 1 (first element)
}

int test_size_promotions() {
  short s = 100;
  int i = s;     // Promotion
  long l = i;    // Promotion
  return (int)l; // Expected: 100
}

int test_arithmetic_conversions() {
  int x = 5;
  unsigned int y = 3;
  int result = x + y; // Mixed arithmetic
  return result;      // Expected: 8
}

int main() {
  return test_char_to_int(); // Expected: 65
}