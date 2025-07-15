/* Test function definitions and calls */

int add_two_numbers(int a, int b) {
    return a + b;
}

int multiply_by_three(int x) {
    return x * 3;
}

int no_args_function() {
    return 42;
}

int chain_function_calls(int x) {
    int result = add_two_numbers(x, 5);
    result = multiply_by_three(result);
    return result;
}

int test_void_function() {
    /* Simulate void function by returning unit value */
    return 1;
}

int main() {
    int result = chain_function_calls(2);  // (2+5)*3 = 21
    return result;  // Expected: 21
}