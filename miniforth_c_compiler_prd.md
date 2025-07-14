# Miniforth C Compiler & VM - Product Requirements Document

## Document Information
- **Version**: 1.0
- **Date**: 2025-01-14
- **Target Implementation**: Claude Code
- **Project Codename**: TinyForth-C

## Executive Summary

Develop a complete C-to-Forth compilation toolchain consisting of:
1. **Miniforth VM** - Portable C implementation of 504-byte Miniforth architecture
2. **C-to-Forth Compiler** - Translates C subset to Miniforth bytecode
3. **Development Tools** - Assembler, debugger, and testing framework

Target ultra-constrained embedded systems (ARM Cortex-M, RISC-V) with <1KB code size while providing familiar C programming interface.

## Project Objectives

### Primary Goals
- **Code Size**: Complete VM implementation ≤1KB compiled size
- **Portability**: 100% ANSI C99 compatible, no platform dependencies
- **Functionality**: Support essential C subset for embedded control applications
- **Performance**: Acceptable for embedded control loops (>1000 operations/second)
- **Memory**: Operate within 1-2KB RAM constraints

### Success Criteria
- [ ] VM passes all Miniforth compatibility tests
- [ ] C compiler handles realistic embedded C programs
- [ ] Complete toolchain builds on Linux, macOS, Windows
- [ ] Generated code fits in 1KB flash memory
- [ ] Execution performance meets embedded real-time requirements

## Architecture Overview

```
┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│   C Source  │───▶│ C-to-Forth  │───▶│  Miniforth  │
│   (.c)      │    │  Compiler   │    │  Bytecode   │
└─────────────┘    └─────────────┘    └─────────────┘
                            │                 │
                            ▼                 ▼
                   ┌─────────────┐    ┌─────────────┐
                   │  Symbol     │    │  Miniforth  │
                   │  Table      │    │  Virtual    │
                   │  (.sym)     │    │  Machine    │
                   └─────────────┘    └─────────────┘
```

## Component 1: Miniforth Virtual Machine

### 1.1 Core Requirements

#### VM Architecture
- **Type**: Stack-based virtual machine
- **Word Size**: 16-bit or 32-bit (configurable)
- **Memory Model**: Harvard architecture (separate code/data)
- **Stack Depth**: Minimum 32 elements data stack, 16 elements return stack

#### Instruction Set (Miniforth Core)
| Opcode | Mnemonic | Stack Effect | Description |
|--------|----------|--------------|-------------|
| 0x00   | `EXIT`   | `( -- )`     | Return from word |
| 0x01   | `LIT`    | `( -- n )`   | Push literal value |
| 0x02   | `DUP`    | `( n -- n n )` | Duplicate top of stack |
| 0x03   | `DROP`   | `( n -- )`   | Remove top of stack |
| 0x04   | `SWAP`   | `( a b -- b a )` | Exchange top two items |
| 0x05   | `+`      | `( a b -- c )` | Addition |
| 0x06   | `-`      | `( a b -- c )` | Subtraction |
| 0x07   | `*`      | `( a b -- c )` | Multiplication |
| 0x08   | `/`      | `( a b -- c )` | Division |
| 0x09   | `=`      | `( a b -- f )` | Equality test |
| 0x0A   | `<`      | `( a b -- f )` | Less than test |
| 0x0B   | `@`      | `( addr -- n )` | Fetch from memory |
| 0x0C   | `!`      | `( n addr -- )` | Store to memory |
| 0x0D   | `JMP`    | `( -- )`     | Unconditional jump |
| 0x0E   | `JZ`     | `( f -- )`   | Jump if zero |
| 0x0F   | `CALL`   | `( -- )`     | Call subroutine |

#### Memory Management
```c
typedef struct {
    uint16_t *code_memory;     // Program storage
    uint16_t *data_memory;     // Variable storage
    uint16_t *data_stack;      // Computation stack
    uint16_t *return_stack;    // Call stack
    
    uint16_t pc;               // Program counter
    uint16_t sp;               // Stack pointer
    uint16_t rp;               // Return stack pointer
    
    uint16_t code_size;        // Code memory size
    uint16_t data_size;        // Data memory size
    uint16_t stack_size;       // Stack size
} miniforth_vm_t;
```

### 1.2 Implementation Specifications

