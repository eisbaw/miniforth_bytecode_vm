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

# Measure code size (source and binary)
code-size: build
    @echo "Miniforth Bytecode VM Code Size Analysis"
    @echo "========================================"
    @echo ""
    @echo "📊 Source Code Metrics:"
    @echo "======================"
    @echo "VM Source Files:"
    @find src/vm/ -name "*.c" -o -name "*.h" | xargs wc -l | tail -1 | awk '{print "  Lines of code: " $$1}'
    @find src/vm/ -name "*.c" -o -name "*.h" | xargs wc -c | tail -1 | awk '{print "  Bytes: " $$1}'
    @echo "  Files: $(find src/vm/ -name '*.c' -o -name '*.h' | wc -l)"
    @echo ""
    @echo "Compiler Source Files:"
    @find src/compiler/ -name "*.c" -o -name "*.h" | xargs wc -l | tail -1 | awk '{print "  Lines of code: " $$1}'
    @find src/compiler/ -name "*.c" -o -name "*.h" | xargs wc -c | tail -1 | awk '{print "  Bytes: " $$1}'
    @echo "  Files: $(find src/compiler/ -name '*.c' -o -name '*.h' | wc -l)"
    @echo ""
    @echo "Total Project Source:"
    @find src/ -name "*.c" -o -name "*.h" | xargs wc -l | tail -1 | awk '{print "  Lines of code: " $$1}'
    @find src/ -name "*.c" -o -name "*.h" | xargs wc -c | tail -1 | awk '{print "  Bytes: " $$1}'
    @echo "  Files: $(find src/ -name '*.c' -o -name '*.h' | wc -l)"
    @echo ""
    @echo "💾 Binary Size Analysis:"
    @echo "======================="
    @if [ -f build/miniforth ]; then \
        echo "Main executable (miniforth):"; \
        ls -lh build/miniforth | awk '{print "  Size: " $$5 " (" $$5 " bytes)"}'; \
        echo "  Stripped size: $(strip -s build/miniforth -o /tmp/miniforth_stripped 2>/dev/null && ls -lh /tmp/miniforth_stripped | awk '{print $$5}' && rm -f /tmp/miniforth_stripped || echo 'N/A')"; \
    else \
        echo "Main executable: Not built"; \
    fi
    @echo ""
    @echo "VM Components:"
    @if [ -f build/src/vm/libminiforth_vm.a ]; then \
        echo "  VM Library: $(ls -lh build/src/vm/libminiforth_vm.a 2>/dev/null | awk '{print $$5}' || echo 'N/A')"; \
    fi
    @if [ -f build/src/compiler/libminiforth_compiler.a ]; then \
        echo "  Compiler Library: $(ls -lh build/src/compiler/libminiforth_compiler.a 2>/dev/null | awk '{print $$5}' || echo 'N/A')"; \
    fi
    @echo ""
    @echo "Test Executables:"
    @if [ -f build/tests/vm_tests ]; then \
        echo "  VM Tests: $(ls -lh build/tests/vm_tests | awk '{print $$5}')"; \
    fi
    @if [ -f build/tests/compiler_tests ]; then \
        echo "  Compiler Tests: $(ls -lh build/tests/compiler_tests | awk '{print $$5}')"; \
    fi
    @if [ -f build/tests/integration_tests ]; then \
        echo "  Integration Tests: $(ls -lh build/tests/integration_tests | awk '{print $$5}')"; \
    fi
    @echo ""
    @echo "🔍 Size Breakdown by Component:"
    @echo "=============================="
    @echo "VM Core Instructions:"
    @wc -l src/vm/instructions.c | awk '{print "  instructions.c: " $$1 " lines"}'
    @echo "VM Main Engine:"
    @wc -l src/vm/miniforth.c | awk '{print "  miniforth.c: " $$1 " lines"}'
    @echo "Compiler Lexer:"
    @wc -l src/compiler/lexer.c | awk '{print "  lexer.c: " $$1 " lines"}'
    @echo "Compiler Parser:"
    @wc -l src/compiler/parser.c | awk '{print "  parser.c: " $$1 " lines"}'
    @echo "Code Generator:"
    @wc -l src/compiler/codegen.c | awk '{print "  codegen.c: " $$1 " lines"}'
    @echo "Symbol Table:"
    @wc -l src/compiler/symbols.c | awk '{print "  symbols.c: " $$1 " lines"}'
    @echo ""
    @echo "📈 Build Artifacts Size:"
    @echo "======================="
    @if [ -d build ]; then \
        echo "Total build directory: $(du -sh build | awk '{print $$1}')"; \
        echo "Object files: $(find build -name '*.o' | wc -l) files, $(find build -name '*.o' -exec ls -l {} + 2>/dev/null | awk '{sum+=$$5} END {print sum/1024 "KB"}' || echo '0KB')"; \
    else \
        echo "Build directory: Not found"; \
    fi

# Show project status
status:
    @echo "Miniforth C Compiler & VM Project Status"
    @echo "========================================"
    @echo "Build directory: $(if [ -d build ]; then echo 'exists'; else echo 'missing'; fi)"
    @echo "Git repository: $(if [ -d .git ]; then echo 'initialized'; else echo 'not initialized'; fi)"
    @echo "Files in src/: $(find src/ -name "*.c" -o -name "*.h" 2>/dev/null | wc -l)"
    @echo "Tests available: $(find tests/ -name "*.c" 2>/dev/null | wc -l)"