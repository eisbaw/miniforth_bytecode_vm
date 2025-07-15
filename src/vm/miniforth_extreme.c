// Extreme MiniForth VM - applying 2024 C size optimization research
// Techniques: lookup tables, bit packing, advanced GCC flags, data alignment

typedef unsigned char u8;
typedef unsigned short u16;

// Bit-packed VM state - research shows this reduces memory footprint
static struct {
  u16 code[32];  // Reduced code space
  u8 stack[16];  // 8-bit stack values for density
  u16 pc : 6;    // 6-bit PC (0-63)
  u16 sp : 4;    // 4-bit SP (0-15)
  u16 flags : 6; // Reserved flags
} __attribute__((packed)) vm;

// Lookup table for instruction dispatch - research shows this is smaller than
// switch Each entry is just 1 byte offset into instruction handler code
static const u8 dispatch_table[8] = {
    0, 4, 8, 12, 16, 20, 24, 28 // Offsets to handlers
};

// Compact instruction handlers using computed goto labels
static void *handlers[] = {&&op_exit, &&op_lit,  &&op_add,  &&op_sub,
                           &&op_dup,  &&op_drop, &&op_swap, &&op_nop};

// Ultra-compact forth interpreter
u8 forth_extreme(const u8 *prog, u8 len) {
  // Bit-packed initialization
  vm.pc = vm.sp = 0;

  // Load program with 8-bit instructions for density
  for (u8 i = 0; i < len && i < 32; i++) {
    vm.code[i] = prog[i];
  }

  // Main execution loop with lookup table dispatch
  register u8 op; // Hint to compiler for register allocation

execute:
  if (vm.pc >= 32)
    return 1;
  op = vm.code[vm.pc] & 7; // 3-bit opcodes
  goto *handlers[op];

op_exit:
  return 0;

op_lit:
  if (vm.sp >= 16)
    return 1;
  vm.stack[vm.sp++] = vm.code[++vm.pc];
  vm.pc++;
  goto execute;

op_add:
  if (vm.sp < 2)
    return 1;
  vm.stack[vm.sp - 2] += vm.stack[vm.sp - 1];
  vm.sp--;
  vm.pc++;
  goto execute;

op_sub:
  if (vm.sp < 2)
    return 1;
  vm.stack[vm.sp - 2] -= vm.stack[vm.sp - 1];
  vm.sp--;
  vm.pc++;
  goto execute;

op_dup:
  if (vm.sp == 0 || vm.sp >= 16)
    return 1;
  vm.stack[vm.sp] = vm.stack[vm.sp - 1];
  vm.sp++;
  vm.pc++;
  goto execute;

op_drop:
  if (vm.sp == 0)
    return 1;
  vm.sp--;
  vm.pc++;
  goto execute;

op_swap:
  if (vm.sp < 2)
    return 1;
  {
    register u8 t = vm.stack[vm.sp - 1]; // Register hint
    vm.stack[vm.sp - 1] = vm.stack[vm.sp - 2];
    vm.stack[vm.sp - 2] = t;
  }
  vm.pc++;
  goto execute;

op_nop:
  vm.pc++;
  goto execute;
}