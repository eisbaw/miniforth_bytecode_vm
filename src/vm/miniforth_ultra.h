#ifndef MINIFORTH_ULTRA_H
#define MINIFORTH_ULTRA_H

// Ultra-minimal MiniForth VM targeting smallest possible .text size
// Inspired by assembly optimization techniques

// Minimal type definitions for size
typedef unsigned short u16;
typedef unsigned char u8;

// Ultra-compact error handling
typedef u8 vm_error_t;
#define VM_OK 0
#define VM_ERROR 1

// Minimal instruction set - only essential opcodes
#define OP_EXIT 0
#define OP_LIT  1
#define OP_ADD  2
#define OP_SUB  3
#define OP_MUL  4
#define OP_DIV  5
#define OP_DUP  6
#define OP_DROP 7
#define OP_SWAP 8
#define OP_FETCH 9
#define OP_STORE 10
#define OP_JZ   11
#define OP_JMP  12

// Compact VM structure - minimal fields
typedef struct {
    u16 code[128];    // Code memory
    u16 data[64];     // Data memory  
    u16 stack[16];    // Data stack
    u16 rstack[8];    // Return stack
    u16 pc;           // Program counter
    u8 sp;            // Stack pointer
    u8 rp;            // Return stack pointer
} vm_t;

// Single global VM instance to avoid pointer passing
extern vm_t g_vm;

// Ultra-minimal API - only essential functions
vm_error_t vm_init(void);
vm_error_t vm_load(const u16 *prog, u8 size);
vm_error_t vm_run(void);

// Inline stack operations for speed/size
static inline vm_error_t push(u16 val) {
    if (g_vm.sp >= 16) return VM_ERROR;
    g_vm.stack[g_vm.sp++] = val;
    return VM_OK;
}

static inline vm_error_t pop(u16 *val) {
    if (g_vm.sp == 0) return VM_ERROR;
    *val = g_vm.stack[--g_vm.sp];
    return VM_OK;
}

#endif // MINIFORTH_ULTRA_H