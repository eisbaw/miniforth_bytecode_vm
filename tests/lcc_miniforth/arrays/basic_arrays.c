/* Test basic array operations */

int test_array_declaration() {
  int arr[5];
  arr[0] = 10;
  arr[1] = 20;
  arr[2] = 30;
  return arr[1]; // Expected: 20
}

int test_array_initialization() {
  int arr[4] = {1, 2, 3, 4};
  return arr[2]; // Expected: 3
}

int test_array_sum() {
  int arr[5] = {1, 2, 3, 4, 5};
  int sum = 0;
  int i;
  for (i = 0; i < 5; i++) {
    sum += arr[i];
  }
  return sum; // Expected: 15
}

int test_array_modification() {
  int arr[3] = {10, 20, 30};
  arr[1] = arr[0] + arr[2]; // 10 + 30 = 40
  return arr[1];            // Expected: 40
}

int test_array_as_parameter(int arr[], int size) {
  int sum = 0;
  int i;
  for (i = 0; i < size; i++) {
    sum += arr[i];
  }
  return sum;
}

int test_array_function_call() {
  int data[3] = {5, 10, 15};
  return test_array_as_parameter(data, 3); // Expected: 30
}

int main() {
  return test_array_sum(); // Expected: 15
}