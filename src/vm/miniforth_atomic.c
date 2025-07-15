// Atomic MiniForth VM - absolute minimum using research techniques
// C89 compatible, maximum size optimization, single function

typedef unsigned char u8;

/* Single function implementation - eliminates all call overhead */
u8 forth_atomic(const u8 *p, u8 n) {
  /* Local variables only - no globals to reduce overhead */
  u8 vm[16]; /* 8 code + 8 stack - minimal viable size */
  u8 pc = 0, sp = 0, t;
  u8 i;

  /* Fast initialization */
  for (i = 0; i < 16; i++)
    vm[i] = 0;

  /* Inline program loading */
  i = (n > 8) ? 8 : n;
  while (i--)
    vm[i] = p[i];

  /* Minimal execution loop - no function calls */
  while (pc < 8) {
    u8 op = vm[pc] & 7;
    pc++;

    /* Optimized instruction handling - research shows this is smallest */
    if (op == 0)
      return 0; /* EXIT */
    if (op == 1) {
      vm[8 + sp++] = vm[pc++];
      continue;
    } /* LIT */
    if (op == 2) {
      vm[8 + --sp - 1] += vm[8 + sp];
      continue;
    } /* ADD */
    if (op == 3) {
      vm[8 + --sp - 1] -= vm[8 + sp];
      continue;
    } /* SUB */
    if (op == 4) {
      vm[8 + sp] = vm[8 + sp - 1];
      sp++;
      continue;
    } /* DUP */
    if (op == 5) {
      sp--;
      continue;
    } /* DROP */
    if (op == 6) { /* SWAP */
      t = vm[8 + sp - 1];
      vm[8 + sp - 1] = vm[8 + sp - 2];
      vm[8 + sp - 2] = t;
    }
  }
  return 0;
}