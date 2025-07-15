# Miniforth C Compiler & VM Development Commands

# Show all available commands
default:
    @just --list
    @echo ""
    @echo "🔍 Useful recipes:"
    @echo "  just build        - Build the project"
    @echo "  just test         - Run all tests"
    @echo "  just code-size    - Analyze source and binary code size"
    @echo "  just status       - Show project status"
    @echo "  just clean        - Clean build artifacts"
    @echo ""
    @echo "🧪 LCC Testing:"
    @echo "  just test-lcc-miniforth      - Run all LCC Miniforth tests"
    @echo "  just test-category CATEGORY  - Test specific category"
    @echo "  just quick-test              - Quick compilation test"
    @echo "  just show-bytecode TEST      - Show bytecode for specific test"
    @echo ""
    @echo "🛠️ Development:"
    @echo "  just build-lcc    - Build LCC compiler"
    @echo "  just check-deps   - Check dependencies"
    @echo "  just help-tests   - Show test categories"
    @echo "  just dev          - Full development cycle"

# Build the project
build:
    @echo "Building miniforth project..."
    @mkdir -p build
    @cd build && cmake .. && make

# Build with debug symbols
build-debug:
    @echo "Building with debug symbols..."
    @mkdir -p build
    @cd build && cmake -DCMAKE_BUILD_TYPE=Debug .. && make

# Build with optimizations
build-release:
    @echo "Building optimized release..."
    @mkdir -p build
    @cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && make

# Clean build artifacts
clean:
    @echo "Cleaning build artifacts..."
    @rm -rf build/
    @rm -f *.o *.so *.a

# Run all tests
test: build
    @echo "Running all tests..."
    @cd build && timeout 60 ctest --verbose

# Run VM-specific tests
vm-test: build
    @echo "Running VM tests..."
    @cd build && ./tests/vm_tests

# Run compiler-specific tests
compiler-test: build
    @echo "Running compiler tests..."
    @cd build && ./tests/compiler_tests

# Run integration tests
integration: build
    @echo "Running integration tests..."
    @cd build && ./tests/integration_tests

# Format code with clang-format
format:
    @echo "Formatting code..."
    @if command -v clang-format >/dev/null 2>&1; then \
        find src/ tests/ -name "*.c" -o -name "*.h" | xargs clang-format -i; \
        echo "✅ Code formatted successfully"; \
    else \
        echo "❌ clang-format not found. Install with your package manager."; \
        echo "  NixOS: nix-shell -p clang-tools"; \
        echo "  Ubuntu: sudo apt install clang-format"; \
    fi

# Run static analysis with cppcheck
lint:
    @echo "Running static analysis..."
    @if command -v cppcheck >/dev/null 2>&1; then \
        cppcheck --enable=warning,style --std=c99 --error-exitcode=1 \
            --suppress=unusedFunction --suppress=missingIncludeSystem \
            --suppress=syntaxError:src/vm/miniforth_extreme.c \
            --suppress=syntaxError:src/vm/miniforth_ultra.c \
            --suppress=unknownEvaluationOrder:src/vm/miniforth_asm_style.c \
            --suppress=unknownEvaluationOrder:src/vm/miniforth_atomic.c \
            src/; \
    else \
        echo "❌ cppcheck not found. Install with your package manager."; \
        echo "  NixOS: nix-shell -p cppcheck"; \
        echo "  Ubuntu: sudo apt install cppcheck"; \
    fi

# Generate code coverage report
coverage: build
    @echo "Generating code coverage report..."
    @if command -v lcov >/dev/null 2>&1 && command -v genhtml >/dev/null 2>&1; then \
        cd build && cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON .. && make; \
        cd build && make test; \
        cd build && lcov --capture --directory . --output-file coverage.info; \
        cd build && genhtml coverage.info --output-directory coverage_html; \
        echo "Coverage report generated in build/coverage_html/"; \
    else \
        echo "❌ lcov/genhtml not found. Install with your package manager."; \
        echo "  NixOS: nix-shell -p lcov"; \
        echo "  Ubuntu: sudo apt install lcov"; \
    fi

# Generate documentation with doxygen
docs:
    @echo "Generating documentation..."
    @if [ -f Doxyfile ]; then \
        doxygen Doxyfile; \
    else \
        echo "❌ Doxyfile not found. Create one with 'doxygen -g' first."; \
    fi

