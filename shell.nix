{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  buildInputs = with pkgs; [
    # Build system
    cmake
    pkg-config
    
    # C compiler with C99 support
    gcc
    clang
    
    # Command runner
    just
    
    # Development tools
    valgrind
    clang-tools # includes clang-format
    cppcheck
    lcov
    doxygen
    
    # Standard development tools
    gdb
    gnumake
    
    # Git for version control
    git
  ];
  
  shellHook = ''
    echo "Miniforth C Compiler & VM Development Environment"
    echo "================================================"
    echo ""
    echo "Available commands:"
    echo "  just --list    - Show all available commands"
    echo "  just build     - Build the project"
    echo "  just test      - Run all tests"
    echo "  just clean     - Clean build artifacts"
    echo ""
    echo "C Compiler: $(gcc --version | head -n1)"
    echo "CMake: $(cmake --version | head -n1)"
    echo "Just: $(just --version)"
    echo ""
    
    # Create build directory if it doesn't exist
    mkdir -p build
    
    # Set up environment variables
    export CC=gcc
    export CXX=g++
    export CMAKE_BUILD_TYPE=Debug
    export CFLAGS="-Wall -Wextra -std=c99"
  '';
}