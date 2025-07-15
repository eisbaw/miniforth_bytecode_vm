/* Long function call chain - Tests sequential stack usage */

int func_20(int x) { return x + 20; }
int func_19(int x) { return func_20(x + 19); }
int func_18(int x) { return func_19(x + 18); }
int func_17(int x) { return func_18(x + 17); }
int func_16(int x) { return func_17(x + 16); }
int func_15(int x) { return func_16(x + 15); }
int func_14(int x) { return func_15(x + 14); }
int func_13(int x) { return func_14(x + 13); }
int func_12(int x) { return func_13(x + 12); }
int func_11(int x) { return func_12(x + 11); }
int func_10(int x) { return func_11(x + 10); }
int func_09(int x) { return func_10(x + 9); }
int func_08(int x) { return func_09(x + 8); }
int func_07(int x) { return func_08(x + 7); }
int func_06(int x) { return func_07(x + 6); }
int func_05(int x) { return func_06(x + 5); }
int func_04(int x) { return func_05(x + 4); }
int func_03(int x) { return func_04(x + 3); }
int func_02(int x) { return func_03(x + 2); }
int func_01(int x) { return func_02(x + 1); }

int complex_parameter_passing(int a, int b, int c, int d, int e) {
    int result = 0;
    result += a * b;
    result += c * d;
    result += e;
    
    if (result > 100) {
        return complex_parameter_passing(a-1, b-1, c-1, d-1, e-1);
    }
    
    return result;
}

int main() {
    /* Test long call chain - sum should be 1+2+...+20 = 210 */
    int chain_result = func_01(0);  // Expected: 210
    
    /* Test complex parameter passing */
    int param_result = complex_parameter_passing(5, 6, 7, 8, 9);
    
    return chain_result;  // Expected: 210
}