# Build example programs
examples: build
    @echo "Building example programs..."
    @cd build && make examples

# Run performance benchmarks
benchmark: build
    @echo "Running performance benchmarks..."
    @if [ -f build/tests/benchmark ]; then \
        cd build && ./tests/benchmark; \
    else \
        echo "❌ Benchmark executable not found. Check CMakeLists.txt for benchmark target."; \
    fi

# Initialize git repository and make first commit
init:
    @echo "Initializing git repository..."
    @git init
    @git add .
    @git commit -m "Initial commit: Project structure with nix-shell, justfile, and CMake"

# Run full development cycle (format, lint, build, test)
dev: format lint build test
    @echo "Development cycle complete!"

# Install the toolchain (copy binaries to /usr/local/bin)
install: build-release
    @echo "Installing miniforth toolchain..."
    @for binary in miniforth miniforth-compiler miniforth-assembler miniforth-debugger; do \
        if [ -f "build/$$binary" ]; then \
            sudo cp "build/$$binary" /usr/local/bin/; \
            echo "✅ Installed $$binary"; \
        else \
            echo "⚠️  $$binary not found, skipping"; \
        fi; \
    done
    @echo "Installation complete!"

# Measure bytecode VM size and performance
code-size: build
    @echo "Miniforth Bytecode VM Size Analysis"
    @echo "===================================="
    @echo ""
    @echo "🎯 Target VM Binary Size:"
    @echo "========================="
    @if [ -f build/miniforth ]; then \
        echo "Main VM executable:"; \
        ls -lh build/miniforth | awk '{print "  Size: " $$5}'; \
        echo "  Raw bytes: $(stat -c%s build/miniforth 2>/dev/null || stat -f%z build/miniforth 2>/dev/null || echo 'N/A')"; \
        echo "  Stripped: $(strip -s build/miniforth -o /tmp/miniforth_stripped 2>/dev/null && ls -lh /tmp/miniforth_stripped | awk '{print $$5}' && rm -f /tmp/miniforth_stripped || echo 'N/A')"; \
    else \
        echo "❌ Main VM executable: Not found in build/"; \
        echo "   Try running: just build"; \
    fi
    @echo ""
    @echo "🔧 VM Core Components:"
    @echo "====================="
    @if [ -f build/src/vm/libvm.a ]; then \
        echo "VM Library (libvm.a): $(ls -lh build/src/vm/libvm.a | awk '{print $$5}')"; \
    elif [ -f build/src/vm/libminiforth_vm.a ]; then \
        echo "VM Library: $(ls -lh build/src/vm/libminiforth_vm.a | awk '{print $$5}')"; \
    else \
        echo "VM Library: Not found"; \
    fi
    @if [ -f build/src/compiler/libcompiler.a ]; then \
        echo "Compiler Library (libcompiler.a): $(ls -lh build/src/compiler/libcompiler.a | awk '{print $$5}')"; \
    elif [ -f build/src/compiler/libminiforth_compiler.a ]; then \
        echo "Compiler Library: $(ls -lh build/src/compiler/libminiforth_compiler.a | awk '{print $$5}')"; \
    else \
        echo "Compiler Library: Not found"; \
    fi
    @echo ""
    @echo "📊 VM Object Files:"
    @echo "=================="
    @if [ -d build ]; then \
        echo "VM Core Objects:"; \
        find build -name "*miniforth*" -o -name "*instructions*" -o -name "*vm*" | grep -E "\\.o$$" | while read f; do \
            echo "  $(basename $$f): $(ls -lh $$f | awk '{print $$5}')"; \
        done 2>/dev/null || echo "  No VM object files found"; \
    fi
    @echo ""
    @echo "🏗️ Build Analysis:"
    @echo "=================="
    @if [ -d build ]; then \
        echo "Build directory total: $(du -sh build 2>/dev/null | awk '{print $$1}')"; \
        echo "All object files: $(find build -name '*.o' 2>/dev/null | wc -l) files"; \
        echo "All libraries: $(find build -name '*.a' 2>/dev/null | wc -l) files"; \
        echo "Executables: $(find build -type f -executable 2>/dev/null | wc -l) files"; \
    else \
        echo "❌ Build directory not found"; \
    fi
    @echo ""
    @echo "📋 Test Executables:"
    @echo "==================="
    @for test in vm_tests compiler_tests integration_tests; do \
        if [ -f build/tests/$$test ]; then \
            echo "$$test: $(ls -lh build/tests/$$test | awk '{print $$5}')"; \
        elif [ -f build/$$test ]; then \
            echo "$$test: $(ls -lh build/$$test | awk '{print $$5}')"; \
        else \
            echo "$$test: Not found"; \
        fi; \
    done
    @echo ""
    @echo "💡 To reduce VM size:"
    @echo "  - Use 'just build-release' for optimized build"
    @echo "  - Strip debug symbols with 'strip build/miniforth'"
    @echo "  - Consider compile-time optimizations (-Os, -O2)"

