// Portable MiniForth VM - maximum size optimization without compiler
// assumptions Works with any C89/C99 compiler on any architecture

typedef unsigned char u8;
typedef unsigned short u16;

// Portable bit manipulation macros (no bit fields for compatibility)
#define GET_PC(state) ((state) & 0x1F)        /* 5-bit PC (0-31) */
#define GET_SP(state) (((state) >> 5) & 0x07) /* 3-bit SP (0-7) */
#define SET_PC(state, pc) ((state) = ((state) & ~0x1F) | ((pc) & 0x1F))
#define SET_SP(state, sp) ((state) = ((state) & ~0xE0) | (((sp) & 0x07) << 5))

// Portable VM state - single array for maximum compatibility
static u8 vm[32]; /* 24 code + 8 stack */
static u8 state;  /* packed PC and SP */

/* Portable instruction handler using function pointer array */
typedef u8 (*handler_func)(void);

/* Forward declarations */
static u8 op_exit(void);
static u8 op_lit(void);
static u8 op_add(void);
static u8 op_sub(void);
static u8 op_dup(void);
static u8 op_drop(void);
static u8 op_swap(void);
static u8 op_nop(void);

/* Portable lookup table - works on any architecture */
static const handler_func handlers[8] = {op_exit, op_lit,  op_add,  op_sub,
                                         op_dup,  op_drop, op_swap, op_nop};

/* Instruction implementations */
static u8 op_exit(void) { return 0; }

static u8 op_lit(void) {
  u8 pc = GET_PC(state);
  u8 sp = GET_SP(state);
  if (sp >= 8)
    return 1;
  if (++pc >= 24)
    return 1;
  vm[24 + sp] = vm[pc];
  SET_PC(state, pc + 1);
  SET_SP(state, sp + 1);
  return 2; /* continue */
}

static u8 op_add(void) {
  u8 sp = GET_SP(state);
  if (sp < 2)
    return 1;
  vm[24 + sp - 2] += vm[24 + sp - 1];
  SET_PC(state, GET_PC(state) + 1);
  SET_SP(state, sp - 1);
  return 2; /* continue */
}

static u8 op_sub(void) {
  u8 sp = GET_SP(state);
  if (sp < 2)
    return 1;
  vm[24 + sp - 2] -= vm[24 + sp - 1];
  SET_PC(state, GET_PC(state) + 1);
  SET_SP(state, sp - 1);
  return 2; /* continue */
}

static u8 op_dup(void) {
  u8 sp = GET_SP(state);
  if (sp == 0 || sp >= 8)
    return 1;
  vm[24 + sp] = vm[24 + sp - 1];
  SET_PC(state, GET_PC(state) + 1);
  SET_SP(state, sp + 1);
  return 2; /* continue */
}

static u8 op_drop(void) {
  u8 sp = GET_SP(state);
  if (sp == 0)
    return 1;
  SET_PC(state, GET_PC(state) + 1);
  SET_SP(state, sp - 1);
  return 2; /* continue */
}

static u8 op_swap(void) {
  u8 sp = GET_SP(state);
  u8 temp;
  if (sp < 2)
    return 1;
  temp = vm[24 + sp - 1];
  vm[24 + sp - 1] = vm[24 + sp - 2];
  vm[24 + sp - 2] = temp;
  SET_PC(state, GET_PC(state) + 1);
  return 2; /* continue */
}

static u8 op_nop(void) {
  SET_PC(state, GET_PC(state) + 1);
  return 2; /* continue */
}

/* Main portable interpreter */
u8 forth_portable(const u8 *prog, u8 len) {
  u8 i;

  /* Initialize VM */
  for (i = 0; i < 32; i++)
    vm[i] = 0;
  state = 0;

  /* Load program */
  if (len > 24)
    len = 24;
  for (i = 0; i < len; i++)
    vm[i] = prog[i];

  /* Execute using portable dispatch */
  while (GET_PC(state) < 24) {
    u8 op = vm[GET_PC(state)] & 7;
    if (op >= 8)
      return 1; /* safety check */

    u8 result = handlers[op]();
    if (result == 0)
      return 0; /* exit */
    if (result == 1)
      return 1; /* error */
                /* result == 2: continue */
  }

  return 0;
}