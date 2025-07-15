#include "miniforth.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_usage(const char *program_name) {
  printf("Usage: %s [options] [file]\n", program_name);
  printf("Options:\n");
  printf("  -h, --help     Show this help message\n");
  printf("  -v, --version  Show version information\n");
  printf("  -t, --test     Run built-in tests\n");
  printf("  -d, --debug    Enable debug output\n");
  printf("  -s, --step     Single step execution\n");
  printf("\nExamples:\n");
  printf("  %s program.mf     Run Miniforth program\n", program_name);
  printf("  %s -t             Run tests\n", program_name);
  printf("  %s -d program.mf  Run with debug output\n", program_name);
}

void print_version() {
  printf("Miniforth C Compiler & VM v1.0.0\n");
  printf("Built with C99 standard\n");
  printf("Configuration:\n");
  printf("  Word size: %d bits\n", MINIFORTH_WORD_SIZE);
  printf("  Default code size: %d words\n", MINIFORTH_DEFAULT_CODE_SIZE);
  printf("  Default data size: %d words\n", MINIFORTH_DEFAULT_DATA_SIZE);
  printf("  Default stack size: %d words\n", MINIFORTH_DEFAULT_STACK_SIZE);
  printf("  Debugging: %s\n",
         MINIFORTH_ENABLE_DEBUGGING ? "enabled" : "disabled");
  printf("  Bounds checking: %s\n",
         MINIFORTH_ENABLE_BOUNDS_CHECK ? "enabled" : "disabled");
}

int run_basic_test() {
  printf("Running basic VM test...\n");

  miniforth_vm_t *vm = vm_create(64, 64, 32);
  if (!vm) {
    printf("Failed to create VM\n");
    return 1;
  }

  // Simple test: 2 + 3 = 5
  uint16_t program[] = {OP_LIT, 2, OP_LIT, 3, OP_ADD, OP_EXIT};

  if (vm_load_program(vm, program, 6) != 0) {
    printf("Failed to load program\n");
    vm_destroy(vm);
    return 1;
  }

  vm_error_t error = vm_run(vm);
  if (error != VM_OK) {
    printf("VM execution failed: %s\n", vm_error_string(error));
    vm_destroy(vm);
    return 1;
  }

  uint16_t result;
  if (vm_pop(vm, &result) != VM_OK) {
    printf("Failed to get result\n");
    vm_destroy(vm);
    return 1;
  }

  if (result == 5) {
    printf("Test PASSED: 2 + 3 = %d\n", result);
    vm_destroy(vm);
    return 0;
  } else {
    printf("Test FAILED: Expected 5, got %d\n", result);
    vm_destroy(vm);
    return 1;
  }
}

int main(int argc, char *argv[]) {
  int debug = 0;
  int step = 0;
  const char *filename = NULL;

  // Parse command line arguments
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
      print_usage(argv[0]);
      return 0;
    } else if (strcmp(argv[i], "-v") == 0 ||
               strcmp(argv[i], "--version") == 0) {
      print_version();
      return 0;
    } else if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--test") == 0) {
      return run_basic_test();
    } else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--debug") == 0) {
      debug = 1;
    } else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--step") == 0) {
      step = 1;
    } else if (argv[i][0] != '-') {
      filename = argv[i];
    } else {
      printf("Unknown option: %s\n", argv[i]);
      print_usage(argv[0]);
      return 1;
    }
  }

  if (!filename) {
    printf("No input file specified\n");
    print_usage(argv[0]);
    return 1;
  }

  // TODO: Implement file loading and execution
  printf("File execution not yet implemented: %s\n", filename);
  printf("Debug mode: %s\n", debug ? "on" : "off");
  printf("Step mode: %s\n", step ? "on" : "off");

  return 0;
}