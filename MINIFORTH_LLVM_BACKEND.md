# MiniForth LLVM Backend Architecture

## Overview

The MiniForth LLVM backend is a custom compiler target that generates MiniForth bytecode from LLVM IR. This backend enables compiling C code (via Clang) to MiniForth's stack-based virtual machine instruction set.

## Architecture Components

### 1. Target Machine (`MiniforthTargetMachine`)

**File**: `MiniforthTargetMachine.h/cpp`

The target machine is the main entry point for the backend, responsible for:
- Configuring the target triple and data layout
- Managing compilation passes
- Providing subtarget information

```cpp
class MiniforthTargetMachine : public TargetMachine {
  std::unique_ptr<TargetLoweringObjectFile> TLOF;
  MiniforthSubtarget Subtarget;
  
  // Data layout: little-endian, 32-bit pointers, 32-bit alignment
  static std::string computeDataLayout(const Triple &TT) {
    return "e-m:e-p:32:32-i64:64-n32-S32";
  }
};
```

### 2. Subtarget (`MiniforthSubtarget`)

**File**: `MiniforthSubtarget.h/cpp`

Manages target-specific features and configuration:
- Instruction info, frame lowering, and register info
- Target-specific optimizations
- Feature parsing and CPU selection

```cpp
class MiniforthSubtarget : public MiniforthGenSubtargetInfo {
  MiniforthInstrInfo InstrInfo;
  MiniforthFrameLowering FrameLowering;
  MiniforthTargetLowering TLInfo;
  MiniforthSelectionDAGInfo TSInfo;
};
```

### 3. Register Information (`MiniforthRegisterInfo`)

**File**: `MiniforthRegisterInfo.h/cpp`, `MiniforthRegisterInfo.td`

Defines the register model for the stack-based architecture:

```tablegen
// Stack pointer register
def SP : MiniforthReg<0, "SP">;

// Program counter register (for frame lowering)
def PC : MiniforthReg<1, "PC">;

// Register classes
def SPReg : RegisterClass<"Miniforth", [i32], 32, (add SP)>;
def GPReg : RegisterClass<"Miniforth", [i32], 32, (add PC)>;
```

Key responsibilities:
- Frame pointer management
- Callee-saved register definitions
- Stack frame elimination

### 4. Instruction Information (`MiniforthInstrInfo`)

**File**: `MiniforthInstrInfo.h/cpp`, `MiniforthInstrInfo.td`

Defines the instruction set architecture:

```tablegen
// Base instruction format
class MiniforthInst<dag outs, dag ins, string asmstr, list<dag> pattern>
  : Instruction {
  let Namespace = "Miniforth";
  let OutOperandList = outs;
  let InOperandList = ins;
  let AsmString = asmstr;
  let Pattern = pattern;
}

// Example instructions
def LIT : MiniforthInst<(outs), (ins i32imm:$val), "LIT $val", []>;
def ADD : MiniforthInst<(outs), (ins), "ADD", []>;
def SUB : MiniforthInst<(outs), (ins), "SUB", []>;
```

### 5. Instruction Selection (`MiniforthISelLowering`, `MiniforthISelDAGToDAG`)

**Files**: `MiniforthISelLowering.h/cpp`, `MiniforthISelDAGToDAG.cpp`

Handles lowering of LLVM IR to MiniForth instructions:

#### ISelLowering
- Converts high-level operations to target-specific nodes
- Handles calling conventions
- Manages argument passing and return values

```cpp
class MiniforthTargetLowering : public TargetLowering {
  // Set up register classes
  addRegisterClass(MVT::i32, &Miniforth::GPRegRegClass);
  
  // Configure operation actions
  setOperationAction(ISD::BR_CC, MVT::i32, Expand);
  setOperationAction(ISD::SELECT_CC, MVT::i32, Expand);
};
```

#### DAG-to-DAG Selection
- Pattern matching for instruction selection
- Converts SelectionDAG nodes to machine instructions

### 6. Frame Lowering (`MiniforthFrameLowering`)

**File**: `MiniforthFrameLowering.h/cpp`

Manages stack frame operations:
- Function prologue/epilogue generation
- Stack pointer management
- Local variable allocation

### 7. MC Layer (Machine Code)

**Files**: `MCTargetDesc/MiniforthMCTargetDesc.h/cpp`, `MiniforthInstPrinter.h/cpp`

Handles the machine code layer:
- Assembly instruction printing
- Object file generation
- Target registration

```cpp
// Target registration
static Target TheMiniforthTarget;

Target &getTheMiniforthTarget() {
  return TheMiniforthTarget;
}
```

### 8. Assembly Printer (`MiniforthAsmPrinter`)