#### VM Core Functions
```c
// VM lifecycle
miniforth_vm_t* vm_create(uint16_t code_size, uint16_t data_size, uint16_t stack_size);
void vm_destroy(miniforth_vm_t *vm);
int vm_load_program(miniforth_vm_t *vm, const uint16_t *program, uint16_t size);

// Execution
int vm_step(miniforth_vm_t *vm);           // Execute one instruction
int vm_run(miniforth_vm_t *vm);            // Run until EXIT or error
int vm_run_steps(miniforth_vm_t *vm, int max_steps);  // Limited execution

// Memory access (raw access for embedded systems)
uint16_t vm_read_memory(miniforth_vm_t *vm, uint16_t addr);
void vm_write_memory(miniforth_vm_t *vm, uint16_t addr, uint16_t value);
uint8_t vm_read_byte(miniforth_vm_t *vm, uint16_t addr);
void vm_write_byte(miniforth_vm_t *vm, uint16_t addr, uint8_t value);

// Stack operations
void vm_push(miniforth_vm_t *vm, uint16_t value);
uint16_t vm_pop(miniforth_vm_t *vm);
uint16_t vm_peek(miniforth_vm_t *vm, int depth);

// Debugging support
void vm_dump_state(miniforth_vm_t *vm);
void vm_dump_stack(miniforth_vm_t *vm);
void vm_dump_memory(miniforth_vm_t *vm, uint16_t start, uint16_t count);
```

#### Error Handling
```c
typedef enum {
    VM_OK = 0,
    VM_ERROR_STACK_OVERFLOW,
    VM_ERROR_STACK_UNDERFLOW,
    VM_ERROR_INVALID_OPCODE,
    VM_ERROR_MEMORY_ACCESS,
    VM_ERROR_DIVISION_BY_ZERO,
    VM_ERROR_PROGRAM_COUNTER,
    VM_ERROR_RETURN_STACK_OVERFLOW,
    VM_ERROR_RETURN_STACK_UNDERFLOW
} vm_error_t;

const char* vm_error_string(vm_error_t error);
```

### 1.3 Configuration Options
```c
// Compile-time configuration
#define MINIFORTH_WORD_SIZE 16        // 16 or 32 bit words
#define MINIFORTH_DEFAULT_CODE_SIZE 512
#define MINIFORTH_DEFAULT_DATA_SIZE 256
#define MINIFORTH_DEFAULT_STACK_SIZE 32

// Feature flags
#define MINIFORTH_ENABLE_DEBUGGING 1   // Include debug functions
#define MINIFORTH_ENABLE_BOUNDS_CHECK 1 // Runtime bounds checking
#define MINIFORTH_ENABLE_FLOAT 0       // Floating point support (future)
```

## Component 2: C-to-Forth Compiler

### 2.1 Supported C Subset

#### Data Types
- `int` - Maps to VM word size
- `char` - 8-bit values, stored as words
- `int array[N]` - Fixed-size arrays
- `char array[N]` - Character arrays

#### Operators
- **Arithmetic**: `+`, `-`, `*`, `/`, `%`
- **Comparison**: `==`, `!=`, `<`, `<=`, `>`, `>=`
- **Logical**: `&&`, `||`, `!`
- **Assignment**: `=`, `+=`, `-=`, `*=`, `/=`
- **Increment**: `++`, `--` (prefix and postfix)

#### Control Structures
- `if (condition) statement`
- `if (condition) statement else statement`
- `while (condition) statement`
- `for (init; condition; increment) statement`
- `do statement while (condition)`
- `break` and `continue`
- `return [expression]`

#### Functions
- Function definition: `int func(int param1, int param2) { ... }`
- Function calls with parameters
- Local variables (stack-allocated)
- Global variables
- Static variables (global scope)

#### Arrays and Pointers
- Fixed-size arrays: `int arr[10]`
- Array indexing: `arr[index]`
- Pointer arithmetic: `ptr + offset`
- Address-of operator: `&variable`
- Dereference operator: `*pointer`

### 2.2 Compiler Architecture

#### Lexical Analysis
```c
typedef enum {
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_PLUS, TOKEN_MINUS, TOKEN_MULTIPLY, TOKEN_DIVIDE,
    TOKEN_EQUAL, TOKEN_NOT_EQUAL, TOKEN_LESS, TOKEN_GREATER,
    TOKEN_ASSIGN,
    TOKEN_SEMICOLON, TOKEN_COMMA, TOKEN_DOT,
    TOKEN_LPAREN, TOKEN_RPAREN,
    TOKEN_LBRACE, TOKEN_RBRACE,
    TOKEN_LBRACKET, TOKEN_RBRACKET,
    TOKEN_IF, TOKEN_ELSE, TOKEN_WHILE, TOKEN_FOR, TOKEN_DO,
    TOKEN_INT, TOKEN_CHAR, TOKEN_RETURN,
    TOKEN_EOF
} token_type_t;

typedef struct {
    token_type_t type;
    char *text;
    int line;
    int column;
} token_t;
```

