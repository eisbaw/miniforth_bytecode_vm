#include "miniforth_ultra.h"

// Global VM instance
vm_t g_vm;

// Initialize VM
vm_error_t vm_init(void) {
  // Clear VM state
  u8 i;
  for (i = 0; i < 128; i++)
    g_vm.code[i] = 0;
  for (i = 0; i < 64; i++)
    g_vm.data[i] = 0;
  for (i = 0; i < 16; i++)
    g_vm.stack[i] = 0;
  for (i = 0; i < 8; i++)
    g_vm.rstack[i] = 0;

  g_vm.pc = 0;
  g_vm.sp = 0;
  g_vm.rp = 0;
  return VM_OK;
}

// Load program
vm_error_t vm_load(const u16 *prog, u8 size) {
  if (size > 128)
    return VM_ERROR;

  u8 i;
  for (i = 0; i < size; i++) {
    g_vm.code[i] = prog[i];
  }

  g_vm.pc = 0;
  g_vm.sp = 0;
  g_vm.rp = 0;
  return VM_OK;
}

// Ultra-compact VM execution loop using computed goto for minimal dispatch
// overhead
vm_error_t vm_run(void) {
  // Use computed goto for fastest dispatch (GCC extension)
  static const void *dispatch[] = {&&op_exit, &&op_lit,   &&op_add,   &&op_sub,
                                   &&op_mul,  &&op_div,   &&op_dup,   &&op_drop,
                                   &&op_swap, &&op_fetch, &&op_store, &&op_jz,
                                   &&op_jmp};

  u16 a, b;
  u8 opcode;

  // Main execution loop
next:
  if (g_vm.pc >= 128)
    return VM_ERROR;
  opcode = g_vm.code[g_vm.pc] & 0xFF;
  if (opcode > 12)
    return VM_ERROR;
  goto *dispatch[opcode];

op_exit:
  if (g_vm.rp == 0)
    return VM_OK;
  g_vm.pc = g_vm.rstack[--g_vm.rp];
  goto next;

op_lit:
  g_vm.pc++;
  if (g_vm.pc >= 128)
    return VM_ERROR;
  if (push(g_vm.code[g_vm.pc]) != VM_OK)
    return VM_ERROR;
  g_vm.pc++;
  goto next;

op_add:
  if (pop(&b) != VM_OK)
    return VM_ERROR;
  if (pop(&a) != VM_OK)
    return VM_ERROR;
  if (push(a + b) != VM_OK)
    return VM_ERROR;
  g_vm.pc++;
  goto next;

op_sub:
  if (pop(&b) != VM_OK)
    return VM_ERROR;
  if (pop(&a) != VM_OK)
    return VM_ERROR;
  if (push(a - b) != VM_OK)
    return VM_ERROR;
  g_vm.pc++;
  goto next;

op_mul:
  if (pop(&b) != VM_OK)
    return VM_ERROR;
  if (pop(&a) != VM_OK)
    return VM_ERROR;
  if (push(a * b) != VM_OK)
    return VM_ERROR;
  g_vm.pc++;
  goto next;

op_div:
  if (pop(&b) != VM_OK)
    return VM_ERROR;
  if (pop(&a) != VM_OK)
    return VM_ERROR;
  if (b == 0)
    return VM_ERROR;
  if (push(a / b) != VM_OK)
    return VM_ERROR;
  g_vm.pc++;
  goto next;

op_dup:
  if (g_vm.sp == 0)
    return VM_ERROR;
  a = g_vm.stack[g_vm.sp - 1];
  if (push(a) != VM_OK)
    return VM_ERROR;
  g_vm.pc++;
  goto next;

op_drop:
  if (pop(&a) != VM_OK)
    return VM_ERROR;
  g_vm.pc++;
  goto next;

op_swap:
  if (pop(&a) != VM_OK)
    return VM_ERROR;
  if (pop(&b) != VM_OK)
    return VM_ERROR;
  if (push(a) != VM_OK)
    return VM_ERROR;
  if (push(b) != VM_OK)
    return VM_ERROR;
  g_vm.pc++;
  goto next;

op_fetch:
  if (pop(&a) != VM_OK)
    return VM_ERROR;
  if (a >= 64)
    return VM_ERROR;
  if (push(g_vm.data[a]) != VM_OK)
    return VM_ERROR;
  g_vm.pc++;
  goto next;

op_store:
  if (pop(&a) != VM_OK)
    return VM_ERROR;
  if (pop(&b) != VM_OK)
    return VM_ERROR;
  if (a >= 64)
    return VM_ERROR;
  g_vm.data[a] = b;
  g_vm.pc++;
  goto next;

op_jz:
  if (pop(&a) != VM_OK)
    return VM_ERROR;
  g_vm.pc++;
  if (g_vm.pc >= 128)
    return VM_ERROR;
  if (a == 0) {
    g_vm.pc = g_vm.code[g_vm.pc];
  } else {
    g_vm.pc++;
  }
  goto next;

op_jmp:
  g_vm.pc++;
  if (g_vm.pc >= 128)
    return VM_ERROR;
  g_vm.pc = g_vm.code[g_vm.pc];
  goto next;
}