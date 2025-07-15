/*
 * ARM bare-metal main for MiniForth VM
 * Minimal main function for microcontroller deployment
 */

#define MINIFORTH_TARGET ARM_BAREMETAL
#define MINIFORTH_SIZE_OPTIMIZE 2

#include "../vm/miniforth_small.h"

// Simple UART output for debugging (platform-specific)
// This is a placeholder - replace with your microcontroller's UART
static void uart_putchar(char c) {
  // Platform-specific UART implementation
  // For example, for STM32:
  // while (!(USART1->SR & USART_SR_TXE));
  // USART1->DR = c;
  (void)c; // Suppress unused parameter warning
}

static void uart_puts(const char *s) {
  while (*s) {
    uart_putchar(*s++);
  }
}

static void uart_puthex(uint16_t value) {
  const char hex[] = "0123456789ABCDEF";
  uart_putchar(hex[(value >> 12) & 0xF]);
  uart_putchar(hex[(value >> 8) & 0xF]);
  uart_putchar(hex[(value >> 4) & 0xF]);
  uart_putchar(hex[value & 0xF]);
}

// Simple test program for the VM
static const uint16_t test_program[] = {
    OP_LIT, 5, // Push 5
    OP_LIT, 3, // Push 3
    OP_ADD,    // Add them
    OP_DUP,    // Duplicate result
    OP_LIT, 2, // Push 2
    OP_MUL,    // Multiply by 2
    OP_EXIT    // End program
};

// Main function
int main(void) {
  // Initialize hardware (platform-specific)
  // This would include clock setup, GPIO, UART, etc.

  uart_puts("MiniForth VM Starting...\r\n");

  // Create VM instance
  miniforth_vm_t *vm = vm_create_static();
  if (!vm) {
    uart_puts("ERROR: Failed to create VM\r\n");
    while (1) { /* Error loop */
    }
  }

  uart_puts("VM Created\r\n");

  // Load test program
  if (vm_load_program(vm, test_program,
                      sizeof(test_program) / sizeof(test_program[0])) != 0) {
    uart_puts("ERROR: Failed to load program\r\n");
    while (1) { /* Error loop */
    }
  }

  uart_puts("Program Loaded\r\n");

  // Execute program
  vm_error_t result = vm_run(vm);

  if (result == VM_OK) {
    uart_puts("Program completed successfully\r\n");

    // Print stack contents
    uart_puts("Final stack:\r\n");
    for (int i = vm->sp - 1; i >= 0; i--) {
      uart_puts("  ");
      uart_puthex(vm->data_stack[i]);
      uart_puts("\r\n");
    }
  } else {
    uart_puts("Program failed with error: ");
    uart_puthex(result);
    uart_puts("\r\n");
  }

  uart_puts("VM execution complete\r\n");

  // Main loop - could implement interactive mode here
  while (1) {
    // Sleep or wait for events
    // Could implement a simple UART command interface
    __asm__("wfi"); // Wait for interrupt (low power)
  }

  return 0;
}