#### Abstract Syntax Tree
```c
typedef enum {
    AST_PROGRAM,
    AST_FUNCTION,
    AST_VARIABLE_DECL,
    AST_ARRAY_DECL,
    AST_BINARY_OP,
    AST_UNARY_OP,
    AST_ASSIGNMENT,
    AST_FUNCTION_CALL,
    AST_IF_STMT,
    AST_WHILE_STMT,
    AST_FOR_STMT,
    AST_RETURN_STMT,
    AST_BLOCK,
    AST_IDENTIFIER,
    AST_NUMBER,
    AST_ARRAY_ACCESS
} ast_node_type_t;

typedef struct ast_node {
    ast_node_type_t type;
    union {
        struct { char *name; struct ast_node *params; struct ast_node *body; } function;
        struct { char *name; int size; } var_decl;
        struct { int op; struct ast_node *left; struct ast_node *right; } binary;
        struct { int op; struct ast_node *operand; } unary;
        struct { struct ast_node *target; struct ast_node *value; } assignment;
        struct { char *name; struct ast_node *args; } call;
        struct { struct ast_node *condition; struct ast_node *then_stmt; struct ast_node *else_stmt; } if_stmt;
        struct { struct ast_node *condition; struct ast_node *body; } while_stmt;
        struct { struct ast_node *init; struct ast_node *condition; struct ast_node *increment; struct ast_node *body; } for_stmt;
        struct { struct ast_node *expression; } return_stmt;
        struct { char *name; } identifier;
        struct { int value; } number;
        struct { struct ast_node *array; struct ast_node *index; } array_access;
    } data;
    struct ast_node *next;  // For lists
} ast_node_t;
```

#### Symbol Table
```c
typedef enum {
    SYMBOL_VARIABLE,
    SYMBOL_ARRAY,
    SYMBOL_FUNCTION,
    SYMBOL_PARAMETER
} symbol_type_t;

typedef struct symbol {
    char *name;
    symbol_type_t type;
    int size;           // For arrays
    int offset;         // Memory offset
    int scope_level;    // For local variables
    struct symbol *next;
} symbol_t;

typedef struct {
    symbol_t *symbols;
    int current_offset;
    int scope_level;
} symbol_table_t;
```

### 2.3 Code Generation

#### Compiler Interface
```c
typedef struct {
    FILE *input;
    FILE *output;
    symbol_table_t *symbols;
    int error_count;
    int warning_count;
} compiler_context_t;

// Main compiler functions
int compile_file(const char *input_file, const char *output_file);
int compile_string(const char *source, char **output);
ast_node_t* parse_program(compiler_context_t *ctx);
void generate_forth_code(compiler_context_t *ctx, ast_node_t *ast);
```

#### Code Generation Patterns
```c
// Expression compilation to postfix
void emit_expression(ast_node_t *expr) {
    switch (expr->type) {
        case AST_NUMBER:
            emit_literal(expr->data.number.value);
            break;
        case AST_IDENTIFIER:
            emit_variable_access(expr->data.identifier.name);
            break;
        case AST_BINARY_OP:
            emit_expression(expr->data.binary.left);
            emit_expression(expr->data.binary.right);
            emit_operator(expr->data.binary.op);
            break;
    }
}
```

## Component 3: Development Tools

### 3.1 Assembler
```c
// Direct Miniforth assembly
int assemble_file(const char *input_file, const char *output_file);
int assemble_string(const char *source, uint16_t **bytecode, int *size);
```

### 3.2 Disassembler
```c
// Convert bytecode back to readable assembly
void disassemble(const uint16_t *bytecode, int size, FILE *output);
char* disassemble_instruction(uint16_t instruction);
```

### 3.3 Debugger
```c
typedef struct {
    miniforth_vm_t *vm;
    uint16_t *breakpoints;
    int breakpoint_count;
    int single_step;
} debugger_t;

debugger_t* debugger_create(miniforth_vm_t *vm);
void debugger_add_breakpoint(debugger_t *dbg, uint16_t address);
void debugger_run(debugger_t *dbg);
void debugger_step(debugger_t *dbg);
void debugger_continue(debugger_t *dbg);
```