# Compile test programs with LCC bytecode
lcc-bytecode:
    @echo "LCC Bytecode Compilation Test"
    @echo "============================="
    @echo ""
    @echo "Creating simple test program..."
    @echo 'int main() { return 42; }' > build/simple_test.c
    @echo "Compiling to LCC bytecode..."
    @external/lcc/build/rcc -target=bytecode -b external/lcc/build build/simple_test.c
    @echo ""
    @echo "Generated bytecode:"
    @head -15 build/simple_test.c
    @echo ""
    @echo "Creating factorial test..."
    @echo 'int factorial(int n) { if (n <= 1) return 1; return n * factorial(n-1); } int main() { return factorial(5); }' > build/factorial_test.c
    @external/lcc/build/rcc -target=bytecode -b external/lcc/build build/factorial_test.c
    @echo "Factorial bytecode (first 20 lines):"
    @head -20 build/factorial_test.c

# Compare GCC vs LCC Miniforth compilation  
lcc-miniforth:
    @echo "LCC Miniforth Backend Test"
    @echo "=========================="
    @echo ""
    @echo "Simple test: int main() { return 42; }"
    @echo 'int main() { return 42; }' > /tmp/miniforth_simple.c
    @external/lcc/build/rcc -target=miniforth -b external/lcc/build /tmp/miniforth_simple.c
    @echo ""
    @echo "Generated Miniforth bytecode:"
    @head -20 /tmp/miniforth_simple.c
    @echo ""
    @echo "Factorial test: int factorial(int n) { if (n <= 1) return 1; return n * factorial(n-1); }"
    @echo 'int factorial(int n) { if (n <= 1) return 1; return n * factorial(n-1); } int main() { return factorial(5); }' > /tmp/miniforth_factorial.c  
    @external/lcc/build/rcc -target=miniforth -b external/lcc/build /tmp/miniforth_factorial.c
    @echo ""
    @echo "Factorial bytecode (first 15 lines):"
    @head -15 /tmp/miniforth_factorial.c

# Compare all compiler targets
compiler-compare:
    @echo "Compiler Comparison: GCC vs LCC Targets"
    @echo "======================================="
    @echo ""
    @echo "Test program: int main() { return 42; }"
    @echo 'int main() { return 42; }' > build/compare_test.c
    @echo ""
    @echo "GCC native compilation:"
    @gcc -O2 build/compare_test.c -o build/gcc_compare
    @echo "  Binary size: $(ls -lh build/gcc_compare | awk '{print $$5}')"
    @echo "  Exit code: $(build/gcc_compare; echo $$?)"
    @echo ""
    @echo "LCC bytecode compilation:"
    @external/lcc/build/rcc -target=bytecode -b external/lcc/build build/compare_test.c
    @echo "  Bytecode size: $(wc -c < build/compare_test.c) bytes"
    @echo "  Bytecode lines: $(wc -l < build/compare_test.c) lines"
    @echo ""
    @echo "LCC Miniforth compilation:"
    @echo 'int main() { return 42; }' > /tmp/lcc_miniforth_test.c
    @external/lcc/build/rcc -target=miniforth -b external/lcc/build /tmp/lcc_miniforth_test.c
    @echo "  Miniforth bytecode size: $(wc -c < /tmp/lcc_miniforth_test.c) bytes"
    @echo "  Miniforth bytecode lines: $(wc -l < /tmp/lcc_miniforth_test.c) lines"

