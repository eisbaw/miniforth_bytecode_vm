// Minimal Portable MiniForth VM - C89 compatible, any architecture
// No compiler extensions, no assumptions about architecture or endianness

/* Use minimal types for maximum portability */
typedef unsigned char u8;

/* Single global array - most portable approach */
static u8 vm[20];  /* 12 code + 8 stack */

/* Portable minimal forth interpreter */
u8 forth_minimal(const u8 *program, u8 program_size) {
    u8 pc, sp, opcode, temp;
    u8 i;
    
    /* Initialize everything to zero */
    for (i = 0; i < 20; i++) {
        vm[i] = 0;
    }
    pc = 0;
    sp = 0;
    
    /* Load program into first 12 bytes */
    for (i = 0; i < program_size && i < 12; i++) {
        vm[i] = program[i];
    }
    
    /* Main execution loop */
    while (pc < 12) {
        opcode = vm[pc] & 7;  /* 3-bit opcodes for portability */
        
        /* Handle each instruction */
        if (opcode == 0) {
            /* EXIT */
            return 0;
        }
        else if (opcode == 1) {
            /* LIT - push next byte onto stack */
            if (sp >= 8) return 1;  /* stack overflow */
            pc++;
            if (pc >= 12) return 1;  /* program bounds */
            vm[12 + sp] = vm[pc];
            sp++;
        }
        else if (opcode == 2) {
            /* ADD - pop two values, push sum */
            if (sp < 2) return 1;  /* stack underflow */
            sp--;
            vm[12 + sp - 1] += vm[12 + sp];
        }
        else if (opcode == 3) {
            /* SUB - pop two values, push difference */
            if (sp < 2) return 1;  /* stack underflow */
            sp--;
            vm[12 + sp - 1] -= vm[12 + sp];
        }
        else if (opcode == 4) {
            /* DUP - duplicate top of stack */
            if (sp == 0 || sp >= 8) return 1;  /* underflow or overflow */
            vm[12 + sp] = vm[12 + sp - 1];
            sp++;
        }
        else if (opcode == 5) {
            /* DROP - remove top of stack */
            if (sp == 0) return 1;  /* stack underflow */
            sp--;
        }
        else if (opcode == 6) {
            /* SWAP - exchange top two stack items */
            if (sp < 2) return 1;  /* stack underflow */
            temp = vm[12 + sp - 1];
            vm[12 + sp - 1] = vm[12 + sp - 2];
            vm[12 + sp - 2] = temp;
        }
        else {
            /* Invalid opcode */
            return 1;
        }
        
        pc++;
    }
    
    return 0;  /* normal termination */
}