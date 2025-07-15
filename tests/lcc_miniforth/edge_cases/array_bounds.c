/* Array boundary condition tests */

int test_array_initialization() {
  int arr[10];
  int i;

  /* Initialize array with boundary values */
  arr[0] = 100; /* First element */
  arr[9] = 200; /* Last element */

  /* Initialize middle elements */
  for (i = 1; i < 9; i++) {
    arr[i] = i * 10;
  }

  /* Sum all elements */
  int sum = 0;
  for (i = 0; i < 10; i++) {
    sum += arr[i];
  }

  return sum; /* Expected: 100 + 10+20+30+40+50+60+70+80 + 200 = 660 */
}

int test_single_element_array() {
  int single[1];
  single[0] = 42;

  return single[0]; /* Expected: 42 */
}

int test_array_copy_boundary() {
  int source[5] = {1, 2, 3, 4, 5};
  int dest[5];
  int i;

  /* Copy first and last elements explicitly */
  dest[0] = source[0];
  dest[4] = source[4];

  /* Copy middle elements */
  for (i = 1; i < 4; i++) {
    dest[i] = source[i];
  }

  /* Verify copy by summing */
  int sum = 0;
  for (i = 0; i < 5; i++) {
    sum += dest[i];
  }

  return sum; /* Expected: 15 */
}

int test_nested_array_boundaries() {
  int matrix[3][3];
  int i, j;

  /* Initialize corners */
  matrix[0][0] = 1; /* Top-left */
  matrix[0][2] = 2; /* Top-right */
  matrix[2][0] = 3; /* Bottom-left */
  matrix[2][2] = 4; /* Bottom-right */

  /* Initialize edges and center */
  matrix[0][1] = 5; /* Top center */
  matrix[1][0] = 6; /* Left center */
  matrix[1][1] = 7; /* Center */
  matrix[1][2] = 8; /* Right center */
  matrix[2][1] = 9; /* Bottom center */

  /* Sum all elements */
  int sum = 0;
  for (i = 0; i < 3; i++) {
    for (j = 0; j < 3; j++) {
      sum += matrix[i][j];
    }
  }

  return sum; /* Expected: 45 */
}

int main() {
  int t1 = test_array_initialization();    /* Expected: 660 */
  int t2 = test_single_element_array();    /* Expected: 42 */
  int t3 = test_array_copy_boundary();     /* Expected: 15 */
  int t4 = test_nested_array_boundaries(); /* Expected: 45 */

  return t2; /* Expected: 42 */
}