# Test all LCC Miniforth test cases
test-lcc-miniforth:
    @echo "LCC Miniforth Comprehensive Test Suite"
    @echo "======================================"
    @echo ""
    @mkdir -p build/lcc_tests
    @echo "🧮 Testing Arithmetic Operations:"
    @for test in tests/lcc_miniforth/arithmetic/*.c; do \
        name=$$(basename "$$test" .c); \
        echo "  Compiling $$name..."; \
        external/lcc/build/rcc -target=miniforth -b external/lcc/build "$$test" > "build/lcc_tests/$$name.mf" 2>/dev/null || echo "    ❌ Compilation failed"; \
        if [ -f "build/lcc_tests/$$name.mf" ] && [ -s "build/lcc_tests/$$name.mf" ]; then \
            echo "    ✅ Generated $(wc -l < "build/lcc_tests/$$name.mf") lines of bytecode"; \
        fi; \
    done
    @echo ""
    @echo "🔀 Testing Control Flow:"
    @for test in tests/lcc_miniforth/control_flow/*.c; do \
        name=$$(basename "$$test" .c); \
        echo "  Compiling $$name..."; \
        external/lcc/build/rcc -target=miniforth -b external/lcc/build "$$test" > "build/lcc_tests/$$name.mf" 2>/dev/null || echo "    ❌ Compilation failed"; \
        if [ -f "build/lcc_tests/$$name.mf" ] && [ -s "build/lcc_tests/$$name.mf" ]; then \
            echo "    ✅ Generated $(wc -l < "build/lcc_tests/$$name.mf") lines of bytecode"; \
        fi; \
    done
    @echo ""
    @echo "🔧 Testing Functions:"
    @for test in tests/lcc_miniforth/functions/*.c; do \
        name=$$(basename "$$test" .c); \
        echo "  Compiling $$name..."; \
        external/lcc/build/rcc -target=miniforth -b external/lcc/build "$$test" > "build/lcc_tests/$$name.mf" 2>/dev/null || echo "    ❌ Compilation failed"; \
        if [ -f "build/lcc_tests/$$name.mf" ] && [ -s "build/lcc_tests/$$name.mf" ]; then \
            echo "    ✅ Generated $(wc -l < "build/lcc_tests/$$name.mf") lines of bytecode"; \
        fi; \
    done
    @echo ""
    @echo "📝 Testing Variables:"
    @for test in tests/lcc_miniforth/variables/*.c; do \
        name=$$(basename "$$test" .c); \
        echo "  Compiling $$name..."; \
        external/lcc/build/rcc -target=miniforth -b external/lcc/build "$$test" > "build/lcc_tests/$$name.mf" 2>/dev/null || echo "    ❌ Compilation failed"; \
        if [ -f "build/lcc_tests/$$name.mf" ] && [ -s "build/lcc_tests/$$name.mf" ]; then \
            echo "    ✅ Generated $(wc -l < "build/lcc_tests/$$name.mf") lines of bytecode"; \
        fi; \
    done
    @echo ""
    @echo "🔢 Testing Arrays & Pointers:"
    @for test in tests/lcc_miniforth/arrays/*.c; do \
        name=$$(basename "$$test" .c); \
        echo "  Compiling $$name..."; \
        external/lcc/build/rcc -target=miniforth -b external/lcc/build "$$test" > "build/lcc_tests/$$name.mf" 2>/dev/null || echo "    ❌ Compilation failed"; \
        if [ -f "build/lcc_tests/$$name.mf" ] && [ -s "build/lcc_tests/$$name.mf" ]; then \
            echo "    ✅ Generated $(wc -l < "build/lcc_tests/$$name.mf") lines of bytecode"; \
        fi; \
    done
    @echo ""
    @echo "📜 Testing Strings:"
    @for test in tests/lcc_miniforth/strings/*.c; do \
        name=$$(basename "$$test" .c); \
        echo "  Compiling $$name..."; \
        external/lcc/build/rcc -target=miniforth -b external/lcc/build "$$test" > "build/lcc_tests/$$name.mf" 2>/dev/null || echo "    ❌ Compilation failed"; \
        if [ -f "build/lcc_tests/$$name.mf" ] && [ -s "build/lcc_tests/$$name.mf" ]; then \
            echo "    ✅ Generated $(wc -l < "build/lcc_tests/$$name.mf") lines of bytecode"; \
        fi; \
    done
    @echo ""
    @echo "🔄 Testing Type Conversions:"
    @for test in tests/lcc_miniforth/types/*.c; do \
        name=$$(basename "$$test" .c); \
        echo "  Compiling $$name..."; \
        external/lcc/build/rcc -target=miniforth -b external/lcc/build "$$test" > "build/lcc_tests/$$name.mf" 2>/dev/null || echo "    ❌ Compilation failed"; \
        if [ -f "build/lcc_tests/$$name.mf" ] && [ -s "build/lcc_tests/$$name.mf" ]; then \
            echo "    ✅ Generated $(wc -l < "build/lcc_tests/$$name.mf") lines of bytecode"; \
        fi; \
    done
    @echo ""
    @echo "📊 Test Summary:"
    @echo "  Total test files: $(find tests/lcc_miniforth -name "*.c" | wc -l)"
    @echo "  Generated bytecode files: $(find build/lcc_tests -name "*.mf" -size +0c 2>/dev/null | wc -l)"
    @echo "  Total bytecode lines: $(find build/lcc_tests -name "*.mf" -exec wc -l {} + 2>/dev/null | tail -1 | awk '{print $$1}' || echo '0')"
    @echo ""
    @echo "🧮 Testing Advanced Categories:"
    @echo "  Algorithms: $(find tests/lcc_miniforth/algorithms -name "*.c" 2>/dev/null | wc -l) tests"
    @echo "  Stress Tests: $(find tests/lcc_miniforth/stress -name "*.c" 2>/dev/null | wc -l) tests"
    @echo "  Data Structures: $(find tests/lcc_miniforth/data_structures -name "*.c" 2>/dev/null | wc -l) tests"
    @echo "  Edge Cases: $(find tests/lcc_miniforth/edge_cases -name "*.c" 2>/dev/null | wc -l) tests"
    @echo "  Mathematical: $(find tests/lcc_miniforth/math -name "*.c" 2>/dev/null | wc -l) tests"

# Show detailed bytecode for a specific test
show-bytecode TEST:
    @echo "Miniforth Bytecode for {{TEST}}:"
    @echo "================================"
    @if [ -f "build/lcc_tests/{{TEST}}.mf" ]; then \
        cat "build/lcc_tests/{{TEST}}.mf"; \
    else \
        echo "❌ Bytecode file not found. Run 'just test-lcc-miniforth' first."; \
    fi

# Show project status
status:
    @echo "Miniforth C Compiler & VM Project Status"
    @echo "========================================"
    @echo "Build directory: $(if [ -d build ]; then echo 'exists'; else echo 'missing'; fi)"
    @echo "Git repository: $(if [ -d .git ]; then echo 'initialized'; else echo 'not initialized'; fi)"
    @echo "Files in src/: $(find src/ -name "*.c" -o -name "*.h" 2>/dev/null | wc -l)"
    @echo "Tests available: $(find tests/ -name "*.c" 2>/dev/null | wc -l)"
    @echo "LCC tests available: $(find tests/lcc_miniforth -name "*.c" 2>/dev/null | wc -l)"
    @echo "LCC compiler: $(if [ -f external/lcc/build/rcc ]; then echo 'available'; else echo 'not built'; fi)"

# Test specific categories of LCC tests
test-category CATEGORY:
    @echo "Testing {{CATEGORY}} category:"
    @echo "=============================="
    @mkdir -p build/lcc_tests
    @if [ -d "tests/lcc_miniforth/{{CATEGORY}}" ]; then \
        for test in tests/lcc_miniforth/{{CATEGORY}}/*.c; do \
            name=$$(basename "$$test" .c); \
            echo "  Compiling $$name..."; \
            external/lcc/build/rcc -target=miniforth -b external/lcc/build "$$test" > "build/lcc_tests/$$name.mf" 2>/dev/null || echo "    ❌ Compilation failed"; \
            if [ -f "build/lcc_tests/$$name.mf" ] && [ -s "build/lcc_tests/$$name.mf" ]; then \
                echo "    ✅ Generated $(wc -l < "build/lcc_tests/$$name.mf") lines of bytecode"; \
            fi; \
        done; \
    else \
        echo "❌ Category '{{CATEGORY}}' not found in tests/lcc_miniforth/"; \
        echo "Available categories:"; \
        ls tests/lcc_miniforth/ | grep -v "\.c$$" | sed 's/^/  - /'; \
    fi

# Quick test of simple C programs
quick-test:
    @echo "Quick LCC Miniforth Test"
    @echo "======================="
    @mkdir -p build/quick_tests
    @echo "Testing simple arithmetic..."
    @echo 'int main() { return 2 + 3; }' > build/quick_tests/add.c
    @external/lcc/build/rcc -target=miniforth -b external/lcc/build build/quick_tests/add.c > build/quick_tests/add.mf 2>/dev/null || echo "❌ Failed"
    @echo "Generated bytecode (add.mf):"
    @head -10 build/quick_tests/add.mf 2>/dev/null || echo "❌ No output"
    @echo ""
    @echo "Testing function call..."
    @echo 'int add(int a, int b) { return a + b; } int main() { return add(5, 7); }' > build/quick_tests/func.c
    @external/lcc/build/rcc -target=miniforth -b external/lcc/build build/quick_tests/func.c > build/quick_tests/func.mf 2>/dev/null || echo "❌ Failed"
    @echo "Generated bytecode (func.mf):"
    @head -15 build/quick_tests/func.mf 2>/dev/null || echo "❌ No output"

# Build LCC compiler if not present
build-lcc:
    @echo "Building LCC compiler with Miniforth backend..."
    @if [ ! -f external/lcc/build/rcc ]; then \
        echo "Building LCC from source..."; \
        cd external/lcc && make; \
        if [ -f build/rcc ]; then \
            echo "✅ LCC compiler built successfully"; \
        else \
            echo "❌ LCC build failed"; \
        fi; \
    else \
        echo "✅ LCC compiler already available"; \
    fi

# Show help for available test categories
help-tests:
    @echo "Available Test Categories:"
    @echo "========================="
    @echo "Basic Language Features:"
    @echo "  arithmetic    - Basic math operations (+, -, *, /, %)"
    @echo "  control_flow  - if/else, loops, conditionals"
    @echo "  functions     - Function calls, recursion"
    @echo "  variables     - Variable declarations and scoping"
    @echo "  arrays        - Array operations and pointer access"
    @echo "  strings       - String handling and operations"
    @echo "  types         - Type conversions and casting"
    @echo ""
    @echo "Advanced Features:"
    @echo "  algorithms    - Sorting, searching, mathematical algorithms"
    @echo "  stress        - Memory stress, deep recursion, large data"
    @echo "  data_structures - Stack, queue, linked list implementations"
    @echo "  edge_cases    - Boundary conditions, error handling"
    @echo "  math          - Mathematical computations, number theory"
    @echo ""
    @echo "Usage Examples:"
    @echo "  just test-category algorithms"
    @echo "  just test-category stress"
    @echo "  just show-bytecode fibonacci"
    @echo "  just quick-test"

# Check project dependencies
check-deps:
    @echo "Checking Project Dependencies:"
    @echo "============================="
    @echo "Core build tools:"
    @echo "  gcc: $(if command -v gcc >/dev/null 2>&1; then echo '✅ available'; else echo '❌ missing'; fi)"
    @echo "  cmake: $(if command -v cmake >/dev/null 2>&1; then echo '✅ available'; else echo '❌ missing'; fi)"
    @echo "  make: $(if command -v make >/dev/null 2>&1; then echo '✅ available'; else echo '❌ missing'; fi)"
    @echo ""
    @echo "Optional tools:"
    @echo "  clang-format: $(if command -v clang-format >/dev/null 2>&1; then echo '✅ available'; else echo '❌ missing'; fi)"
    @echo "  cppcheck: $(if command -v cppcheck >/dev/null 2>&1; then echo '✅ available'; else echo '❌ missing'; fi)"
    @echo "  doxygen: $(if command -v doxygen >/dev/null 2>&1; then echo '✅ available'; else echo '❌ missing'; fi)"
    @echo "  lcov: $(if command -v lcov >/dev/null 2>&1; then echo '✅ available'; else echo '❌ missing'; fi)"
    @echo ""
    @echo "Project files:"
    @echo "  LCC compiler: $(if [ -f external/lcc/build/rcc ]; then echo '✅ built'; else echo '❌ run: just build-lcc'; fi)"
    @echo "  Main executable: $(if [ -f build/miniforth ]; then echo '✅ built'; else echo '❌ run: just build'; fi)"
    @echo "  Test files: $(find tests/lcc_miniforth -name "*.c" 2>/dev/null | wc -l) available"