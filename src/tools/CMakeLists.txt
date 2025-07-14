# Tools library CMake configuration

# Tools library source files
set(TOOLS_SOURCES
    assembler.c
    disassembler.c
    debugger.c
)

# Create tools library
add_library(tools STATIC ${TOOLS_SOURCES})

# Set properties
set_target_properties(tools PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
)

# Compiler flags specific to tools
target_compile_options(tools PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -std=c99
)

# Include directories
target_include_directories(tools PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../vm
)

# Link with VM library
target_link_libraries(tools vm)