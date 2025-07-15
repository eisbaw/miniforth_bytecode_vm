/* Test comparison operations */

int test_equality() {
    return (5 == 5) ? 1 : 0;  // Expected: 1 (true)
}

int test_inequality() {
    return (5 != 3) ? 1 : 0;  // Expected: 1 (true)
}

int test_less_than() {
    return (3 < 5) ? 1 : 0;  // Expected: 1 (true)
}

int test_greater_than() {
    return (7 > 4) ? 1 : 0;  // Expected: 1 (true)
}

int test_less_equal() {
    return (5 <= 5) ? 1 : 0;  // Expected: 1 (true)
}

int test_greater_equal() {
    return (6 >= 4) ? 1 : 0;  // Expected: 1 (true)
}

int main() {
    return test_equality() + test_less_than() + test_greater_than();  // Expected: 3
}