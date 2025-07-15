/* Test variable scope and shadowing */

int global_x = 50;

int test_local_scope() {
    int x = 10;
    {
        int x = 20;  // Shadows outer x
        return x;    // Expected: 20
    }
}

int test_global_vs_local() {
    int global_x = 30;  // Shadows global
    return global_x;    // Expected: 30 (local wins)
}

int test_function_parameters(int x) {
    return x + 1;
}

int test_parameter_scope() {
    int x = 5;
    int result = test_function_parameters(10);  // x=10 in function
    return result + x;  // Expected: 11 + 5 = 16
}

int test_block_scope() {
    int sum = 0;
    {
        int i = 1;
        sum += i;
    }
    {
        int i = 2;  // Different i
        sum += i;
    }
    return sum;  // Expected: 3
}

int main() {
    return test_local_scope();  // Expected: 20
}