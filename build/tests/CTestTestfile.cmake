# CMake generated Testfile for 
# Source directory: /home/mpedersen/topics/miniforth_bytecode_vm/tests
# Build directory: /home/mpedersen/topics/miniforth_bytecode_vm/build/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(vm_tests "/home/mpedersen/topics/miniforth_bytecode_vm/build/tests/vm_tests")
set_tests_properties(vm_tests PROPERTIES  TIMEOUT "30" _BACKTRACE_TRIPLES "/home/mpedersen/topics/miniforth_bytecode_vm/tests/CMakeLists.txt;17;add_test;/home/mpedersen/topics/miniforth_bytecode_vm/tests/CMakeLists.txt;0;")
add_test(compiler_tests "/home/mpedersen/topics/miniforth_bytecode_vm/build/tests/compiler_tests")
set_tests_properties(compiler_tests PROPERTIES  TIMEOUT "30" _BACKTRACE_TRIPLES "/home/mpedersen/topics/miniforth_bytecode_vm/tests/CMakeLists.txt;22;add_test;/home/mpedersen/topics/miniforth_bytecode_vm/tests/CMakeLists.txt;0;")
add_test(integration_tests "/home/mpedersen/topics/miniforth_bytecode_vm/build/tests/integration_tests")
set_tests_properties(integration_tests PROPERTIES  TIMEOUT "60" _BACKTRACE_TRIPLES "/home/mpedersen/topics/miniforth_bytecode_vm/tests/CMakeLists.txt;27;add_test;/home/mpedersen/topics/miniforth_bytecode_vm/tests/CMakeLists.txt;0;")
