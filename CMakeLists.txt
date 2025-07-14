cmake_minimum_required(VERSION 3.16)
project(miniforth_c_compiler VERSION 1.0.0 LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Compiler flags
set(CMAKE_C_FLAGS "-Wall -Wextra -Wpedantic -std=c99")
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -DDEBUG")
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")

# Enable testing
enable_testing()

# Code coverage support
option(ENABLE_COVERAGE "Enable code coverage" OFF)
if(ENABLE_COVERAGE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/vm
    ${CMAKE_CURRENT_SOURCE_DIR}/src/compiler
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tools
)

# Add subdirectories
add_subdirectory(src)
add_subdirectory(tests)

# Create main executable
add_executable(miniforth src/main.c)
target_link_libraries(miniforth vm compiler tools)

# Create separate tool executables
add_executable(miniforth-compiler src/main.c)
target_link_libraries(miniforth-compiler vm compiler tools)
target_compile_definitions(miniforth-compiler PRIVATE COMPILER_ONLY)

add_executable(miniforth-assembler src/main.c)
target_link_libraries(miniforth-assembler vm tools)
target_compile_definitions(miniforth-assembler PRIVATE ASSEMBLER_ONLY)

add_executable(miniforth-debugger src/main.c)
target_link_libraries(miniforth-debugger vm tools)
target_compile_definitions(miniforth-debugger PRIVATE DEBUGGER_ONLY)

# Install targets
install(TARGETS miniforth miniforth-compiler miniforth-assembler miniforth-debugger
        RUNTIME DESTINATION bin)

# Package configuration
set(CPACK_PACKAGE_NAME "miniforth-c-compiler")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Miniforth C Compiler & VM")
set(CPACK_PACKAGE_VENDOR "TinyForth-C Project")
include(CPack)

# Print configuration summary
message(STATUS "")
message(STATUS "Miniforth C Compiler & VM Configuration Summary")
message(STATUS "===============================================")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "C Flags: ${CMAKE_C_FLAGS}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Coverage enabled: ${ENABLE_COVERAGE}")
message(STATUS "")