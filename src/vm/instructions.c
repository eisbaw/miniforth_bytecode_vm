#include "miniforth.h"
#include <stdio.h>

// Forward declarations
static vm_error_t exec_exit(miniforth_vm_t *vm);
static vm_error_t exec_lit(miniforth_vm_t *vm);
static vm_error_t exec_dup(miniforth_vm_t *vm);
static vm_error_t exec_drop(miniforth_vm_t *vm);
static vm_error_t exec_swap(miniforth_vm_t *vm);
static vm_error_t exec_add(miniforth_vm_t *vm);
static vm_error_t exec_sub(miniforth_vm_t *vm);
static vm_error_t exec_mul(miniforth_vm_t *vm);
static vm_error_t exec_div(miniforth_vm_t *vm);
static vm_error_t exec_eq(miniforth_vm_t *vm);
static vm_error_t exec_lt(miniforth_vm_t *vm);
static vm_error_t exec_fetch(miniforth_vm_t *vm);
static vm_error_t exec_store(miniforth_vm_t *vm);
static vm_error_t exec_jmp(miniforth_vm_t *vm);
static vm_error_t exec_jz(miniforth_vm_t *vm);
static vm_error_t exec_call(miniforth_vm_t *vm);

// Instruction execution function
vm_error_t vm_step(miniforth_vm_t *vm) {
  if (!vm)
    return VM_ERROR_NULL_POINTER;

  // Check if we can execute
  if (vm->pc >= vm->code_size) {
    vm->last_error = VM_ERROR_PROGRAM_COUNTER;
    return VM_ERROR_PROGRAM_COUNTER;
  }

  // Get current instruction
  uint16_t instruction = vm->code_memory[vm->pc];
  opcode_t opcode = (opcode_t)(instruction & 0xFF);

  // Execute instruction
  vm_error_t error = VM_OK;
  switch (opcode) {
  case OP_EXIT:
    error = exec_exit(vm);
    break;
  case OP_LIT:
    error = exec_lit(vm);
    break;
  case OP_DUP:
    error = exec_dup(vm);
    break;
  case OP_DROP:
    error = exec_drop(vm);
    break;
  case OP_SWAP:
    error = exec_swap(vm);
    break;
  case OP_ADD:
    error = exec_add(vm);
    break;
  case OP_SUB:
    error = exec_sub(vm);
    break;
  case OP_MUL:
    error = exec_mul(vm);
    break;
  case OP_DIV:
    error = exec_div(vm);
    break;
  case OP_EQ:
    error = exec_eq(vm);
    break;
  case OP_LT:
    error = exec_lt(vm);
    break;
  case OP_FETCH:
    error = exec_fetch(vm);
    break;
  case OP_STORE:
    error = exec_store(vm);
    break;
  case OP_JMP:
    error = exec_jmp(vm);
    break;
  case OP_JZ:
    error = exec_jz(vm);
    break;
  case OP_CALL:
    error = exec_call(vm);
    break;
  default:
    error = VM_ERROR_INVALID_OPCODE;
    break;
  }

  if (error != VM_OK) {
    vm->last_error = error;
  }

  return error;
}

vm_error_t vm_run(miniforth_vm_t *vm) {
  if (!vm)
    return VM_ERROR_NULL_POINTER;

  vm_error_t error = VM_OK;
  while (vm_is_running(vm)) {
    error = vm_step(vm);
    if (error != VM_OK) {
      break;
    }
  }

  return error;
}

vm_error_t vm_run_steps(miniforth_vm_t *vm, int max_steps) {
  if (!vm)
    return VM_ERROR_NULL_POINTER;

  vm_error_t error = VM_OK;
  int steps = 0;

  while (vm_is_running(vm) && steps < max_steps) {
    error = vm_step(vm);
    if (error != VM_OK) {
      break;
    }
    steps++;
  }

  return error;
}

// Instruction implementations
static vm_error_t exec_exit(miniforth_vm_t *vm) {
  uint16_t return_addr;
  vm_error_t error = vm_rpop(vm, &return_addr);
  if (error != VM_OK) {
    // If return stack is empty, halt execution
    vm->pc = vm->code_size;
    return VM_OK;
  }

  vm->pc = return_addr;
  return VM_OK;
}

static vm_error_t exec_lit(miniforth_vm_t *vm) {
  vm->pc++;
  if (vm->pc >= vm->code_size) {
    return VM_ERROR_PROGRAM_COUNTER;
  }

  uint16_t literal = vm->code_memory[vm->pc];
  vm->pc++;

  return vm_push(vm, literal);
}

static vm_error_t exec_dup(miniforth_vm_t *vm) {
  uint16_t value;
  vm_error_t error = vm_peek(vm, 0, &value);
  if (error != VM_OK) {
    return error;
  }

  vm->pc++;
  return vm_push(vm, value);
}

static vm_error_t exec_drop(miniforth_vm_t *vm) {
  uint16_t value;
  vm_error_t error = vm_pop(vm, &value);
  if (error != VM_OK) {
    return error;
  }

  vm->pc++;
  return VM_OK;
}

static vm_error_t exec_swap(miniforth_vm_t *vm) {
  uint16_t a, b;
  vm_error_t error;

  error = vm_pop(vm, &a);
  if (error != VM_OK)
    return error;

  error = vm_pop(vm, &b);
  if (error != VM_OK)
    return error;

  error = vm_push(vm, a);
  if (error != VM_OK)
    return error;

  error = vm_push(vm, b);
  if (error != VM_OK)
    return error;

  vm->pc++;
  return VM_OK;
}

