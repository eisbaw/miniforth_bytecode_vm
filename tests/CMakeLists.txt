# Test CMake configuration

# Enable testing
enable_testing()

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/vm
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/compiler
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/tools
)

# VM tests
add_executable(vm_tests vm_tests.c)
target_link_libraries(vm_tests vm)
add_test(NAME vm_tests COMMAND vm_tests)

# Compiler tests (will be added later)
add_executable(compiler_tests compiler_tests.c)
target_link_libraries(compiler_tests vm compiler)
add_test(NAME compiler_tests COMMAND compiler_tests)

# Integration tests
add_executable(integration_tests integration_tests.c)
target_link_libraries(integration_tests vm compiler tools)
add_test(NAME integration_tests COMMAND integration_tests)

# Set test properties
set_tests_properties(vm_tests PROPERTIES TIMEOUT 30)
set_tests_properties(compiler_tests PROPERTIES TIMEOUT 30)
set_tests_properties(integration_tests PROPERTIES TIMEOUT 60)

# Create examples target
add_custom_target(examples
    DEPENDS fibonacci_example control_loop_example array_demo_example
)

# Example programs
add_executable(fibonacci_example examples/fibonacci.c)
target_link_libraries(fibonacci_example vm compiler)

add_executable(control_loop_example examples/control_loop.c)
target_link_libraries(control_loop_example vm compiler)

add_executable(array_demo_example examples/array_demo.c)
target_link_libraries(array_demo_example vm compiler)