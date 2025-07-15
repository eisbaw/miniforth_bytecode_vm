// Direct-threaded MiniForth VM - minimal dispatch overhead
// Inspired by assembly forth direct threading techniques

typedef unsigned short u16;
typedef unsigned char u8;
typedef void (*word_t)(void);

// VM state
static u16 stack[8], data[32];
static u16 *ip; // Instruction pointer
static u8 sp;

// Forward declarations
static void w_exit(void), w_lit(void), w_add(void), w_sub(void);
static void w_dup(void), w_drop(void), w_swap(void);

// Word table for direct threading
static const word_t words[] = {w_exit, w_lit,  w_add, w_sub,
                               w_dup,  w_drop, w_swap};

// Direct execution - no switch/case overhead
static inline void execute(u16 word_idx) {
  if (word_idx < 7)
    words[word_idx]();
}

// Core words implementation
static void w_exit(void) {
  ip = 0; // Signal end
}

static void w_lit(void) {
  if (sp < 8)
    stack[sp++] = *ip++;
}

static void w_add(void) {
  if (sp >= 2) {
    sp--;
    stack[sp - 1] += stack[sp];
  }
}

static void w_sub(void) {
  if (sp >= 2) {
    sp--;
    stack[sp - 1] -= stack[sp];
  }
}

static void w_dup(void) {
  if (sp > 0 && sp < 8) {
    stack[sp] = stack[sp - 1];
    sp++;
  }
}

static void w_drop(void) {
  if (sp > 0)
    sp--;
}

static void w_swap(void) {
  if (sp >= 2) {
    u16 tmp = stack[sp - 1];
    stack[sp - 1] = stack[sp - 2];
    stack[sp - 2] = tmp;
  }
}

// Minimal forth interpreter
u8 forth_exec(u16 *program) {
  ip = program;
  sp = 0;

  while (ip) {
    execute(*ip++);
  }

  return 0;
}