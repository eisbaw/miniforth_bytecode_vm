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
    @find src/ tests/ -name "*.c" -o -name "*.h" | xargs clang-format -i

# Run static analysis with cppcheck
lint:
    @echo "Running static analysis..."
    @cppcheck --enable=all --std=c99 --error-exitcode=1 src/

# Generate code coverage report
coverage: build
    @echo "Generating code coverage report..."
    @cd build && cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON .. && make
    @cd build && make test
    @cd build && lcov --capture --directory . --output-file coverage.info
    @cd build && genhtml coverage.info --output-directory coverage_html
    @echo "Coverage report generated in build/coverage_html/"

# Generate documentation with doxygen
docs:
    @echo "Generating documentation..."
    @doxygen Doxyfile

# Build example programs
examples: build
    @echo "Building example programs..."
    @cd build && make examples

# Run performance benchmarks
benchmark: build
    @echo "Running performance benchmarks..."
    @cd build && ./tests/benchmark

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
    @sudo cp build/miniforth /usr/local/bin/
    @sudo cp build/miniforth-compiler /usr/local/bin/
    @sudo cp build/miniforth-assembler /usr/local/bin/
    @sudo cp build/miniforth-debugger /usr/local/bin/
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

# Show project status
status:
    @echo "Miniforth C Compiler & VM Project Status"
    @echo "========================================"
    @echo "Build directory: $(if [ -d build ]; then echo 'exists'; else echo 'missing'; fi)"
    @echo "Git repository: $(if [ -d .git ]; then echo 'initialized'; else echo 'not initialized'; fi)"
    @echo "Files in src/: $(find src/ -name "*.c" -o -name "*.h" 2>/dev/null | wc -l)"
    @echo "Tests available: $(find tests/ -name "*.c" 2>/dev/null | wc -l)"
    @echo "LCC compiler: $(if [ -f external/lcc/build/rcc ]; then echo 'available'; else echo 'not built'; fi)"