/* Control flow edge cases and boundary conditions */

int test_nested_conditionals(int x) {
    int result = 0;
    
    if (x > 0) {
        if (x > 10) {
            if (x > 20) {
                result = 3;  /* x > 20 */
            } else {
                result = 2;  /* 10 < x <= 20 */
            }
        } else {
            result = 1;      /* 0 < x <= 10 */
        }
    } else {
        if (x < 0) {
            result = -1;     /* x < 0 */
        } else {
            result = 0;      /* x == 0 */
        }
    }
    
    return result;
}

int test_loop_edge_cases() {
    int result = 0;
    int i;
    
    /* Empty loop (0 iterations) */
    for (i = 10; i < 10; i++) {
        result += i;  /* Should not execute */
    }
    
    /* Single iteration loop */
    for (i = 5; i < 6; i++) {
        result += i;  /* Should add 5 */
    }
    
    /* Reverse loop */
    for (i = 3; i > 0; i--) {
        result += i;  /* Should add 3+2+1 = 6 */
    }
    
    return result;  /* Expected: 0 + 5 + 6 = 11 */
}

int test_switch_like_behavior() {
    /* Simulate switch using if-else chain */
    int value = 2;
    int result = 0;
    
    if (value == 1) {
        result = 10;
    } else if (value == 2) {
        result = 20;
    } else if (value == 3) {
        result = 30;
    } else {
        result = 0;
    }
    
    return result;  /* Expected: 20 */
}

int test_while_loop_conditions() {
    int count = 0;
    int sum = 0;
    
    /* Standard while loop */
    while (count < 5) {
        sum += count;
        count++;
    }
    
    /* While loop that doesn't execute */
    count = 10;
    while (count < 5) {
        sum += 100;  /* Should not execute */
    }
    
    return sum;  /* Expected: 0+1+2+3+4 = 10 */
}

int test_break_continue_simulation() {
    /* Simulate break/continue using conditionals and goto-like logic */
    int sum = 0;
    int i;
    
    for (i = 0; i < 10; i++) {
        if (i == 3) {
            /* Skip this iteration (simulate continue) */
        } else if (i == 7) {
            /* Early exit (simulate break) */
            return sum;
        } else {
            sum += i;
        }
    }
    
    return sum;  /* Should exit at i=7, sum = 0+1+2+4+5+6 = 18 */
}

int main() {
    int t1 = test_nested_conditionals(15);   /* Expected: 2 */
    int t2 = test_loop_edge_cases();         /* Expected: 11 */
    int t3 = test_switch_like_behavior();    /* Expected: 20 */
    int t4 = test_while_loop_conditions();   /* Expected: 10 */
    int t5 = test_break_continue_simulation(); /* Expected: 18 */
    
    return t5;  /* Expected: 18 */
}