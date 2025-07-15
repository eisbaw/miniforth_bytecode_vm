/* Deep recursion stress test - Tests VM call stack limits */

int deep_countdown(int n) {
    if (n <= 0) {
        return 0;
    }
    return 1 + deep_countdown(n - 1);
}

int tail_recursive_sum(int n, int acc) {
    if (n <= 0) {
        return acc;
    }
    return tail_recursive_sum(n - 1, acc + n);
}

int mutual_recursion_a(int n);
int mutual_recursion_b(int n);

int mutual_recursion_a(int n) {
    if (n <= 0) {
        return 1;
    }
    return mutual_recursion_b(n - 1);
}

int mutual_recursion_b(int n) {
    if (n <= 0) {
        return 0;
    }
    return mutual_recursion_a(n - 1);
}

int nested_function_calls(int depth) {
    if (depth <= 0) {
        return depth;
    }
    
    int a = nested_function_calls(depth - 1);
    int b = nested_function_calls(depth - 1);
    int c = nested_function_calls(depth - 1);
    
    return a + b + c;
}

int main() {
    /* Test moderate deep recursion */
    int result1 = deep_countdown(100);          // Expected: 100
    int result2 = tail_recursive_sum(50, 0);    // Expected: 1275 (sum 1..50)
    int result3 = mutual_recursion_a(10);       // Expected: 0 or 1
    
    /* Careful with this one - exponential growth */
    int result4 = nested_function_calls(8);     // Expected: large number
    
    return result1;  // Return simpler result
}