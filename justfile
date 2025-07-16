# Miniforth Bytecode VM - Main Project


# Core development targets

# Run static analysis on all source code
lint:
	just compiler::lint
	just vm::lint

# Build the entire project
build:
	mkdir -p build
	cd build && cmake .. && make

# Run all tests
test: build
	cd build && ctest --verbose

# Project modules
mod compiler 'src/compiler/justfile'
mod vm 'src/vm/justfile'
mod examples 'tests/examples/justfile'
mod lcc 'external/lcc/justfile'
mod llvm 'external/llvm-project/justfile'

# Utility recipes

# Clean all build artifacts
clean:
	-rm -rf build/

# Run complete development cycle: lint, build, test
dev: lint build test
	echo "✅ Development cycle complete"

# Format all source code with clang-format
format:
	find src/ tests/ -name "*.c" -o -name "*.h" | xargs clang-format -i

