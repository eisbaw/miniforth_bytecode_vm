// Assembly-style MiniForth VM in C - targeting absolute minimal .text
// Uses assembly optimization techniques adapted to C

typedef unsigned short u16;
typedef unsigned char u8;

// Single global state array - most compact representation
static u16 vm[64+16+8];  // code[64] + stack[16] + misc[8]
#define CODE   vm
#define STACK  (vm+64)
#define PC     vm[80]
#define SP     vm[81]

// Assembly-style macros
#define PUSH(x) (STACK[SP++] = (x))
#define POP()   (STACK[--SP])

// Ultra-minimal forth with direct address computation
u8 forth(const u16 *prog, u8 len) {
    // Clear state
    u8 i;
    for (i = 0; i < 82; i++) vm[i] = 0;
    
    // Load program
    for (i = 0; i < len && i < 64; i++) CODE[i] = prog[i];
    
    // Execute with minimal dispatch
    while (PC < 64) {
        u16 op = CODE[PC];
        
        switch (op & 7) {  // 3-bit opcodes for density
        case 0: return 0;  // EXIT
        case 1: PUSH(CODE[++PC]); break;  // LIT
        case 2: {  // ADD
            u16 b = POP();
            STACK[SP-1] += b;
            break;
        }
        case 3: {  // SUB
            u16 b = POP();
            STACK[SP-1] -= b;
            break;
        }  
        case 4: PUSH(STACK[SP-1]); break;  // DUP
        case 5: SP--; break;  // DROP
        case 6: {  // SWAP
            u16 t = STACK[SP-1];
            STACK[SP-1] = STACK[SP-2];
            STACK[SP-2] = t;
            break;
        }
        default: return 1;  // ERROR
        }
        PC++;
    }
    return 0;
}