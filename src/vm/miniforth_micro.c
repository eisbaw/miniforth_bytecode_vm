// Micro MiniForth VM - absolute minimal .text size
// Single-file implementation inspired by 512-byte assembly version

typedef unsigned short u16;
typedef unsigned char u8;

// Global state - single instance, no malloc
static u16 code[64], data[32], stack[8];
static u16 pc, sp;

// Ultra-compact instruction encoding (4-bit opcodes)
#define EXIT 0
#define LIT  1  
#define ADD  2
#define SUB  3
#define DUP  4
#define DROP 5
#define SWAP 6

// Minimal API - single run function
u8 forth_run(const u16 *prog, u8 len) {
    // Load program
    u8 i;
    for (i = 0; i < len && i < 64; i++) code[i] = prog[i];
    
    // Reset state
    pc = sp = 0;
    
    // Execution loop - minimalist dispatch
    u16 a, b, op;
    while (pc < 64) {
        op = code[pc] & 0xF;
        
        switch (op) {
        case EXIT: return 0;
        
        case LIT:
            if (sp >= 8) return 1;
            stack[sp++] = code[++pc];
            break;
            
        case ADD:
            if (sp < 2) return 1;
            b = stack[--sp];
            stack[sp-1] += b;
            break;
            
        case SUB:
            if (sp < 2) return 1;
            b = stack[--sp];
            stack[sp-1] -= b;
            break;
            
        case DUP:
            if (sp == 0 || sp >= 8) return 1;
            stack[sp] = stack[sp-1];
            sp++;
            break;
            
        case DROP:
            if (sp == 0) return 1;
            sp--;
            break;
            
        case SWAP:
            if (sp < 2) return 1;
            a = stack[sp-1];
            stack[sp-1] = stack[sp-2];
            stack[sp-2] = a;
            break;
            
        default:
            return 1;
        }
        pc++;
    }
    return 0;
}