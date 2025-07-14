# VM library CMake configuration

# VM library source files
set(VM_SOURCES
    miniforth.c
    instructions.c
)

# Create VM library
add_library(vm STATIC ${VM_SOURCES})

# Set properties
set_target_properties(vm PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
)

# Compiler flags specific to VM
target_compile_options(vm PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -std=c99
)

# Include directories
target_include_directories(vm PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)