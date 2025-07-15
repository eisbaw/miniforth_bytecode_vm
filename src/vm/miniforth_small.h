#ifndef MINIFORTH_SMALL_H
#define MINIFORTH_SMALL_H

#include "miniforth_config.h"
#include <stdint.h>

// Ensure size optimization level is defined
#ifndef MINIFORTH_SIZE_OPTIMIZE
#define MINIFORTH_SIZE_OPTIMIZE 0
#endif

// Minimal includes for bare-metal
#if MINIFORTH_TARGET_ARM_BAREMETAL
// No standard library includes
#else
#include <stddef.h>
#endif

// Instruction opcodes - compact encoding
typedef enum {
  OP_EXIT = 0x00,  // Return from word
  OP_LIT = 0x01,   // Push literal value
  OP_DUP = 0x02,   // Duplicate top of stack
  OP_DROP = 0x03,  // Remove top of stack
  OP_SWAP = 0x04,  // Exchange top two items
  OP_ADD = 0x05,   // Addition
  OP_SUB = 0x06,   // Subtraction
  OP_MUL = 0x07,   // Multiplication
  OP_DIV = 0x08,   // Division
  OP_EQ = 0x09,    // Equality test
  OP_LT = 0x0A,    // Less than test
  OP_FETCH = 0x0B, // Fetch from memory (@)
  OP_STORE = 0x0C, // Store to memory (!)
  OP_JMP = 0x0D,   // Unconditional jump
  OP_JZ = 0x0E,    // Jump if zero
  OP_CALL = 0x0F   // Call subroutine
} opcode_t;

// Minimal error codes
#if MINIFORTH_SIZE_OPTIMIZE >= 2
typedef enum { VM_OK = 0, VM_ERROR = 1 } vm_error_t;
#else
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
  VM_ERROR_INVALID_SIZE,
  VM_ERROR = VM_ERROR_INVALID_OPCODE // Generic error for compatibility
} vm_error_t;
#endif

// Compact VM structure
typedef struct {
#if MINIFORTH_USE_STATIC_ALLOCATION
  // Static allocation for bare-metal
  uint16_t code_memory[MINIFORTH_DEFAULT_CODE_SIZE];
  uint16_t data_memory[MINIFORTH_DEFAULT_DATA_SIZE];
  uint16_t data_stack[MINIFORTH_DEFAULT_STACK_SIZE];
  uint16_t return_stack[MINIFORTH_DEFAULT_STACK_SIZE];
#else
  // Dynamic allocation for hosted
  uint16_t *code_memory;
  uint16_t *data_memory;
  uint16_t *data_stack;
  uint16_t *return_stack;
#endif

  uint16_t pc; // Program counter
  uint8_t sp;  // Stack pointer (8-bit for small stacks)
  uint8_t rp;  // Return stack pointer (8-bit)

#if !MINIFORTH_USE_STATIC_ALLOCATION
  uint16_t code_size; // Code memory size
  uint16_t data_size; // Data memory size
  uint8_t stack_size; // Stack size (8-bit)
#endif

#if MINIFORTH_SIZE_OPTIMIZE < 2
  vm_error_t last_error; // Last error code
#endif
} miniforth_vm_t;

// Essential function declarations
#if MINIFORTH_USE_STATIC_ALLOCATION
miniforth_vm_t *vm_create_static(void);
#else
miniforth_vm_t *vm_create(uint16_t code_size, uint16_t data_size,
                          uint16_t stack_size);
void vm_destroy(miniforth_vm_t *vm);
#endif

int vm_load_program(miniforth_vm_t *vm, const uint16_t *program, uint16_t size);

// Core execution functions
vm_error_t vm_step(miniforth_vm_t *vm);
vm_error_t vm_run(miniforth_vm_t *vm);

// Essential stack operations
vm_error_t vm_push(miniforth_vm_t *vm, uint16_t value);
vm_error_t vm_pop(miniforth_vm_t *vm, uint16_t *value);

#if MINIFORTH_ENABLE_UTILITY_FUNCTIONS
// Optional utility functions
vm_error_t vm_peek(miniforth_vm_t *vm, int depth, uint16_t *value);
vm_error_t vm_rpush(miniforth_vm_t *vm, uint16_t value);
vm_error_t vm_rpop(miniforth_vm_t *vm, uint16_t *value);
void vm_reset(miniforth_vm_t *vm);
int vm_is_running(miniforth_vm_t *vm);
#endif

// Memory access (if enabled)
#if MINIFORTH_ENABLE_BYTE_ACCESS || MINIFORTH_SIZE_OPTIMIZE < 2
uint16_t vm_read_memory(miniforth_vm_t *vm, uint16_t addr);
void vm_write_memory(miniforth_vm_t *vm, uint16_t addr, uint16_t value);
#endif

#if MINIFORTH_ENABLE_BYTE_ACCESS
uint8_t vm_read_byte(miniforth_vm_t *vm, uint16_t addr);
void vm_write_byte(miniforth_vm_t *vm, uint16_t addr, uint8_t value);
#endif

// Error handling (if enabled)
#if MINIFORTH_ENABLE_ERROR_STRINGS
const char *vm_error_string(vm_error_t error);
#endif

#if MINIFORTH_SIZE_OPTIMIZE < 2
vm_error_t vm_get_last_error(miniforth_vm_t *vm);
#endif

// Debugging support (if enabled)
#if MINIFORTH_ENABLE_DEBUGGING
void vm_dump_state(miniforth_vm_t *vm);
void vm_dump_stack(miniforth_vm_t *vm);
void vm_dump_memory(miniforth_vm_t *vm, uint16_t start, uint16_t count);
#endif

// ARM bare-metal specific functions
#if MINIFORTH_TARGET_ARM_BAREMETAL
void *vm_memcpy(void *dest, const void *src, size_t n);
void *vm_memset(void *s, int c, size_t n);
int vm_strlen(const char *s);
#endif

#endif // MINIFORTH_SMALL_H