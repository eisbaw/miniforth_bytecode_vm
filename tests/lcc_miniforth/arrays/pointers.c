/* Test pointer operations */

int test_pointer_basics() {
    int x = 42;
    int *ptr = &x;
    return *ptr;  // Expected: 42
}

int test_pointer_assignment() {
    int x = 10;
    int y = 20;
    int *ptr = &x;
    *ptr = 30;    // Modify x through pointer
    ptr = &y;     // Point to y
    *ptr = 40;    // Modify y through pointer
    return x + y; // Expected: 30 + 40 = 70
}

int test_pointer_arithmetic() {
    int arr[4] = {1, 2, 3, 4};
    int *ptr = arr;        // Point to first element
    int first = *ptr;      // 1
    ptr++;                 // Move to next element
    int second = *ptr;     // 2
    ptr += 2;              // Move 2 elements forward
    int fourth = *ptr;     // 4
    return first + second + fourth;  // Expected: 1 + 2 + 4 = 7
}

int test_array_pointer_equivalence() {
    int arr[3] = {5, 10, 15};
    int *ptr = arr;
    return ptr[1] + arr[2];  // Expected: 10 + 15 = 25
}

int swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
    return 1;  // Success indicator
}

int test_pointer_parameters() {
    int x = 100;
    int y = 200;
    swap(&x, &y);
    return x;  // Expected: 200 (after swap)
}

int main() {
    return test_pointer_arithmetic();  // Expected: 7
}