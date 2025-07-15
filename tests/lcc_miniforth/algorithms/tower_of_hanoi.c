/* Tower of Hanoi - Stress test recursive algorithm */

int move_count = 0;

void hanoi(int n, int from, int to, int aux) {
    if (n == 1) {
        move_count++;
        return;
    }
    
    hanoi(n - 1, from, aux, to);
    move_count++;
    hanoi(n - 1, aux, to, from);
}

int solve_hanoi(int disks) {
    move_count = 0;
    hanoi(disks, 1, 3, 2);
    return move_count;
}

int main() {
    /* Test with 5 disks - should be 2^5 - 1 = 31 moves */
    return solve_hanoi(5);  // Expected: 31
}