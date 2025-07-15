#ifndef MINIFORTH_H
#define MINIFORTH_H

#include <stdint.h>
#include <stddef.h>

// Compile-time configuration
#define MINIFORTH_WORD_SIZE 16
#define MINIFORTH_DEFAULT_CODE_SIZE 512
#define MINIFORTH_DEFAULT_DATA_SIZE 256
#define MINIFORTH_DEFAULT_STACK_SIZE 32

// Feature flags
#define MINIFORTH_ENABLE_DEBUGGING 1
#define MINIFORTH_ENABLE_BOUNDS_CHECK 1
#define MINIFORTH_ENABLE_FLOAT 0

// Instruction opcodes
typedef enum {
    OP_EXIT = 0x00,    // Return from word
    OP_LIT = 0x01,     // Push literal value
    OP_DUP = 0x02,     // Duplicate top of stack
    OP_DROP = 0x03,    // Remove top of stack
    OP_SWAP = 0x04,    // Exchange top two items
    OP_ADD = 0x05,     // Addition
    OP_SUB = 0x06,     // Subtraction
    OP_MUL = 0x07,     // Multiplication
    OP_DIV = 0x08,     // Division
    OP_EQ = 0x09,      // Equality test
    OP_LT = 0x0A,      // Less than test
    OP_FETCH = 0x0B,   // Fetch from memory (@)
    OP_STORE = 0x0C,   // Store to memory (!)
    OP_JMP = 0x0D,     // Unconditional jump
    OP_JZ = 0x0E,      // Jump if zero
    OP_CALL = 0x0F     // Call subroutine
} opcode_t;

// Error codes
typedef enum {
    VM_OK = 0,
    VM_ERROR_STACK_OVERFLOW,
    VM_ERROR_STACK_UNDERFLOW,
    VM_ERROR_INVALID_OPCODE,
    VM_ERROR_MEMORY_ACCESS,
    VM_ERROR_DIVISION_BY_ZERO,
    VM_ERROR_PROGRAM_COUNTER,
    VM_ERROR_RETURN_STACK_OVERFLOW,
    VM_ERROR_RETURN_STACK_UNDERFLOW,
    VM_ERROR_NULL_POINTER,
    VM_ERROR_INVALID_SIZE
} vm_error_t;

// VM structure
typedef struct {
    uint16_t *code_memory;     // Program storage
    uint16_t *data_memory;     // Variable storage
    uint16_t *data_stack;      // Computation stack
    uint16_t *return_stack;    // Call stack
    
    uint16_t pc;               // Program counter
    uint16_t sp;               // Stack pointer
    uint16_t rp;               // Return stack pointer
    
    uint16_t code_size;        // Code memory size
    uint16_t data_size;        // Data memory size
    uint16_t stack_size;       // Stack size
    
    vm_error_t last_error;     // Last error code
} miniforth_vm_t;

// VM lifecycle functions
miniforth_vm_t* vm_create(uint16_t code_size, uint16_t data_size, uint16_t stack_size);
void vm_destroy(miniforth_vm_t *vm);
int vm_load_program(miniforth_vm_t *vm, const uint16_t *program, uint16_t size);

// Execution functions
vm_error_t vm_step(miniforth_vm_t *vm);
vm_error_t vm_run(miniforth_vm_t *vm);
vm_error_t vm_run_steps(miniforth_vm_t *vm, int max_steps);

// Memory access functions
uint16_t vm_read_memory(miniforth_vm_t *vm, uint16_t addr);
void vm_write_memory(miniforth_vm_t *vm, uint16_t addr, uint16_t value);
uint8_t vm_read_byte(miniforth_vm_t *vm, uint16_t addr);
void vm_write_byte(miniforth_vm_t *vm, uint16_t addr, uint8_t value);

// Stack operations
vm_error_t vm_push(miniforth_vm_t *vm, uint16_t value);
vm_error_t vm_pop(miniforth_vm_t *vm, uint16_t *value);
vm_error_t vm_peek(miniforth_vm_t *vm, int depth, uint16_t *value);

// Return stack operations
vm_error_t vm_rpush(miniforth_vm_t *vm, uint16_t value);
vm_error_t vm_rpop(miniforth_vm_t *vm, uint16_t *value);

// Debugging support
#if MINIFORTH_ENABLE_DEBUGGING
void vm_dump_state(const miniforth_vm_t *vm);
void vm_dump_stack(const miniforth_vm_t *vm);
void vm_dump_memory(const miniforth_vm_t *vm, uint16_t start, uint16_t count);
#endif

// Error handling
const char* vm_error_string(vm_error_t error);
vm_error_t vm_get_last_error(miniforth_vm_t *vm);

// Utility functions
void vm_reset(miniforth_vm_t *vm);
int vm_is_running(const miniforth_vm_t *vm);

#endif // MINIFORTH_H