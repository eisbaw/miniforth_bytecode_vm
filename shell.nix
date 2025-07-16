{ pkgs ? (import (builtins.fetchTarball {
  url = "https://github.com/nixos/nixpkgs/tarball/25.05";
  sha256 = "1915r28xc4znrh2vf4rrjnxldw2imysz819gzhk9qlrkqanmfsxd";
}) {}) }:

pkgs.mkShell {
  buildInputs = [ pkgs.gcc pkgs.cmake pkgs.git pkgs.clang-tools pkgs.cppcheck pkgs.just pkgs.ninja ];
  
  shellHook = ''
    # Create build directory if it doesn't exist
    mkdir -p build
    
    # Set up environment variables
    export CC=gcc
    export CXX=g++
    export CMAKE_BUILD_TYPE=Debug
    export CFLAGS="-Wall -Wextra -std=c99"
    
    # Just configuration
    export JUST_LIST_SUBMODULES=true
  '';
}