#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "miniforth.h"

// Test result tracking
static int tests_run = 0;
static int tests_passed = 0;

#define TEST(name) \
    do { \
        printf("Running test: %s\n", #name); \
        tests_run++; \
        name(); \
        tests_passed++; \
        printf("  PASSED\n"); \
    } while (0)

// Test 1: Basic Stack Operations
void test_stack_operations() {
    miniforth_vm_t *vm = vm_create(64, 64, 32);
    assert(vm != NULL);
    
    // Test DUP
    uint16_t program1[] = {OP_LIT, 42, OP_DUP, OP_EXIT};
    assert(vm_load_program(vm, program1, 4) == 0);
    assert(vm_run(vm) == VM_OK);
    
    uint16_t val1, val2;
    assert(vm_pop(vm, &val1) == VM_OK);
    assert(vm_pop(vm, &val2) == VM_OK);
    assert(val1 == 42);
    assert(val2 == 42);
    
    // Reset VM for next test
    vm_reset(vm);
    
    // Test SWAP
    uint16_t program2[] = {OP_LIT, 1, OP_LIT, 2, OP_SWAP, OP_EXIT};
    assert(vm_load_program(vm, program2, 6) == 0);
    assert(vm_run(vm) == VM_OK);
    
    assert(vm_pop(vm, &val1) == VM_OK);
    assert(vm_pop(vm, &val2) == VM_OK);
    assert(val1 == 1);
    assert(val2 == 2);
    
    vm_destroy(vm);
}

// Test 2: Arithmetic Operations
void test_arithmetic() {
    miniforth_vm_t *vm = vm_create(64, 64, 32);
    assert(vm != NULL);
    
    // Test addition: 3 + 4 = 7
    uint16_t program[] = {OP_LIT, 3, OP_LIT, 4, OP_ADD, OP_EXIT};
    assert(vm_load_program(vm, program, 6) == 0);
    assert(vm_run(vm) == VM_OK);
    
    uint16_t result;
    assert(vm_pop(vm, &result) == VM_OK);
    assert(result == 7);
    
    // Reset VM for next test
    vm_reset(vm);
    
    // Test multiplication: 6 * 7 = 42
    uint16_t program2[] = {OP_LIT, 6, OP_LIT, 7, OP_MUL, OP_EXIT};
    assert(vm_load_program(vm, program2, 6) == 0);
    assert(vm_run(vm) == VM_OK);
    
    assert(vm_pop(vm, &result) == VM_OK);
    assert(result == 42);
    
    // Reset VM for next test
    vm_reset(vm);
    
    // Test subtraction: 10 - 3 = 7
    uint16_t program3[] = {OP_LIT, 10, OP_LIT, 3, OP_SUB, OP_EXIT};
    assert(vm_load_program(vm, program3, 6) == 0);
    assert(vm_run(vm) == VM_OK);
    
    assert(vm_pop(vm, &result) == VM_OK);
    assert(result == 7);
    
    // Reset VM for next test
    vm_reset(vm);
    
    // Test division: 20 / 4 = 5
    uint16_t program4[] = {OP_LIT, 20, OP_LIT, 4, OP_DIV, OP_EXIT};
    assert(vm_load_program(vm, program4, 6) == 0);
    assert(vm_run(vm) == VM_OK);
    
    assert(vm_pop(vm, &result) == VM_OK);
    assert(result == 5);
    
    vm_destroy(vm);
}

// Test 3: Memory Access
void test_memory_access() {
    miniforth_vm_t *vm = vm_create(64, 64, 32);
    assert(vm != NULL);
    
    // Store 100 at address 10, then fetch it
    uint16_t program[] = {
        OP_LIT, 100,    // value
        OP_LIT, 10,     // address
        OP_STORE,       // store value at address
        OP_LIT, 10,     // address
        OP_FETCH,       // fetch from address
        OP_EXIT
    };
    assert(vm_load_program(vm, program, 12) == 0);
    assert(vm_run(vm) == VM_OK);
    
    uint16_t result;
    assert(vm_pop(vm, &result) == VM_OK);
    assert(result == 100);
    
    vm_destroy(vm);
}

// Test 4: Control Flow
void test_control_flow() {
    miniforth_vm_t *vm = vm_create(64, 64, 32);
    assert(vm != NULL);
    
    // Conditional jump test
    uint16_t program[] = {
        OP_LIT, 1,      // condition (true)
        OP_JZ, 8,       // jump to address 8 if zero
        OP_LIT, 42,     // this should execute
        OP_JMP, 10,     // jump to end
        OP_LIT, 99,     // this should be skipped (address 8)
        OP_EXIT         // address 10
    };
    assert(vm_load_program(vm, program, 12) == 0);
    assert(vm_run(vm) == VM_OK);
    
    uint16_t result;
    assert(vm_pop(vm, &result) == VM_OK);
    assert(result == 42);  // Should be 42, not 99
    
    // Reset VM for next test
    vm_reset(vm);
    
    // Test JZ with zero condition
    uint16_t program2[] = {
        OP_LIT, 0,      // condition (false)
        OP_JZ, 8,       // jump to address 8 if zero
        OP_LIT, 42,     // this should be skipped
        OP_JMP, 10,     // jump to end
        OP_LIT, 99,     // this should execute (address 8)
        OP_EXIT         // address 10
    };
    assert(vm_load_program(vm, program2, 12) == 0);
    assert(vm_run(vm) == VM_OK);
    
    assert(vm_pop(vm, &result) == VM_OK);
    assert(result == 99);  // Should be 99, not 42
    
    vm_destroy(vm);
}

// Test 5: Comparison Operations
void test_comparisons() {
    miniforth_vm_t *vm = vm_create(64, 64, 32);
    assert(vm != NULL);
    
    // Test equality: 5 == 5 should be true (1)
    uint16_t program1[] = {OP_LIT, 5, OP_LIT, 5, OP_EQ, OP_EXIT};
    assert(vm_load_program(vm, program1, 6) == 0);
    assert(vm_run(vm) == VM_OK);
    
    uint16_t result;
    assert(vm_pop(vm, &result) == VM_OK);
    assert(result == 1);
    
    // Reset VM for next test
    vm_reset(vm);
    
    // Test inequality: 5 == 3 should be false (0)
    uint16_t program2[] = {OP_LIT, 5, OP_LIT, 3, OP_EQ, OP_EXIT};
    assert(vm_load_program(vm, program2, 6) == 0);
    assert(vm_run(vm) == VM_OK);
    
    assert(vm_pop(vm, &result) == VM_OK);
    assert(result == 0);
    
    // Reset VM for next test
    vm_reset(vm);
    
    // Test less than: 3 < 5 should be true (1)
    uint16_t program3[] = {OP_LIT, 3, OP_LIT, 5, OP_LT, OP_EXIT};
    assert(vm_load_program(vm, program3, 6) == 0);
    assert(vm_run(vm) == VM_OK);
    
    assert(vm_pop(vm, &result) == VM_OK);
    assert(result == 1);
    
    // Reset VM for next test
    vm_reset(vm);
    
    // Test less than false: 5 < 3 should be false (0)
    uint16_t program4[] = {OP_LIT, 5, OP_LIT, 3, OP_LT, OP_EXIT};
    assert(vm_load_program(vm, program4, 6) == 0);
    assert(vm_run(vm) == VM_OK);
    
    assert(vm_pop(vm, &result) == VM_OK);
    assert(result == 0);
    
    vm_destroy(vm);
}

// Test 6: Function Calls
void test_function_calls() {
    miniforth_vm_t *vm = vm_create(64, 64, 32);
    assert(vm != NULL);
    
    // Simple function call test
    uint16_t program[] = {
        OP_CALL, 5,     // call function at address 5
        OP_LIT, 99,     // this should execute after return
        OP_EXIT,        // end of main
        OP_LIT, 42,     // function body (address 5)
        OP_EXIT         // return from function
    };
    assert(vm_load_program(vm, program, 8) == 0);
    assert(vm_run(vm) == VM_OK);
    
    uint16_t result1, result2;
    assert(vm_pop(vm, &result1) == VM_OK);
    assert(vm_pop(vm, &result2) == VM_OK);
    assert(result1 == 99);  // From main after return (top of stack)
    assert(result2 == 42);  // From function (bottom of stack)
    
    vm_destroy(vm);
}

// Test 7: Error Handling
void test_error_handling() {
    miniforth_vm_t *vm = vm_create(64, 64, 32);
    assert(vm != NULL);
    
    // Test division by zero
    uint16_t program1[] = {OP_LIT, 10, OP_LIT, 0, OP_DIV, OP_EXIT};
    assert(vm_load_program(vm, program1, 6) == 0);
    assert(vm_run(vm) == VM_ERROR_DIVISION_BY_ZERO);
    
    // Reset VM
    vm_reset(vm);
    
    // Test stack underflow
    uint16_t program2[] = {OP_DROP, OP_EXIT};
    assert(vm_load_program(vm, program2, 2) == 0);
    assert(vm_run(vm) == VM_ERROR_STACK_UNDERFLOW);
    
    vm_destroy(vm);
}

// Test 8: VM Creation and Destruction
void test_vm_lifecycle() {
    // Test normal creation
    miniforth_vm_t *vm = vm_create(100, 100, 50);
    assert(vm != NULL);
    assert(vm->code_size == 100);
    assert(vm->data_size == 100);
    assert(vm->stack_size == 50);
    assert(vm->pc == 0);
    assert(vm->sp == 0);
    assert(vm->rp == 0);
    vm_destroy(vm);
    
    // Test invalid creation
    vm = vm_create(0, 100, 50);
    assert(vm == NULL);
    
    vm = vm_create(100, 0, 50);
    assert(vm == NULL);
    
    vm = vm_create(100, 100, 0);
    assert(vm == NULL);
}

// Test 9: Stack Operations Edge Cases
void test_stack_edge_cases() {
    miniforth_vm_t *vm = vm_create(64, 64, 4);  // Small stack
    assert(vm != NULL);
    
    // Fill stack to capacity
    for (int i = 0; i < 4; i++) {
        assert(vm_push(vm, i) == VM_OK);
    }
    
    // Stack overflow
    assert(vm_push(vm, 999) == VM_ERROR_STACK_OVERFLOW);
    
    // Empty stack
    for (int i = 0; i < 4; i++) {
        uint16_t val;
        assert(vm_pop(vm, &val) == VM_OK);
    }
    
    // Stack underflow
    uint16_t val;
    assert(vm_pop(vm, &val) == VM_ERROR_STACK_UNDERFLOW);
    
    vm_destroy(vm);
}

// Test 10: Complex arithmetic expressions
void test_complex_arithmetic() {
    miniforth_vm_t *vm = vm_create(64, 64, 32);
    assert(vm != NULL);
    
    // Test: (10 + 5) * 2 - 3 = 27
    uint16_t program[] = {
        OP_LIT, 10,     // 10
        OP_LIT, 5,      // 10 5
        OP_ADD,         // 15
        OP_LIT, 2,      // 15 2
        OP_MUL,         // 30
        OP_LIT, 3,      // 30 3
        OP_SUB,         // 27
        OP_EXIT
    };
    assert(vm_load_program(vm, program, 14) == 0);
    assert(vm_run(vm) == VM_OK);
    
    uint16_t result;
    assert(vm_pop(vm, &result) == VM_OK);
    assert(result == 27);
    
    vm_destroy(vm);
}

// Test 11: Modulo operation (using division remainder)
void test_modulo_simulation() {
    miniforth_vm_t *vm = vm_create(64, 64, 32);
    assert(vm != NULL);
    
    // Simulate 17 % 5 = 2 using: 17 - (17/5)*5
    uint16_t program[] = {
        OP_LIT, 17,     // dividend
        OP_DUP,         // 17 17
        OP_LIT, 5,      // 17 17 5
        OP_DIV,         // 17 3 (17/5=3)
        OP_LIT, 5,      // 17 3 5
        OP_MUL,         // 17 15 (3*5=15)
        OP_SUB,         // 2 (17-15=2)
        OP_EXIT
    };
    assert(vm_load_program(vm, program, 16) == 0);
    assert(vm_run(vm) == VM_OK);
    
    uint16_t result;
    assert(vm_pop(vm, &result) == VM_OK);
    assert(result == 2);  // 17 % 5 = 2
    
    vm_destroy(vm);
}

// Test 12: Memory operations edge cases
void test_memory_edge_cases() {
    miniforth_vm_t *vm = vm_create(64, 64, 32);
    assert(vm != NULL);
    
    // Test memory at boundaries
    uint16_t program[] = {
        OP_LIT, 999,    // value
        OP_LIT, 63,     // address (last valid address)
        OP_STORE,       // store
        OP_LIT, 63,     // address
        OP_FETCH,       // fetch
        OP_EXIT
    };
    assert(vm_load_program(vm, program, 12) == 0);
    assert(vm_run(vm) == VM_OK);
    
    uint16_t result;
    assert(vm_pop(vm, &result) == VM_OK);
    assert(result == 999);
    
    vm_destroy(vm);
}

// Test 13: Conditional branching patterns
void test_conditional_patterns() {
    miniforth_vm_t *vm = vm_create(64, 64, 32);
    assert(vm != NULL);
    
    // Test if-else pattern: if (5 < 10) then 42 else 99
    uint16_t program[] = {
        OP_LIT, 5,      // 5
        OP_LIT, 10,     // 5 10
        OP_LT,          // 1 (true)
        OP_JZ, 10,      // jump to else if false (address 10)
        OP_LIT, 42,     // then branch
        OP_JMP, 12,     // jump to end (address 12)
        OP_LIT, 99,     // else branch (address 10)
        OP_EXIT         // end (address 12)
    };
    assert(vm_load_program(vm, program, 14) == 0);
    assert(vm_run(vm) == VM_OK);
    
    uint16_t result;
    assert(vm_pop(vm, &result) == VM_OK);
    assert(result == 42);  // Should take then branch
    
    vm_destroy(vm);
}

// Test 14: Return stack operations
void test_return_stack_edge_cases() {
    miniforth_vm_t *vm = vm_create(64, 64, 4);  // Small return stack
    assert(vm != NULL);
    
    // Fill return stack to capacity with deep recursion simulation
    for (int i = 0; i < 4; i++) {
        assert(vm_rpush(vm, i) == VM_OK);
    }
    
    // Return stack overflow
    assert(vm_rpush(vm, 999) == VM_ERROR_RETURN_STACK_OVERFLOW);
    
    // Empty return stack
    for (int i = 0; i < 4; i++) {
        uint16_t val;
        assert(vm_rpop(vm, &val) == VM_OK);
    }
    
    // Return stack underflow
    uint16_t val;
    assert(vm_rpop(vm, &val) == VM_ERROR_RETURN_STACK_UNDERFLOW);
    
    vm_destroy(vm);
}

// Test 15: Mixed operations stress test
void test_mixed_operations() {
    miniforth_vm_t *vm = vm_create(128, 128, 64);
    assert(vm != NULL);
    
    // Complex program mixing all operations
    uint16_t program[] = {
        OP_LIT, 10,     // Initial value
        OP_DUP,         // Duplicate it
        OP_LIT, 5,      // Add another value
        OP_SWAP,        // Swap order
        OP_LIT, 1,      // Address for storage
        OP_STORE,       // Store 10 at address 1
        OP_ADD,         // Add remaining values (10 + 5 = 15)
        OP_LIT, 1,      // Get stored value
        OP_FETCH,       // Fetch 10
        OP_MUL,         // 15 * 10 = 150
        OP_LIT, 150,    // Expected result
        OP_EQ,          // Compare (should be 1)
        OP_EXIT
    };
    assert(vm_load_program(vm, program, 26) == 0);
    assert(vm_run(vm) == VM_OK);
    
    uint16_t result;
    assert(vm_pop(vm, &result) == VM_OK);
    assert(result == 1);  // Comparison should be true
    
    vm_destroy(vm);
}

int main() {
    printf("Running Miniforth VM Tests\n");
    printf("==========================\n");
    
    TEST(test_vm_lifecycle);
    TEST(test_stack_operations);
    TEST(test_arithmetic);
    TEST(test_memory_access);
    TEST(test_control_flow);
    TEST(test_comparisons);
    TEST(test_function_calls);
    TEST(test_error_handling);
    TEST(test_stack_edge_cases);
    TEST(test_complex_arithmetic);
    TEST(test_modulo_simulation);
    TEST(test_memory_edge_cases);
    TEST(test_return_stack_edge_cases);
    
    printf("\nTest Results:\n");
    printf("Tests run: %d\n", tests_run);
    printf("Tests passed: %d\n", tests_passed);
    
    if (tests_passed == tests_run) {
        printf("ALL TESTS PASSED!\n");
        return 0;
    } else {
        printf("SOME TESTS FAILED!\n");
        return 1;
    }
}