/* Test negation and unary operations */

int test_negation() {
    int x = 5;
    return -x;  // Expected: -5
}

int test_double_negation() {
    int x = -7;
    return -x;  // Expected: 7
}

int test_complex_negation() {
    return -(3 + 4);  // Expected: -7
}

int main() {
    return test_double_negation();  // Expected: 7
}