static vm_error_t exec_add(miniforth_vm_t *vm) {
  uint16_t a, b;
  vm_error_t error;

  error = vm_pop(vm, &b);
  if (error != VM_OK)
    return error;

  error = vm_pop(vm, &a);
  if (error != VM_OK)
    return error;

  error = vm_push(vm, a + b);
  if (error != VM_OK)
    return error;

  vm->pc++;
  return VM_OK;
}

static vm_error_t exec_sub(miniforth_vm_t *vm) {
  uint16_t a, b;
  vm_error_t error;

  error = vm_pop(vm, &b);
  if (error != VM_OK)
    return error;

  error = vm_pop(vm, &a);
  if (error != VM_OK)
    return error;

  error = vm_push(vm, a - b);
  if (error != VM_OK)
    return error;

  vm->pc++;
  return VM_OK;
}

static vm_error_t exec_mul(miniforth_vm_t *vm) {
  uint16_t a, b;
  vm_error_t error;

  error = vm_pop(vm, &b);
  if (error != VM_OK)
    return error;

  error = vm_pop(vm, &a);
  if (error != VM_OK)
    return error;

  error = vm_push(vm, a * b);
  if (error != VM_OK)
    return error;

  vm->pc++;
  return VM_OK;
}

static vm_error_t exec_div(miniforth_vm_t *vm) {
  uint16_t a, b;
  vm_error_t error;

  error = vm_pop(vm, &b);
  if (error != VM_OK)
    return error;

  error = vm_pop(vm, &a);
  if (error != VM_OK)
    return error;

  if (b == 0) {
    return VM_ERROR_DIVISION_BY_ZERO;
  }

  error = vm_push(vm, a / b);
  if (error != VM_OK)
    return error;

  vm->pc++;
  return VM_OK;
}

static vm_error_t exec_eq(miniforth_vm_t *vm) {
  uint16_t a, b;
  vm_error_t error;

  error = vm_pop(vm, &b);
  if (error != VM_OK)
    return error;

  error = vm_pop(vm, &a);
  if (error != VM_OK)
    return error;

  error = vm_push(vm, (a == b) ? 1 : 0);
  if (error != VM_OK)
    return error;

  vm->pc++;
  return VM_OK;
}

static vm_error_t exec_lt(miniforth_vm_t *vm) {
  uint16_t a, b;
  vm_error_t error;

  error = vm_pop(vm, &b);
  if (error != VM_OK)
    return error;

  error = vm_pop(vm, &a);
  if (error != VM_OK)
    return error;

  error = vm_push(vm, (a < b) ? 1 : 0);
  if (error != VM_OK)
    return error;

  vm->pc++;
  return VM_OK;
}

static vm_error_t exec_fetch(miniforth_vm_t *vm) {
  uint16_t addr;
  vm_error_t error = vm_pop(vm, &addr);
  if (error != VM_OK)
    return error;

  uint16_t value = vm_read_memory(vm, addr);
  if (vm->last_error != VM_OK) {
    return vm->last_error;
  }

  error = vm_push(vm, value);
  if (error != VM_OK)
    return error;

  vm->pc++;
  return VM_OK;
}

static vm_error_t exec_store(miniforth_vm_t *vm) {
  uint16_t addr, value;
  vm_error_t error;

  error = vm_pop(vm, &addr);
  if (error != VM_OK)
    return error;

  error = vm_pop(vm, &value);
  if (error != VM_OK)
    return error;

  vm_write_memory(vm, addr, value);
  if (vm->last_error != VM_OK) {
    return vm->last_error;
  }

  vm->pc++;
  return VM_OK;
}

static vm_error_t exec_jmp(miniforth_vm_t *vm) {
  vm->pc++;
  if (vm->pc >= vm->code_size) {
    return VM_ERROR_PROGRAM_COUNTER;
  }

  uint16_t target = vm->code_memory[vm->pc];
  if (target >= vm->code_size) {
    return VM_ERROR_PROGRAM_COUNTER;
  }

  vm->pc = target;
  return VM_OK;
}

static vm_error_t exec_jz(miniforth_vm_t *vm) {
  uint16_t condition;
  vm_error_t error = vm_pop(vm, &condition);
  if (error != VM_OK)
    return error;

  vm->pc++;
  if (vm->pc >= vm->code_size) {
    return VM_ERROR_PROGRAM_COUNTER;
  }

  uint16_t target = vm->code_memory[vm->pc];
  if (target >= vm->code_size) {
    return VM_ERROR_PROGRAM_COUNTER;
  }

  if (condition == 0) {
    vm->pc = target;
  } else {
    vm->pc++;
  }

  return VM_OK;
}

static vm_error_t exec_call(miniforth_vm_t *vm) {
  vm->pc++;
  if (vm->pc >= vm->code_size) {
    return VM_ERROR_PROGRAM_COUNTER;
  }

  uint16_t target = vm->code_memory[vm->pc];
  if (target >= vm->code_size) {
    return VM_ERROR_PROGRAM_COUNTER;
  }

  vm->pc++;
  vm_error_t error = vm_rpush(vm, vm->pc);
  if (error != VM_OK)
    return error;

  vm->pc = target;
  return VM_OK;
}