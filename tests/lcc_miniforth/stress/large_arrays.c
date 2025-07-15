/* Large array operations - Tests VM memory handling */

int test_large_array_sum() {
    int large_array[500];  /* Large stack allocation */
    int i;
    int sum = 0;
    
    /* Initialize array with pattern */
    for (i = 0; i < 500; i++) {
        large_array[i] = i % 100;  /* 0-99 repeating pattern */
    }
    
    /* Sum all elements */
    for (i = 0; i < 500; i++) {
        sum += large_array[i];
    }
    
    return sum;  /* Expected: 5 * (0+1+...+99) = 5 * 4950 = 24750 */
}

int test_matrix_operations() {
    int matrix[20][20];  /* 400 element 2D array */
    int i, j;
    int sum = 0;
    
    /* Initialize matrix */
    for (i = 0; i < 20; i++) {
        for (j = 0; j < 20; j++) {
            matrix[i][j] = (i * j) % 10;
        }
    }
    
    /* Calculate matrix sum */
    for (i = 0; i < 20; i++) {
        for (j = 0; j < 20; j++) {
            sum += matrix[i][j];
        }
    }
    
    return sum;
}

int test_array_copy() {
    int source[100];
    int dest[100];
    int i;
    
    /* Initialize source */
    for (i = 0; i < 100; i++) {
        source[i] = i * i;  /* Square numbers */
    }
    
    /* Copy array */
    for (i = 0; i < 100; i++) {
        dest[i] = source[i];
    }
    
    /* Verify copy by summing destination */
    int sum = 0;
    for (i = 0; i < 100; i++) {
        sum += dest[i];
    }
    
    return sum;  /* Sum of squares 0^2 + 1^2 + ... + 99^2 */
}

int test_nested_array_access() {
    int data[10][10][5];  /* 3D array - 500 elements */
    int i, j, k;
    int count = 0;
    
    /* Initialize and count simultaneously */
    for (i = 0; i < 10; i++) {
        for (j = 0; j < 10; j++) {
            for (k = 0; k < 5; k++) {
                data[i][j][k] = count;
                count++;
            }
        }
    }
    
    /* Sum diagonal elements */
    int sum = 0;
    for (i = 0; i < 5; i++) {
        sum += data[i][i][i];  /* 0, 56, 112, 168, 224 */
    }
    
    return sum;  /* Expected: 560 */
}

int main() {
    return test_large_array_sum();  /* Expected: 24750 */
}