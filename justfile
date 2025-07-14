# Miniforth C Compiler & VM Development Commands

# Show all available commands
default:
    @just --list

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

# Show project status
status:
    @echo "Miniforth C Compiler & VM Project Status"
    @echo "========================================"
    @echo "Build directory: $(if [ -d build ]; then echo 'exists'; else echo 'missing'; fi)"
    @echo "Git repository: $(if [ -d .git ]; then echo 'initialized'; else echo 'not initialized'; fi)"
    @echo "Files in src/: $(find src/ -name "*.c" -o -name "*.h" 2>/dev/null | wc -l)"
    @echo "Tests available: $(find tests/ -name "*.c" 2>/dev/null | wc -l)"