## Test Cases and Validation

### 4.1 VM Core Tests

#### Test 1: Basic Stack Operations
```c
void test_stack_operations() {
    miniforth_vm_t *vm = vm_create(64, 64, 32);
    
    // Test DUP
    uint16_t program1[] = {LIT, 42, DUP, EXIT};
    vm_load_program(vm, program1, 4);
    vm_run(vm);
    assert(vm_pop(vm) == 42);
    assert(vm_pop(vm) == 42);
    
    // Test SWAP
    uint16_t program2[] = {LIT, 1, LIT, 2, SWAP, EXIT};
    vm_load_program(vm, program2, 6);
    vm_run(vm);
    assert(vm_pop(vm) == 1);
    assert(vm_pop(vm) == 2);
    
    vm_destroy(vm);
}
```

#### Test 2: Arithmetic Operations
```c
void test_arithmetic() {
    miniforth_vm_t *vm = vm_create(64, 64, 32);
    
    // Test addition: 3 + 4 = 7
    uint16_t program[] = {LIT, 3, LIT, 4, ADD, EXIT};
    vm_load_program(vm, program, 6);
    vm_run(vm);
    assert(vm_pop(vm) == 7);
    
    // Test multiplication: 6 * 7 = 42
    uint16_t program2[] = {LIT, 6, LIT, 7, MUL, EXIT};
    vm_load_program(vm, program2, 6);
    vm_run(vm);
    assert(vm_pop(vm) == 42);
    
    vm_destroy(vm);
}
```

#### Test 3: Memory Access
```c
void test_memory_access() {
    miniforth_vm_t *vm = vm_create(64, 64, 32);
    
    // Store 100 at address 10, then fetch it
    uint16_t program[] = {
        LIT, 100,    // value
        LIT, 10,     // address
        STORE,       // store value at address
        LIT, 10,     // address
        FETCH,       // fetch from address
        EXIT
    };
    vm_load_program(vm, program, 12);
    vm_run(vm);
    assert(vm_pop(vm) == 100);
    
    vm_destroy(vm);
}
```

#### Test 4: Control Flow
```c
void test_control_flow() {
    miniforth_vm_t *vm = vm_create(64, 64, 32);
    
    // Conditional jump test
    uint16_t program[] = {
        LIT, 1,      // condition (true)
        JZ, 8,       // jump to address 8 if zero
        LIT, 42,     // this should execute
        JMP, 10,     // jump to end
        LIT, 99,     // this should be skipped (address 8)
        EXIT         // address 10
    };
    vm_load_program(vm, program, 12);
    vm_run(vm);
    assert(vm_pop(vm) == 42);  // Should be 42, not 99
    
    vm_destroy(vm);
}
```

### 4.2 Compiler Tests

#### Test 1: Simple Function
```c
const char *test_source_1 = 
    "int add(int a, int b) {"
    "    return a + b;"
    "}";

void test_simple_function() {
    char *output;
    assert(compile_string(test_source_1, &output) == 0);
    
    // Expected Forth output:
    // : add + ;
    assert(strstr(output, ": add") != NULL);
    assert(strstr(output, "+") != NULL);
    assert(strstr(output, ";") != NULL);
    
    free(output);
}
```

#### Test 2: Variables and Assignment
```c
const char *test_source_2 = 
    "int main() {"
    "    int x = 5;"
    "    int y = 3;"
    "    int result = x + y;"
    "    return result;"
    "}";

void test_variables() {
    char *output;
    assert(compile_string(test_source_2, &output) == 0);
    
    // Should contain variable declarations and assignments
    assert(strstr(output, "VARIABLE x") != NULL);
    assert(strstr(output, "VARIABLE y") != NULL);
    assert(strstr(output, "VARIABLE result") != NULL);
    
    free(output);
}
```

#### Test 3: Arrays
```c
const char *test_source_3 = 
    "int main() {"
    "    int arr[5];"
    "    arr[2] = 42;"
    "    return arr[2];"
    "}";

void test_arrays() {
    char *output;
    assert(compile_string(test_source_3, &output) == 0);
    
    // Should contain array declaration and indexing
    assert(strstr(output, "5 CELLS ALLOT") != NULL);
    
    free(output);
}
```

