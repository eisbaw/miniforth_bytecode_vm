# Compiler library CMake configuration

# Compiler library source files
set(COMPILER_SOURCES
    lexer.c
    parser.c
    codegen.c
    symbols.c
)

# Create compiler library
add_library(compiler STATIC ${COMPILER_SOURCES})

# Set properties
set_target_properties(compiler PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
)

# Compiler flags specific to compiler
target_compile_options(compiler PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -std=c99
)

# Include directories
target_include_directories(compiler PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../vm
)

# Link with VM library
target_link_libraries(compiler vm)