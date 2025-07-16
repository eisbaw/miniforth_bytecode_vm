// Comprehensive test for IRGraph backend
#include <stdio.h>

// Test function calls and arithmetic
int factorial(int n) {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

// Test conditionals and loops
int fibonacci(int n) {
    int a = 0, b = 1, temp;
    for (int i = 0; i < n; i++) {
        temp = a + b;
        a = b;
        b = temp;
    }
    return a;
}

// Test multiple operations
int complex_calculation(int x, int y, int z) {
    int result = 0;
    
    // Test arithmetic operations
    result = x + y;
    result = result * z;
    result = result / 2;
    result = result - 5;
    
    // Test comparisons
    if (result > 10) {
        result = result + 100;
    } else if (result < 0) {
        result = 0;
    }
    
    return result;
}

int main() {
    int a = 5;
    int b = 3;
    int c = 2;
    
    // Test various operations that should create graph nodes
    int sum = a + b;
    int product = sum * c;
    int quotient = product / 2;
    
    // Test function calls
    int fact = factorial(4);
    int fib = fibonacci(6);
    int complex = complex_calculation(a, b, c);
    
    // Test comparisons
    int comparison = (sum > product) ? 1 : 0;
    
    return sum + product + quotient + fact + fib + complex + comparison;
}