/* Test loop constructs */

int test_while_loop() {
    int sum = 0;
    int i = 1;
    while (i <= 5) {
        sum = sum + i;
        i = i + 1;
    }
    return sum;  // Expected: 15 (1+2+3+4+5)
}

int test_for_loop() {
    int sum = 0;
    int i;
    for (i = 1; i <= 4; i = i + 1) {
        sum = sum + i;
    }
    return sum;  // Expected: 10 (1+2+3+4)
}

int test_nested_loops() {
    int total = 0;
    int i, j;
    for (i = 1; i <= 3; i = i + 1) {
        for (j = 1; j <= 2; j = j + 1) {
            total = total + 1;
        }
    }
    return total;  // Expected: 6 (3*2)
}

int test_break_continue() {
    int sum = 0;
    int i;
    for (i = 1; i <= 10; i = i + 1) {
        if (i > 5) {
            break;
        }
        if (i == 3) {
            continue;
        }
        sum = sum + i;
    }
    return sum;  // Expected: 12 (1+2+4+5, skipping 3)
}

int main() {
    return test_while_loop();  // Expected: 15
}