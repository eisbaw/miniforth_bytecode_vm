#include "miniforth.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// VM lifecycle functions
miniforth_vm_t* vm_create(uint16_t code_size, uint16_t data_size, uint16_t stack_size) {
    if (code_size == 0 || data_size == 0 || stack_size == 0) {
        return NULL;
    }
    
    miniforth_vm_t *vm = malloc(sizeof(miniforth_vm_t));
    if (!vm) {
        return NULL;
    }
    
    // Allocate memory segments
    vm->code_memory = calloc(code_size, sizeof(uint16_t));
    vm->data_memory = calloc(data_size, sizeof(uint16_t));
    vm->data_stack = calloc(stack_size, sizeof(uint16_t));
    vm->return_stack = calloc(stack_size, sizeof(uint16_t));
    
    if (!vm->code_memory || !vm->data_memory || !vm->data_stack || !vm->return_stack) {
        vm_destroy(vm);
        return NULL;
    }
    
    // Initialize VM state
    vm->pc = 0;
    vm->sp = 0;
    vm->rp = 0;
    vm->code_size = code_size;
    vm->data_size = data_size;
    vm->stack_size = stack_size;
    vm->last_error = VM_OK;
    
    return vm;
}

void vm_destroy(miniforth_vm_t *vm) {
    if (!vm) return;
    
    free(vm->code_memory);
    free(vm->data_memory);
    free(vm->data_stack);
    free(vm->return_stack);
    free(vm);
}

int vm_load_program(miniforth_vm_t *vm, const uint16_t *program, uint16_t size) {
    if (!vm || !program || size == 0 || size > vm->code_size) {
        return -1;
    }
    
    memcpy(vm->code_memory, program, size * sizeof(uint16_t));
    vm->pc = 0;
    vm->sp = 0;
    vm->rp = 0;
    vm->last_error = VM_OK;
    
    return 0;
}

// Stack operations
vm_error_t vm_push(miniforth_vm_t *vm, uint16_t value) {
    if (!vm) return VM_ERROR_NULL_POINTER;
    
#if MINIFORTH_ENABLE_BOUNDS_CHECK
    if (vm->sp >= vm->stack_size) {
        vm->last_error = VM_ERROR_STACK_OVERFLOW;
        return VM_ERROR_STACK_OVERFLOW;
    }
#endif
    
    vm->data_stack[vm->sp++] = value;
    return VM_OK;
}

vm_error_t vm_pop(miniforth_vm_t *vm, uint16_t *value) {
    if (!vm || !value) return VM_ERROR_NULL_POINTER;
    
#if MINIFORTH_ENABLE_BOUNDS_CHECK
    if (vm->sp == 0) {
        vm->last_error = VM_ERROR_STACK_UNDERFLOW;
        return VM_ERROR_STACK_UNDERFLOW;
    }
#endif
    
    *value = vm->data_stack[--vm->sp];
    return VM_OK;
}

vm_error_t vm_peek(miniforth_vm_t *vm, int depth, uint16_t *value) {
    if (!vm || !value) return VM_ERROR_NULL_POINTER;
    
#if MINIFORTH_ENABLE_BOUNDS_CHECK
    if (depth < 0 || vm->sp <= depth) {
        vm->last_error = VM_ERROR_STACK_UNDERFLOW;
        return VM_ERROR_STACK_UNDERFLOW;
    }
#endif
    
    *value = vm->data_stack[vm->sp - 1 - depth];
    return VM_OK;
}

// Return stack operations
vm_error_t vm_rpush(miniforth_vm_t *vm, uint16_t value) {
    if (!vm) return VM_ERROR_NULL_POINTER;
    
#if MINIFORTH_ENABLE_BOUNDS_CHECK
    if (vm->rp >= vm->stack_size) {
        vm->last_error = VM_ERROR_RETURN_STACK_OVERFLOW;
        return VM_ERROR_RETURN_STACK_OVERFLOW;
    }
#endif
    
    vm->return_stack[vm->rp++] = value;
    return VM_OK;
}

vm_error_t vm_rpop(miniforth_vm_t *vm, uint16_t *value) {
    if (!vm || !value) return VM_ERROR_NULL_POINTER;
    
#if MINIFORTH_ENABLE_BOUNDS_CHECK
    if (vm->rp == 0) {
        vm->last_error = VM_ERROR_RETURN_STACK_UNDERFLOW;
        return VM_ERROR_RETURN_STACK_UNDERFLOW;
    }
#endif
    
    *value = vm->return_stack[--vm->rp];
    return VM_OK;
}

// Memory access functions
uint16_t vm_read_memory(miniforth_vm_t *vm, uint16_t addr) {
    if (!vm || addr >= vm->data_size) {
        if (vm) vm->last_error = VM_ERROR_MEMORY_ACCESS;
        return 0;
    }
    
    return vm->data_memory[addr];
}

