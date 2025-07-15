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
    # Create build directory if it doesn't exist
    mkdir -p build
    
    # Set up environment variables
    export CC=gcc
    export CXX=g++
    export CMAKE_BUILD_TYPE=Debug
    export CFLAGS="-Wall -Wextra -std=c99"
  '';
}