#ifndef SYMBOLS_H
#define SYMBOLS_H

#include <stddef.h>
#include <stdint.h>

// Symbol types
typedef enum {
  SYMBOL_VARIABLE,
  SYMBOL_FUNCTION,
  SYMBOL_PARAMETER,
  SYMBOL_LOCAL
} symbol_type_t;

// Symbol entry
typedef struct symbol {
  char *name;          // Symbol name
  symbol_type_t type;  // Symbol type
  uint16_t address;    // Memory address for variables
  int size;            // Size in bytes (for arrays)
  int scope_level;     // Scope nesting level
  struct symbol *next; // Next symbol in hash chain
} symbol_t;

// Symbol table structure
typedef struct {
  symbol_t **table;      // Hash table array
  size_t table_size;     // Size of hash table
  uint16_t next_address; // Next available memory address
  int current_scope;     // Current scope level
  int symbol_count;      // Total number of symbols
} symbol_table_t;

// Symbol table functions
symbol_table_t *symbol_table_create(size_t table_size);
void symbol_table_destroy(symbol_table_t *table);

// Symbol management
symbol_t *symbol_table_declare(symbol_table_t *table, const char *name,
                               symbol_type_t type, int size);
symbol_t *symbol_table_lookup(symbol_table_t *table, const char *name);
symbol_t *symbol_table_lookup_current_scope(symbol_table_t *table,
                                            const char *name);

// Scope management
void symbol_table_enter_scope(symbol_table_t *table);
void symbol_table_exit_scope(symbol_table_t *table);

// Memory management
uint16_t symbol_table_allocate_memory(symbol_table_t *table, int size);
uint16_t symbol_table_get_next_address(symbol_table_t *table);

// Utility functions
void symbol_table_dump(symbol_table_t *table);
const char *symbol_type_string(symbol_type_t type);

#endif // SYMBOLS_H