void vm_write_memory(miniforth_vm_t *vm, uint16_t addr, uint16_t value) {
    if (!vm || addr >= vm->data_size) {
        if (vm) vm->last_error = VM_ERROR_MEMORY_ACCESS;
        return;
    }
    
    vm->data_memory[addr] = value;
}

uint8_t vm_read_byte(miniforth_vm_t *vm, uint16_t addr) {
    if (!vm || addr >= vm->data_size * 2) {
        if (vm) vm->last_error = VM_ERROR_MEMORY_ACCESS;
        return 0;
    }
    
    uint16_t word_addr = addr / 2;
    uint16_t word_value = vm->data_memory[word_addr];
    
    if (addr % 2 == 0) {
        return (uint8_t)(word_value & 0xFF);
    } else {
        return (uint8_t)((word_value >> 8) & 0xFF);
    }
}

void vm_write_byte(miniforth_vm_t *vm, uint16_t addr, uint8_t value) {
    if (!vm || addr >= vm->data_size * 2) {
        if (vm) vm->last_error = VM_ERROR_MEMORY_ACCESS;
        return;
    }
    
    uint16_t word_addr = addr / 2;
    uint16_t word_value = vm->data_memory[word_addr];
    
    if (addr % 2 == 0) {
        word_value = (word_value & 0xFF00) | value;
    } else {
        word_value = (word_value & 0x00FF) | (value << 8);
    }
    
    vm->data_memory[word_addr] = word_value;
}

// Error handling
const char* vm_error_string(vm_error_t error) {
    switch (error) {
        case VM_OK: return "No error";
        case VM_ERROR_STACK_OVERFLOW: return "Stack overflow";
        case VM_ERROR_STACK_UNDERFLOW: return "Stack underflow";
        case VM_ERROR_INVALID_OPCODE: return "Invalid opcode";
        case VM_ERROR_MEMORY_ACCESS: return "Memory access error";
        case VM_ERROR_DIVISION_BY_ZERO: return "Division by zero";
        case VM_ERROR_PROGRAM_COUNTER: return "Program counter error";
        case VM_ERROR_RETURN_STACK_OVERFLOW: return "Return stack overflow";
        case VM_ERROR_RETURN_STACK_UNDERFLOW: return "Return stack underflow";
        case VM_ERROR_NULL_POINTER: return "Null pointer error";
        case VM_ERROR_INVALID_SIZE: return "Invalid size";
        default: return "Unknown error";
    }
}

vm_error_t vm_get_last_error(miniforth_vm_t *vm) {
    return vm ? vm->last_error : VM_ERROR_NULL_POINTER;
}

// Utility functions
void vm_reset(miniforth_vm_t *vm) {
    if (!vm) return;
    
    vm->pc = 0;
    vm->sp = 0;
    vm->rp = 0;
    vm->last_error = VM_OK;
    
    // Clear stacks
    memset(vm->data_stack, 0, vm->stack_size * sizeof(uint16_t));
    memset(vm->return_stack, 0, vm->stack_size * sizeof(uint16_t));
}

int vm_is_running(const miniforth_vm_t *vm) {
    if (!vm) return 0;
    return vm->pc < vm->code_size && vm->last_error == VM_OK;
}

#if MINIFORTH_ENABLE_DEBUGGING
void vm_dump_state(const miniforth_vm_t *vm) {
    if (!vm) return;
    
    printf("VM State:\n");
    printf("  PC: %u\n", vm->pc);
    printf("  SP: %u\n", vm->sp);
    printf("  RP: %u\n", vm->rp);
    printf("  Last Error: %s\n", vm_error_string(vm->last_error));
    
    if (vm->pc < vm->code_size) {
        printf("  Current Instruction: 0x%04X\n", vm->code_memory[vm->pc]);
    }
}

void vm_dump_stack(const miniforth_vm_t *vm) {
    if (!vm) return;
    
    printf("Data Stack (SP=%u):\n", vm->sp);
    for (int i = vm->sp - 1; i >= 0; i--) {
        printf("  [%d]: %u (0x%04X)\n", i, vm->data_stack[i], vm->data_stack[i]);
    }
    
    printf("Return Stack (RP=%u):\n", vm->rp);
    for (int i = vm->rp - 1; i >= 0; i--) {
        printf("  [%d]: %u (0x%04X)\n", i, vm->return_stack[i], vm->return_stack[i]);
    }
}

void vm_dump_memory(const miniforth_vm_t *vm, uint16_t start, uint16_t count) {
    if (!vm) return;
    
    printf("Memory dump from 0x%04X (%u words):\n", start, count);
    for (uint16_t i = 0; i < count && (start + i) < vm->data_size; i++) {
        if (i % 8 == 0) {
            printf("  %04X: ", start + i);
        }
        printf("%04X ", vm->data_memory[start + i]);
        if ((i + 1) % 8 == 0) {
            printf("\n");
        }
    }
    printf("\n");
}
#endif