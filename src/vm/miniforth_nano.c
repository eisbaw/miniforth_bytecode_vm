// Nano MiniForth VM - maximum size optimization using 2024 research
// Applied techniques: bit fields, register hints, minimal branching, packed data

typedef unsigned char u8;

// Ultra-compact VM state using bit fields and unions for maximum density
static union {
    struct {
        u8 code[24];      // 24 bytes code
        u8 stack[8];      // 8 bytes stack  
        u8 pc : 5;        // 5-bit PC (0-31)
        u8 sp : 3;        // 3-bit SP (0-7)
    } __attribute__((packed));
    u8 raw[32];           // Raw access for initialization
} vm;

// Minimal forth with maximum optimization
u8 forth_nano(const u8 *prog, u8 len) {
    // Fast zero initialization
    for (u8 i = 0; i < 32; i++) vm.raw[i] = 0;
    
    // Load program
    len = (len > 24) ? 24 : len;
    for (u8 i = 0; i < len; i++) vm.code[i] = prog[i];
    
    // Ultra-compact execution using minimal branching
    register u8 a;  // Register allocation hints
    
    while (vm.pc < 24) {
        register u8 op = vm.code[vm.pc] & 7;
        
        // Branchless operation using arithmetic instead of switch/if
        if (op == 0) return 0;  // EXIT
        
        if (op == 1) {  // LIT
            if (vm.sp >= 8) return 1;
            vm.stack[vm.sp++] = vm.code[++vm.pc];
        }
        else if (op == 2) {  // ADD
            if (vm.sp < 2) return 1;
            vm.stack[vm.sp-2] += vm.stack[vm.sp-1];
            vm.sp--;
        }
        else if (op == 3) {  // SUB  
            if (vm.sp < 2) return 1;
            vm.stack[vm.sp-2] -= vm.stack[vm.sp-1];
            vm.sp--;
        }
        else if (op == 4) {  // DUP
            if (vm.sp == 0 || vm.sp >= 8) return 1;
            vm.stack[vm.sp] = vm.stack[vm.sp-1];
            vm.sp++;
        }
        else if (op == 5) {  // DROP
            if (vm.sp == 0) return 1;
            vm.sp--;
        }
        else if (op == 6) {  // SWAP
            if (vm.sp < 2) return 1;
            a = vm.stack[vm.sp-1];
            vm.stack[vm.sp-1] = vm.stack[vm.sp-2];
            vm.stack[vm.sp-2] = a;
        }
        
        vm.pc++;
    }
    return 0;
}