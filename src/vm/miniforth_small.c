#include "miniforth_small.h"

// Minimal libc replacements for bare-metal
#if MINIFORTH_TARGET_ARM_BAREMETAL

void* vm_memcpy(void *dest, const void *src, size_t n) {
    char *d = (char*)dest;
    const char *s = (const char*)src;
    while (n--) *d++ = *s++;
    return dest;
}

void* vm_memset(void *s, int c, size_t n) {
    char *p = (char*)s;
    while (n--) *p++ = c;
    return s;
}

int vm_strlen(const char *s) {
    int len = 0;
    while (*s++) len++;
    return len;
}

#define memcpy vm_memcpy
#define memset vm_memset
#define malloc(size) ((void*)0)
#define calloc(n, size) ((void*)0)
#define free(ptr) ((void)0)

#else
#include <stdlib.h>
#include <string.h>
#endif

// Static VM instance for bare-metal targets
#if MINIFORTH_USE_STATIC_ALLOCATION
static miniforth_vm_t static_vm_instance;
static int static_vm_initialized = 0;
#endif

// VM lifecycle functions
#if MINIFORTH_USE_STATIC_ALLOCATION
miniforth_vm_t* vm_create_static(void) {
    if (static_vm_initialized) return NULL; // Only one instance allowed
    
    miniforth_vm_t *vm = &static_vm_instance;
    
    // Initialize VM state
    vm->pc = 0;
    vm->sp = 0;
    vm->rp = 0;
    
#if MINIFORTH_SIZE_OPTIMIZE < 2
    vm->last_error = VM_OK;
#endif
    
    // Clear memory areas
    memset(vm->code_memory, 0, sizeof(vm->code_memory));
    memset(vm->data_memory, 0, sizeof(vm->data_memory));
    memset(vm->data_stack, 0, sizeof(vm->data_stack));
    memset(vm->return_stack, 0, sizeof(vm->return_stack));
    
    static_vm_initialized = 1;
    return vm;
}
#else
miniforth_vm_t* vm_create(uint16_t code_size, uint16_t data_size, uint16_t stack_size) {
    if (code_size == 0 || data_size == 0 || stack_size == 0) {
        return NULL;
    }
    
    miniforth_vm_t *vm = malloc(sizeof(miniforth_vm_t));
    if (!vm) return NULL;
    
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
    
#if MINIFORTH_SIZE_OPTIMIZE < 2
    vm->last_error = VM_OK;
#endif
    
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
#endif

int vm_load_program(miniforth_vm_t *vm, const uint16_t *program, uint16_t size) {
    if (!vm || !program || size == 0) return -1;
    
#if MINIFORTH_USE_STATIC_ALLOCATION
    if (size > MINIFORTH_DEFAULT_CODE_SIZE) return -1;
#else
    if (size > vm->code_size) return -1;
#endif
    
    memcpy(vm->code_memory, program, size * sizeof(uint16_t));
    vm->pc = 0;
    vm->sp = 0;
    vm->rp = 0;
    
#if MINIFORTH_SIZE_OPTIMIZE < 2
    vm->last_error = VM_OK;
#endif
    
    return 0;
}

// Stack operations with minimal bounds checking
static inline vm_error_t vm_push_fast(miniforth_vm_t *vm, uint16_t value) {
#if MINIFORTH_ENABLE_BOUNDS_CHECK
    #if MINIFORTH_USE_STATIC_ALLOCATION
    if (vm->sp >= MINIFORTH_DEFAULT_STACK_SIZE) {
    #else
    if (vm->sp >= vm->stack_size) {
    #endif
        #if MINIFORTH_SIZE_OPTIMIZE >= 2
        return VM_ERROR;
        #else
        vm->last_error = VM_ERROR_STACK_OVERFLOW;
        return VM_ERROR_STACK_OVERFLOW;
        #endif
    }
#endif
    vm->data_stack[vm->sp++] = value;
    return VM_OK;
}

static inline vm_error_t vm_pop_fast(miniforth_vm_t *vm, uint16_t *value) {
#if MINIFORTH_ENABLE_BOUNDS_CHECK
    if (vm->sp == 0) {
        #if MINIFORTH_SIZE_OPTIMIZE >= 2
        return VM_ERROR;
        #else
        vm->last_error = VM_ERROR_STACK_UNDERFLOW;
        return VM_ERROR_STACK_UNDERFLOW;
        #endif
    }
#endif
    *value = vm->data_stack[--vm->sp];
    return VM_OK;
}

vm_error_t vm_push(miniforth_vm_t *vm, uint16_t value) {
    if (!vm) {
        #if MINIFORTH_SIZE_OPTIMIZE >= 2
        return VM_ERROR;
        #else
        return VM_ERROR_NULL_POINTER;
        #endif
    }
    return vm_push_fast(vm, value);
}

vm_error_t vm_pop(miniforth_vm_t *vm, uint16_t *value) {
    if (!vm || !value) {
        #if MINIFORTH_SIZE_OPTIMIZE >= 2
        return VM_ERROR;
        #else
        return VM_ERROR_NULL_POINTER;
        #endif
    }
    return vm_pop_fast(vm, value);
}

// Compact instruction execution
vm_error_t vm_step(miniforth_vm_t *vm) {
    if (!vm) {
        #if MINIFORTH_SIZE_OPTIMIZE >= 2
        return VM_ERROR;
        #else
        return VM_ERROR_NULL_POINTER;
        #endif
    }
    
    // Check PC bounds
#if MINIFORTH_USE_STATIC_ALLOCATION
    if (vm->pc >= MINIFORTH_DEFAULT_CODE_SIZE) {
#else
    if (vm->pc >= vm->code_size) {
#endif
        #if MINIFORTH_SIZE_OPTIMIZE >= 2
        return VM_ERROR;
        #else
        vm->last_error = VM_ERROR_PROGRAM_COUNTER;
        return VM_ERROR_PROGRAM_COUNTER;
        #endif
    }
    
    uint16_t instruction = vm->code_memory[vm->pc];
    opcode_t opcode = (opcode_t)(instruction & 0xFF);
    
    uint16_t a, b, temp;
    vm_error_t error = VM_OK;
    
    switch (opcode) {
        case OP_EXIT:
            // Return from call or halt
#if MINIFORTH_ENABLE_BOUNDS_CHECK
            if (vm->rp == 0) {
#if MINIFORTH_USE_STATIC_ALLOCATION
                vm->pc = MINIFORTH_DEFAULT_CODE_SIZE;
#else
                vm->pc = vm->code_size;
#endif
                return VM_OK;
            }
#endif
            vm->pc = vm->return_stack[--vm->rp];
            break;
            
        case OP_LIT:
            vm->pc++;
#if MINIFORTH_USE_STATIC_ALLOCATION
            if (vm->pc >= MINIFORTH_DEFAULT_CODE_SIZE) return VM_ERROR;
#else
            if (vm->pc >= vm->code_size) return VM_ERROR;
#endif
            temp = vm->code_memory[vm->pc];
            vm->pc++;
            return vm_push_fast(vm, temp);
            
        case OP_DUP:
            if (vm->sp == 0) return VM_ERROR;
            temp = vm->data_stack[vm->sp - 1];
            vm->pc++;
            return vm_push_fast(vm, temp);
            
        case OP_DROP:
            error = vm_pop_fast(vm, &temp);
            if (error != VM_OK) return error;
            vm->pc++;
            break;
            
        case OP_SWAP:
            error = vm_pop_fast(vm, &a);
            if (error != VM_OK) return error;
            error = vm_pop_fast(vm, &b);
            if (error != VM_OK) return error;
            error = vm_push_fast(vm, a);
            if (error != VM_OK) return error;
            error = vm_push_fast(vm, b);
            if (error != VM_OK) return error;
            vm->pc++;
            break;
            
        case OP_ADD:
            error = vm_pop_fast(vm, &b);
            if (error != VM_OK) return error;
            error = vm_pop_fast(vm, &a);
            if (error != VM_OK) return error;
            error = vm_push_fast(vm, a + b);
            if (error != VM_OK) return error;
            vm->pc++;
            break;
            
        case OP_SUB:
            error = vm_pop_fast(vm, &b);
            if (error != VM_OK) return error;
            error = vm_pop_fast(vm, &a);
            if (error != VM_OK) return error;
            error = vm_push_fast(vm, a - b);
            if (error != VM_OK) return error;
            vm->pc++;
            break;
            
        case OP_MUL:
            error = vm_pop_fast(vm, &b);
            if (error != VM_OK) return error;
            error = vm_pop_fast(vm, &a);
            if (error != VM_OK) return error;
            error = vm_push_fast(vm, a * b);
            if (error != VM_OK) return error;
            vm->pc++;
            break;
            
        case OP_DIV:
            error = vm_pop_fast(vm, &b);
            if (error != VM_OK) return error;
            error = vm_pop_fast(vm, &a);
            if (error != VM_OK) return error;
            if (b == 0) {
                #if MINIFORTH_SIZE_OPTIMIZE >= 2
                return VM_ERROR;
                #else
                vm->last_error = VM_ERROR_DIVISION_BY_ZERO;
                return VM_ERROR_DIVISION_BY_ZERO;
                #endif
            }
            error = vm_push_fast(vm, a / b);
            if (error != VM_OK) return error;
            vm->pc++;
            break;
            
        case OP_EQ:
            error = vm_pop_fast(vm, &b);
            if (error != VM_OK) return error;
            error = vm_pop_fast(vm, &a);
            if (error != VM_OK) return error;
            error = vm_push_fast(vm, (a == b) ? 1 : 0);
            if (error != VM_OK) return error;
            vm->pc++;
            break;
            
        case OP_LT:
            error = vm_pop_fast(vm, &b);
            if (error != VM_OK) return error;
            error = vm_pop_fast(vm, &a);
            if (error != VM_OK) return error;
            error = vm_push_fast(vm, (a < b) ? 1 : 0);
            if (error != VM_OK) return error;
            vm->pc++;
            break;
            
        case OP_FETCH:
            error = vm_pop_fast(vm, &a);
            if (error != VM_OK) return error;
#if MINIFORTH_USE_STATIC_ALLOCATION
            if (a >= MINIFORTH_DEFAULT_DATA_SIZE) return VM_ERROR;
#else
            if (a >= vm->data_size) return VM_ERROR;
#endif
            error = vm_push_fast(vm, vm->data_memory[a]);
            if (error != VM_OK) return error;
            vm->pc++;
            break;
            
        case OP_STORE:
            error = vm_pop_fast(vm, &a);
            if (error != VM_OK) return error;
            error = vm_pop_fast(vm, &b);
            if (error != VM_OK) return error;
#if MINIFORTH_USE_STATIC_ALLOCATION
            if (a >= MINIFORTH_DEFAULT_DATA_SIZE) return VM_ERROR;
#else
            if (a >= vm->data_size) return VM_ERROR;
#endif
            vm->data_memory[a] = b;
            vm->pc++;
            break;
            
        case OP_JMP:
            vm->pc++;
#if MINIFORTH_USE_STATIC_ALLOCATION
            if (vm->pc >= MINIFORTH_DEFAULT_CODE_SIZE) return VM_ERROR;
            temp = vm->code_memory[vm->pc];
            if (temp >= MINIFORTH_DEFAULT_CODE_SIZE) return VM_ERROR;
#else
            if (vm->pc >= vm->code_size) return VM_ERROR;
            temp = vm->code_memory[vm->pc];
            if (temp >= vm->code_size) return VM_ERROR;
#endif
            vm->pc = temp;
            break;
            
        case OP_JZ:
            error = vm_pop_fast(vm, &a);
            if (error != VM_OK) return error;
            vm->pc++;
#if MINIFORTH_USE_STATIC_ALLOCATION
            if (vm->pc >= MINIFORTH_DEFAULT_CODE_SIZE) return VM_ERROR;
            temp = vm->code_memory[vm->pc];
            if (temp >= MINIFORTH_DEFAULT_CODE_SIZE) return VM_ERROR;
#else
            if (vm->pc >= vm->code_size) return VM_ERROR;
            temp = vm->code_memory[vm->pc];
            if (temp >= vm->code_size) return VM_ERROR;
#endif
            if (a == 0) {
                vm->pc = temp;
            } else {
                vm->pc++;
            }
            break;
            
        case OP_CALL:
            vm->pc++;
#if MINIFORTH_USE_STATIC_ALLOCATION
            if (vm->pc >= MINIFORTH_DEFAULT_CODE_SIZE) return VM_ERROR;
            temp = vm->code_memory[vm->pc];
            if (temp >= MINIFORTH_DEFAULT_CODE_SIZE) return VM_ERROR;
            if (vm->rp >= MINIFORTH_DEFAULT_STACK_SIZE) return VM_ERROR;
#else
            if (vm->pc >= vm->code_size) return VM_ERROR;
            temp = vm->code_memory[vm->pc];
            if (temp >= vm->code_size) return VM_ERROR;
            if (vm->rp >= vm->stack_size) return VM_ERROR;
#endif
            vm->pc++;
            vm->return_stack[vm->rp++] = vm->pc;
            vm->pc = temp;
            break;
            
        default:
            #if MINIFORTH_SIZE_OPTIMIZE >= 2
            return VM_ERROR;
            #else
            vm->last_error = VM_ERROR_INVALID_OPCODE;
            return VM_ERROR_INVALID_OPCODE;
            #endif
    }
    
    return VM_OK;
}

vm_error_t vm_run(miniforth_vm_t *vm) {
    if (!vm) {
        #if MINIFORTH_SIZE_OPTIMIZE >= 2
        return VM_ERROR;
        #else
        return VM_ERROR_NULL_POINTER;
        #endif
    }
    
    vm_error_t error = VM_OK;
    while (1) {
        // Check if we should continue running
#if MINIFORTH_USE_STATIC_ALLOCATION
        if (vm->pc >= MINIFORTH_DEFAULT_CODE_SIZE) break;
#else
        if (vm->pc >= vm->code_size) break;
#endif
        
#if MINIFORTH_SIZE_OPTIMIZE < 2
        if (vm->last_error != VM_OK) break;
#endif
        
        error = vm_step(vm);
        if (error != VM_OK) break;
    }
    
    return error;
}

// Optional utility functions
#if MINIFORTH_ENABLE_UTILITY_FUNCTIONS

vm_error_t vm_peek(miniforth_vm_t *vm, int depth, uint16_t *value) {
    if (!vm || !value || depth < 0 || vm->sp <= depth) {
        #if MINIFORTH_SIZE_OPTIMIZE >= 2
        return VM_ERROR;
        #else
        return VM_ERROR_STACK_UNDERFLOW;
        #endif
    }
    *value = vm->data_stack[vm->sp - 1 - depth];
    return VM_OK;
}

vm_error_t vm_rpush(miniforth_vm_t *vm, uint16_t value) {
    if (!vm) {
        #if MINIFORTH_SIZE_OPTIMIZE >= 2
        return VM_ERROR;
        #else
        return VM_ERROR_NULL_POINTER;
        #endif
    }
    
#if MINIFORTH_ENABLE_BOUNDS_CHECK
    #if MINIFORTH_USE_STATIC_ALLOCATION
    if (vm->rp >= MINIFORTH_DEFAULT_STACK_SIZE) {
    #else
    if (vm->rp >= vm->stack_size) {
    #endif
        #if MINIFORTH_SIZE_OPTIMIZE >= 2
        return VM_ERROR;
        #else
        vm->last_error = VM_ERROR_RETURN_STACK_OVERFLOW;
        return VM_ERROR_RETURN_STACK_OVERFLOW;
        #endif
    }
#endif
    
    vm->return_stack[vm->rp++] = value;
    return VM_OK;
}

vm_error_t vm_rpop(miniforth_vm_t *vm, uint16_t *value) {
    if (!vm || !value) {
        #if MINIFORTH_SIZE_OPTIMIZE >= 2
        return VM_ERROR;
        #else
        return VM_ERROR_NULL_POINTER;
        #endif
    }
    
#if MINIFORTH_ENABLE_BOUNDS_CHECK
    if (vm->rp == 0) {
        #if MINIFORTH_SIZE_OPTIMIZE >= 2
        return VM_ERROR;
        #else
        vm->last_error = VM_ERROR_RETURN_STACK_UNDERFLOW;
        return VM_ERROR_RETURN_STACK_UNDERFLOW;
        #endif
    }
#endif
    
    *value = vm->return_stack[--vm->rp];
    return VM_OK;
}

void vm_reset(miniforth_vm_t *vm) {
    if (!vm) return;
    
    vm->pc = 0;
    vm->sp = 0;
    vm->rp = 0;
    
#if MINIFORTH_SIZE_OPTIMIZE < 2
    vm->last_error = VM_OK;
#endif
    
    // Clear stacks
#if MINIFORTH_USE_STATIC_ALLOCATION
    memset(vm->data_stack, 0, sizeof(vm->data_stack));
    memset(vm->return_stack, 0, sizeof(vm->return_stack));
#else
    memset(vm->data_stack, 0, vm->stack_size * sizeof(uint16_t));
    memset(vm->return_stack, 0, vm->stack_size * sizeof(uint16_t));
#endif
}

int vm_is_running(const miniforth_vm_t *vm) {
    if (!vm) return 0;
#if MINIFORTH_USE_STATIC_ALLOCATION
    int running = vm->pc < MINIFORTH_DEFAULT_CODE_SIZE;
#else
    int running = vm->pc < vm->code_size;
#endif
#if MINIFORTH_SIZE_OPTIMIZE < 2
    running = running && vm->last_error == VM_OK;
#endif
    return running;
}

#endif // MINIFORTH_ENABLE_UTILITY_FUNCTIONS

// Memory access functions
#if MINIFORTH_ENABLE_BYTE_ACCESS || MINIFORTH_SIZE_OPTIMIZE < 2

uint16_t vm_read_memory(miniforth_vm_t *vm, uint16_t addr) {
#if MINIFORTH_USE_STATIC_ALLOCATION
    if (!vm || addr >= MINIFORTH_DEFAULT_DATA_SIZE) {
#else
    if (!vm || addr >= vm->data_size) {
#endif
#if MINIFORTH_SIZE_OPTIMIZE < 2
        if (vm) vm->last_error = VM_ERROR_MEMORY_ACCESS;
#endif
        return 0;
    }
    return vm->data_memory[addr];
}

void vm_write_memory(miniforth_vm_t *vm, uint16_t addr, uint16_t value) {
#if MINIFORTH_USE_STATIC_ALLOCATION
    if (!vm || addr >= MINIFORTH_DEFAULT_DATA_SIZE) {
#else
    if (!vm || addr >= vm->data_size) {
#endif
#if MINIFORTH_SIZE_OPTIMIZE < 2
        if (vm) vm->last_error = VM_ERROR_MEMORY_ACCESS;
#endif
        return;
    }
    vm->data_memory[addr] = value;
}

#endif

// Error handling
#if MINIFORTH_ENABLE_ERROR_STRINGS && !MINIFORTH_TARGET_ARM_BAREMETAL
const char* vm_error_string(vm_error_t error) {
    switch (error) {
        case VM_OK: return "No error";
        #if MINIFORTH_SIZE_OPTIMIZE < 2
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
        #else
        case VM_ERROR: return "Error";
        #endif
        default: return "Unknown error";
    }
}
#endif

#if MINIFORTH_SIZE_OPTIMIZE < 2
vm_error_t vm_get_last_error(miniforth_vm_t *vm) {
    return vm ? vm->last_error : VM_ERROR_NULL_POINTER;
}
#endif