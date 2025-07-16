#!/bin/bash

cd external/llvm-project

# Clean previous build
rm -rf build

# Create build directory
mkdir -p build
cd build

# Configure with minimal build - only build the necessary tools
cmake -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DLLVM_TARGETS_TO_BUILD="X86;Miniforth" \
    -DLLVM_ENABLE_PROJECTS="" \
    -DLLVM_ENABLE_RUNTIMES="" \
    -DLLVM_INCLUDE_BENCHMARKS=OFF \
    -DLLVM_INCLUDE_EXAMPLES=OFF \
    -DLLVM_INCLUDE_TESTS=OFF \
    -DLLVM_INCLUDE_TOOLS=ON \
    -DLLVM_BUILD_TOOLS=ON \
    -DLLVM_ENABLE_BINDINGS=OFF \
    ../llvm

# Build just the essential components
ninja llc

echo "Build completed. Testing if MiniForth target is available..."
./bin/llc --version | grep -i miniforth || echo "MiniForth target not found in llc"