**File**: `MiniforthAsmPrinter.cpp`

Converts machine instructions to assembly text:
- Instruction emission
- Symbol handling
- Assembly formatting

## Instruction Set Design

### Stack-Based Architecture

MiniForth uses a stack-based instruction set similar to Forth:

```
LIT 42    ; Push 42 onto stack
LIT 13    ; Push 13 onto stack  
ADD       ; Pop two values, add them, push result
```

### Key Instructions

- **LIT**: Push literal value onto stack
- **ADD/SUB/MUL/DIV**: Arithmetic operations
- **DUP**: Duplicate top stack item
- **DROP**: Remove top stack item
- **SWAP**: Swap two top stack items
- **CALL**: Function call
- **RET**: Return from function

## The Register-to-Stack Translation Challenge

### The Core Problem

Emitting stack machine code from LLVM IR is fundamentally challenging due to the **impedance mismatch** between LLVM's register-based SSA form and MiniForth's stack-based execution model.

LLVM IR uses explicit registers and SSA form:
```llvm
define i32 @example(i32 %a, i32 %b, i32 %c) {
  %1 = add i32 %a, %b      ; Register %1 = %a + %b
  %2 = mul i32 %1, %c      ; Register %2 = %1 * %c
  ret i32 %2               ; Return %2
}
```

MiniForth expects stack-based operations:
```
; Stack starts with: [c, b, a] (a on top)
ADD     ; Pop a,b; push (a+b); stack: [c, result]
MUL     ; Pop c,result; push (c*result); stack: [final_result]
```

### How We Solve This

Our backend uses a **virtual register approach** with sophisticated lowering:

#### 1. **Virtual Stack Registers**

We model the stack as virtual registers in LLVM's register allocator:

```cpp
// MiniforthRegisterInfo.td
def GPReg : RegisterClass<"Miniforth", [i32], 32, (add PC)>;
```

#### 2. **Instruction Pattern Matching**

We use empty patterns `[]` in our instruction definitions because stack operations are implicit:

```tablegen
// MiniforthInstrInfo.td
def ADD : InstBinary<0x05, "ADD", []>;  // No explicit operands
def SUB : InstBinary<0x06, "SUB", []>;  // Stack-based operation
def MUL : InstBinary<0x07, "MUL", []>;  // Implicit stack manipulation
```

#### 3. **Custom Lowering Strategy**

Our `MiniforthISelLowering` class handles the transformation:

```cpp
// Key insight: We let LLVM handle register allocation,
// then emit stack operations during instruction selection
class MiniforthTargetLowering : public TargetLowering {
  // Configure operations to be expanded or custom-lowered
  setOperationAction(ISD::BR_CC, MVT::i32, Expand);
  setOperationAction(ISD::SELECT_CC, MVT::i32, Expand);
};
```

#### 4. **Stack Effect Modeling**

Each instruction has implicit stack effects:
- `LIT 42`: Stack effect `[] -> [42]`
- `ADD`: Stack effect `[a, b] -> [a+b]`
- `DUP`: Stack effect `[a] -> [a, a]`

#### 5. **Calling Convention Translation**

The most complex part is function calls:

```cpp
// MiniforthISelLowering.cpp - LowerCall method
SDValue MiniforthTargetLowering::LowerCall(CallLoweringInfo &CLI,
                                         SmallVectorImpl<SDValue> &InVals) const {
  // Arguments passed via stack in MiniForth
  // We translate register arguments to stack pushes
  CCState CCInfo(CallConv, IsVarArg, DAG.getMachineFunction(), ArgLocs,
                 *DAG.getContext());
  CCInfo.AnalyzeCallOperands(Outs, CC_Miniforth);
  
  // Each argument becomes a stack operation
  for (auto &VA : ArgLocs) {
    // Generate LIT instruction for each argument
    // Arguments are implicitly consumed by CALL
  }
}
```

### Translation Examples

#### Simple Arithmetic
```llvm
; LLVM IR
%result = add i32 %a, %b
```

Becomes:
```
; MiniForth (conceptual - actual emission is more complex)
; %a and %b are already on stack from previous operations
ADD     ; Implicit: pop two values, push sum
```

#### Function Calls
```llvm
; LLVM IR
%result = call i32 @func(i32 %x, i32 %y)
```

Becomes:
```
; MiniForth
; Arguments %x and %y are pushed onto stack
; CALL instruction expects arguments on stack
CALL func   ; Arguments consumed, result pushed
```

### Key Insights

1. **Register Allocation Still Works**: We use LLVM's register allocator on virtual registers, then emit stack operations
2. **Pattern Matching is Minimal**: Most instructions have empty patterns `[]` because stack effects are implicit
3. **Lowering Does the Heavy Lifting**: The `ISelLowering` class handles register-to-stack translation
4. **Stack Discipline**: We maintain stack discipline through careful instruction ordering