#### Test 4: Control Structures
```c
const char *test_source_4 = 
    "int main() {"
    "    int x = 5;"
    "    if (x > 3) {"
    "        return 1;"
    "    } else {"
    "        return 0;"
    "    }"
    "}";

void test_control_structures() {
    char *output;
    assert(compile_string(test_source_4, &output) == 0);
    
    // Should contain conditional logic
    assert(strstr(output, "JZ") != NULL);  // Jump if zero for if condition
    
    free(output);
}
```

### 4.3 Integration Tests

#### Test 1: Complete Program Compilation and Execution
```c
void test_integration_fibonacci() {
    const char *fib_source = 
        "int fibonacci(int n) {"
        "    if (n <= 1) {"
        "        return n;"
        "    }"
        "    return fibonacci(n-1) + fibonacci(n-2);"
        "}"
        ""
        "int main() {"
        "    return fibonacci(6);"  // Should return 8
        "}";
    
    // Compile C to Forth
    char *forth_code;
    assert(compile_string(fib_source, &forth_code) == 0);
    
    // Assemble Forth to bytecode
    uint16_t *bytecode;
    int size;
    assert(assemble_string(forth_code, &bytecode, &size) == 0);
    
    // Execute on VM
    miniforth_vm_t *vm = vm_create(512, 256, 64);
    vm_load_program(vm, bytecode, size);
    vm_run(vm);
    
    assert(vm_pop(vm) == 8);  // fibonacci(6) = 8
    
    free(forth_code);
    free(bytecode);
    vm_destroy(vm);
}
```

#### Test 2: Embedded Control Example
```c
void test_embedded_control() {
    const char *control_source = 
        "int sensor_value;"
        "int motor_speed;"
        ""
        "void control_loop() {"
        "    while (1) {"
        "        if (sensor_value > 100) {"
        "            motor_speed = 50;"
        "        } else {"
        "            motor_speed = 0;"
        "        }"
        "    }"
        "}";
    
    char *forth_code;
    assert(compile_string(control_source, &forth_code) == 0);
    
    // Verify it contains proper control structures
    assert(strstr(forth_code, "BEGIN") != NULL);  // Infinite loop
    assert(strstr(forth_code, "AGAIN") != NULL);
    
    free(forth_code);
}
```

## Performance Requirements

### 5.1 Code Size Targets
- **VM Core**: ≤800 bytes compiled (ARM Thumb, -Os)
- **Runtime Libraries**: ≤200 bytes
- **Total System**: ≤1024 bytes

### 5.2 Execution Performance
- **Instruction Throughput**: ≥1000 VM instructions/second on 16MHz ARM Cortex-M0
- **Function Call Overhead**: ≤10 cycles per call
- **Memory Access**: ≤5 cycles per word access

### 5.3 Memory Usage
- **Code Memory**: 512-2048 bytes
- **Data Memory**: 256-1024 bytes
- **Stack Memory**: 64-128 bytes total (data + return stacks)

## Implementation Phases

### Phase 1: Core VM (Week 1-2)
- [ ] Implement basic VM structure
- [ ] Implement all 16 core instructions
- [ ] Add memory management
- [ ] Create test suite for VM
- [ ] Verify stack operations work correctly

### Phase 2: Basic Compiler (Week 3-4)
- [ ] Implement lexer for C subset
- [ ] Implement parser for basic expressions
- [ ] Implement code generator for arithmetic
- [ ] Support variables and assignment
- [ ] Test with simple programs

### Phase 3: Advanced Compiler (Week 5-6)
- [ ] Add function support
- [ ] Add control structures (if/while/for)
- [ ] Add array support
- [ ] Implement proper symbol table
- [ ] Add error reporting

### Phase 4: Integration & Optimization (Week 7-8)
- [ ] Complete integration testing
- [ ] Optimize generated code size
- [ ] Add debugging tools
- [ ] Performance testing and optimization
- [ ] Documentation and examples

## Development Environment

### Build System
The project uses **CMake** as the primary build system for cross-platform compatibility and modern C/C++ development practices.

### Development Environment Setup
The project uses **nix-shell** to provide a consistent development environment with all required dependencies:

```bash
nix-shell  # Enter development environment with all dependencies
```

### Development Commands
The project uses **just** (a command runner) to provide clear, documented commands for common development tasks:

```bash
just --list          # Show all available commands
just build           # Build the project
just test            # Run all tests
just clean           # Clean build artifacts
just vm-test         # Run VM-specific tests
just compiler-test   # Run compiler-specific tests
just integration     # Run integration tests
just format          # Format code with clang-format
just lint            # Run static analysis
just coverage        # Generate code coverage report
just docs            # Generate documentation
just examples        # Build example programs
just benchmark       # Run performance benchmarks
```

### Required Dependencies
All dependencies are managed through `shell.nix`:
- **CMake** (≥3.16) - Build system
- **GCC** or **Clang** - C compiler with C99 support
- **Just** - Command runner
- **Valgrind** - Memory leak detection
- **Clang-format** - Code formatting
- **Cppcheck** - Static analysis
- **LCOV** - Code coverage
- **Doxygen** - Documentation generation
- **pkg-config** - Library configuration

## File Structure

```
miniforth-c/
├── shell.nix                    # Nix development environment
├── justfile                     # Development commands
├── CMakeLists.txt              # Main CMake configuration
├── src/
│   ├── CMakeLists.txt          # Source CMake configuration
│   ├── vm/
│   │   ├── CMakeLists.txt      # VM CMake configuration
│   │   ├── miniforth.h          # VM public interface
│   │   ├── miniforth.c          # VM implementation
│   │   └── instructions.c       # Instruction implementations
│   ├── compiler/
│   │   ├── CMakeLists.txt      # Compiler CMake configuration
│   │   ├── lexer.h              # Lexical analyzer
│   │   ├── lexer.c
│   │   ├── parser.h             # Syntax analyzer
│   │   ├── parser.c
│   │   ├── codegen.h            # Code generation
│   │   ├── codegen.c
│   │   └── symbols.c            # Symbol table management
│   ├── tools/
│   │   ├── CMakeLists.txt      # Tools CMake configuration
│   │   ├── assembler.c          # Forth assembler
│   │   ├── disassembler.c       # Bytecode disassembler
│   │   └── debugger.c           # Interactive debugger
│   └── main.c                   # Command-line interface
├── tests/
│   ├── CMakeLists.txt          # Test CMake configuration
│   ├── vm_tests.c               # VM unit tests
│   ├── compiler_tests.c         # Compiler unit tests
│   ├── integration_tests.c      # End-to-end tests
│   └── examples/                # Example C programs
│       ├── fibonacci.c
│       ├── control_loop.c
│       └── array_demo.c
├── docs/
│   ├── vm_spec.md               # VM specification
│   ├── c_subset.md              # Supported C features
│   └── examples.md              # Usage examples
├── build/                       # CMake build directory (generated)
└── README.md
```

## Success Metrics

### Functional Requirements
- [ ] VM executes all test programs correctly
- [ ] Compiler handles 95% of typical embedded C constructs
- [ ] Generated code fits in 1KB for moderate programs
- [ ] No memory leaks or crashes in normal operation
- [ ] Cross-platform compatibility (Linux, macOS, Windows)

### Performance Requirements
- [ ] VM achieves target instruction throughput
- [ ] Compiler generates reasonably efficient code
- [ ] Build time under 10 seconds for complete toolchain
- [ ] Memory usage stays within embedded constraints

### Quality Requirements
- [ ] 90%+ code coverage in tests
- [ ] Clean compilation with -Wall -Werror
- [ ] Valgrind clean (no memory errors)
- [ ] Static analysis clean (cppcheck, clang-tidy)

## Risk Mitigation

### Technical Risks
- **Stack overflow**: Implement bounds checking and configurable stack sizes
- **Memory corruption**: Use safe memory access patterns and extensive testing
- **Performance issues**: Profile and optimize critical paths
- **Portability problems**: Use only standard C99 features

### Schedule Risks
- **Scope creep**: Strictly maintain C subset limitations
- **Complex debugging**: Implement debugging tools early
- **Integration issues**: Test components together frequently

## Deliverables

### Code Deliverables
1. Complete VM implementation in portable C
2. C-to-Forth compiler with full C subset support
3. Development tools (assembler, debugger)
4. Comprehensive test suite
5. Example programs and documentation

### Documentation Deliverables
1. API documentation for all public interfaces
2. User guide for C subset programming
3. VM architecture specification
4. Performance analysis and optimization guide
5. Porting guide for new embedded platforms

---

**Note for Claude Code**: This PRD provides complete specifications for implementing a production-quality C-to-Forth compiler and Miniforth VM. Focus on correctness and portability first, then optimize for size and performance. The test cases are essential - implement them alongside the code to ensure reliability.