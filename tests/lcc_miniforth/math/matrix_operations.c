/* Matrix operations for mathematical computation stress testing */

void matrix_multiply_3x3(int a[3][3], int b[3][3], int result[3][3]) {
    int i, j, k;
    
    /* Initialize result matrix */
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            result[i][j] = 0;
        }
    }
    
    /* Matrix multiplication */
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            for (k = 0; k < 3; k++) {
                result[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}

void matrix_transpose_3x3(int matrix[3][3], int result[3][3]) {
    int i, j;
    
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            result[j][i] = matrix[i][j];
        }
    }
}

int matrix_determinant_3x3(int matrix[3][3]) {
    int det = 0;
    
    /* Calculate determinant using cofactor expansion */
    det += matrix[0][0] * (matrix[1][1] * matrix[2][2] - matrix[1][2] * matrix[2][1]);
    det -= matrix[0][1] * (matrix[1][0] * matrix[2][2] - matrix[1][2] * matrix[2][0]);
    det += matrix[0][2] * (matrix[1][0] * matrix[2][1] - matrix[1][1] * matrix[2][0]);
    
    return det;
}

int matrix_trace_3x3(int matrix[3][3]) {
    /* Sum of diagonal elements */
    return matrix[0][0] + matrix[1][1] + matrix[2][2];
}

int test_matrix_operations() {
    int a[3][3] = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    int b[3][3] = {{9, 8, 7}, {6, 5, 4}, {3, 2, 1}};
    int result[3][3];
    int transpose[3][3];
    
    /* Test matrix multiplication */
    matrix_multiply_3x3(a, b, result);
    
    /* Sum all elements of result matrix */
    int sum = 0;
    int i, j;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            sum += result[i][j];
        }
    }
    
    /* Test transpose */
    matrix_transpose_3x3(a, transpose);
    int transpose_sum = 0;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            transpose_sum += transpose[i][j];
        }
    }
    
    /* Test determinant */
    int det_a = matrix_determinant_3x3(a);  /* Should be 0 (singular matrix) */
    
    /* Test trace */
    int trace_a = matrix_trace_3x3(a);  /* Should be 1+5+9 = 15 */
    
    return sum + trace_a;  /* Expected: sum + 15 */
}

int test_identity_matrix() {
    int identity[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
    int test_matrix[3][3] = {{2, 3, 4}, {5, 6, 7}, {8, 9, 10}};
    int result[3][3];
    
    /* Multiply by identity should give original matrix */
    matrix_multiply_3x3(test_matrix, identity, result);
    
    /* Verify result equals test_matrix */
    int matches = 1;
    int i, j;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            if (result[i][j] != test_matrix[i][j]) {
                matches = 0;
            }
        }
    }
    
    int det_identity = matrix_determinant_3x3(identity);  /* Should be 1 */
    int trace_identity = matrix_trace_3x3(identity);      /* Should be 3 */
    
    return matches * 100 + det_identity + trace_identity;  /* Expected: 104 */
}

int test_symmetric_matrix() {
    int symmetric[3][3] = {{1, 2, 3}, {2, 4, 5}, {3, 5, 6}};
    int transpose[3][3];
    
    matrix_transpose_3x3(symmetric, transpose);
    
    /* Check if matrix equals its transpose */
    int is_symmetric = 1;
    int i, j;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            if (symmetric[i][j] != transpose[i][j]) {
                is_symmetric = 0;
            }
        }
    }
    
    int det = matrix_determinant_3x3(symmetric);  /* Calculate determinant */
    int trace = matrix_trace_3x3(symmetric);      /* 1+4+6 = 11 */
    
    return is_symmetric * 100 + trace;  /* Expected: 111 */
}

int main() {
    int test1 = test_matrix_operations();  /* Expected: depends on calculation */
    int test2 = test_identity_matrix();    /* Expected: 104 */
    int test3 = test_symmetric_matrix();   /* Expected: 111 */
    
    return test2;  /* Expected: 104 */
}