### Why This Approach Works

- **Leverage LLVM's Infrastructure**: We get all of LLVM's optimizations for free
- **Gradual Lowering**: Transform high-level operations step by step
- **Maintain Semantics**: Stack operations preserve the original program semantics
- **Efficient Code**: LLVM's optimizations work on the IR before stack translation

This sophisticated approach allows us to compile complex C programs to efficient MiniForth bytecode while maintaining the benefits of LLVM's optimization pipeline.

## Compilation Pipeline

### 1. C Source → LLVM IR
```bash
clang -emit-llvm -S input.c -o input.ll
```

### 2. LLVM IR → MiniForth Assembly
```bash
llc -march=miniforth input.ll -o output.mf
```

### 3. MiniForth Assembly → Bytecode
```bash
miniforth_assembler output.mf -o output.mfb
```

## Build Integration

### CMake Configuration

The backend integrates with LLVM's build system:

```cmake
# CMakeLists.txt
add_llvm_component_group(Miniforth)

add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)

add_llvm_target(MiniforthCodeGen
  MiniforthTargetMachine.cpp
  MiniforthSubtarget.cpp
  MiniforthInstrInfo.cpp
  MiniforthISelLowering.cpp
  MiniforthISelDAGToDAG.cpp
  MiniforthFrameLowering.cpp
  MiniforthRegisterInfo.cpp
  MiniforthAsmPrinter.cpp
)
```

### TableGen Generation

TableGen generates several important files:
- `MiniforthGenInstrInfo.inc`: Instruction definitions
- `MiniforthGenRegisterInfo.inc`: Register information
- `MiniforthGenSubtargetInfo.inc`: Subtarget features
- `MiniforthGenDAGISel.inc`: Instruction selection patterns

## Usage Examples

### Simple Arithmetic
```c
// input.c
int main() {
    int x = 42;
    int y = 13;
    return x + y;
}
```

Generated MiniForth assembly:
```
main:
    LIT 42
    LIT 13
    ADD
    RET
```

### Function Calls
```c
// input.c
int add(int a, int b) {
    return a + b;
}

int main() {
    return add(10, 20);
}
```

Generated MiniForth assembly:
```
add:
    ; Arguments already on stack
    ADD
    RET

main:
    LIT 10
    LIT 20
    CALL add
    RET
```

## Testing and Validation

### Unit Tests
- Register allocation tests
- Instruction selection tests
- Assembly generation tests

### Integration Tests
- Full compilation pipeline tests
- Bytecode execution tests
- Performance benchmarks

### Test Commands
```bash
# Build only MiniForth backend
cd external/llvm-project/build_minimal
ninja LLVMMiniforthCodeGen

# Run backend tests
ninja check-miniforth

# Test with custom input
echo "int main() { return 42; }" | \
  clang -x c -emit-llvm -S - | \
  llc -march=miniforth -o output.mf
```

## Development Workflow

### Adding New Instructions

1. **Define in TableGen**:
```tablegen
// MiniforthInstrInfo.td
def NEWOP : MiniforthInst<(outs), (ins), "NEWOP", []>;
```

2. **Add to instruction info**:
```cpp
// MiniforthInstrInfo.cpp
// Handle instruction-specific logic
```

3. **Update assembler**:
```cpp
// MiniforthInstPrinter.cpp
// Add printing logic for new instruction
```

### Debugging Tips

- Use `llc -debug` for verbose output
- Check generated TableGen files in build directory
- Use `llvm-dis` to examine LLVM IR
- Test with simple C programs first

## Performance Considerations

### Optimization Opportunities
- **Dead code elimination**: Remove unused stack operations
- **Peephole optimization**: Combine adjacent instructions
- **Register allocation**: Optimize stack usage patterns

### Memory Layout
- Stack grows downward
- 32-bit word size
- Little-endian byte order

## Future Enhancements

### Potential Improvements
1. **Advanced optimizations**: Loop optimization, vectorization
2. **Debugging support**: DWARF debug info generation
3. **Profiling integration**: Performance monitoring hooks
4. **Extended instruction set**: SIMD operations, atomic operations

### Compatibility
- Maintains compatibility with existing MiniForth VM
- Supports all MiniForth bytecode instructions
- Integrates with existing toolchain

## Conclusion

The MiniForth LLVM backend provides a robust compilation path from C to MiniForth bytecode. Its modular architecture follows LLVM best practices while accommodating the unique requirements of a stack-based virtual machine. The backend successfully bridges the gap between high-level C code and the MiniForth runtime environment.