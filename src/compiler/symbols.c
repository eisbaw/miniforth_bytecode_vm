#include "symbols.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Default symbol table size
#define DEFAULT_TABLE_SIZE 128

// Simple hash function
static unsigned int hash_string(const char *str, size_t table_size) {
  unsigned int hash = 5381;
  int c;

  while ((c = *str++)) {
    hash = ((hash << 5) + hash) + c; // hash * 33 + c
  }

  return hash % table_size;
}

// Symbol table creation and destruction
symbol_table_t *symbol_table_create(size_t table_size) {
  if (table_size == 0) {
    table_size = DEFAULT_TABLE_SIZE;
  }

  symbol_table_t *table = malloc(sizeof(symbol_table_t));
  if (!table)
    return NULL;

  table->table = calloc(table_size, sizeof(symbol_t *));
  if (!table->table) {
    free(table);
    return NULL;
  }

  table->table_size = table_size;
  table->next_address = 0;  // Start from address 0
  table->current_scope = 0; // Global scope
  table->symbol_count = 0;

  return table;
}

static void symbol_destroy(symbol_t *symbol) {
  if (symbol) {
    free(symbol->name);
    free(symbol);
  }
}

void symbol_table_destroy(symbol_table_t *table) {
  if (!table)
    return;

  // Free all symbols in all hash chains
  for (size_t i = 0; i < table->table_size; i++) {
    symbol_t *current = table->table[i];
    while (current) {
      symbol_t *next = current->next;
      symbol_destroy(current);
      current = next;
    }
  }

  free(table->table);
  free(table);
}

// Symbol management
symbol_t *symbol_table_declare(symbol_table_t *table, const char *name,
                               symbol_type_t type, int size) {
  if (!table || !name)
    return NULL;

  // Check if symbol already exists in current scope
  const symbol_t *existing = symbol_table_lookup_current_scope(table, name);
  if (existing) {
    // Symbol already declared in current scope
    return NULL;
  }

  // Create new symbol
  symbol_t *symbol = malloc(sizeof(symbol_t));
  if (!symbol)
    return NULL;

  symbol->name = malloc(strlen(name) + 1);
  if (!symbol->name) {
    free(symbol);
    return NULL;
  }
  strcpy(symbol->name, name);

  symbol->type = type;
  symbol->size = size;
  symbol->scope_level = table->current_scope;

  // Allocate memory address for variables
  if (type == SYMBOL_VARIABLE || type == SYMBOL_LOCAL ||
      type == SYMBOL_PARAMETER) {
    symbol->address = symbol_table_allocate_memory(table, size);
  } else {
    symbol->address = 0;
  }

  // Insert into hash table
  unsigned int hash = hash_string(name, table->table_size);
  symbol->next = table->table[hash];
  table->table[hash] = symbol;

  table->symbol_count++;

  return symbol;
}

symbol_t *symbol_table_lookup(symbol_table_t *table, const char *name) {
  if (!table || !name)
    return NULL;

  unsigned int hash = hash_string(name, table->table_size);
  symbol_t *current = table->table[hash];

  // Search through the hash chain for the symbol
  // Return the most recently declared symbol (highest scope level)
  symbol_t *best_match = NULL;
  while (current) {
    if (strcmp(current->name, name) == 0) {
      if (!best_match || current->scope_level > best_match->scope_level) {
        best_match = current;
      }
    }
    current = current->next;
  }

  return best_match;
}

symbol_t *symbol_table_lookup_current_scope(symbol_table_t *table,
                                            const char *name) {
  if (!table || !name)
    return NULL;

  unsigned int hash = hash_string(name, table->table_size);
  symbol_t *current = table->table[hash];

  // Search for symbol in current scope only
  while (current) {
    if (strcmp(current->name, name) == 0 &&
        current->scope_level == table->current_scope) {
      return current;
    }
    current = current->next;
  }

  return NULL;
}

// Scope management
void symbol_table_enter_scope(symbol_table_t *table) {
  if (table) {
    table->current_scope++;
  }
}

void symbol_table_exit_scope(symbol_table_t *table) {
  if (!table)
    return;

  // Remove all symbols from the current scope
  for (size_t i = 0; i < table->table_size; i++) {
    symbol_t **current = &table->table[i];

    while (*current) {
      if ((*current)->scope_level == table->current_scope) {
        symbol_t *to_remove = *current;
        *current = (*current)->next;
        symbol_destroy(to_remove);
        table->symbol_count--;
      } else {
        current = &(*current)->next;
      }
    }
  }

  table->current_scope--;
}

// Memory management
uint16_t symbol_table_allocate_memory(symbol_table_t *table, int size) {
  if (!table || size <= 0)
    return 0;

  uint16_t address = table->next_address;
  table->next_address += size;

  return address;
}

uint16_t symbol_table_get_next_address(symbol_table_t *table) {
  return table ? table->next_address : 0;
}

// Utility functions
const char *symbol_type_string(symbol_type_t type) {
  switch (type) {
  case SYMBOL_VARIABLE:
    return "VARIABLE";
  case SYMBOL_FUNCTION:
    return "FUNCTION";
  case SYMBOL_PARAMETER:
    return "PARAMETER";
  case SYMBOL_LOCAL:
    return "LOCAL";
  default:
    return "UNKNOWN";
  }
}

void symbol_table_dump(symbol_table_t *table) {
  if (!table) {
    printf("Symbol table is NULL\n");
    return;
  }

  printf("Symbol Table (scope: %d, symbols: %d, next_addr: %u)\n",
         table->current_scope, table->symbol_count, table->next_address);
  printf("================================================\n");

  for (size_t i = 0; i < table->table_size; i++) {
    symbol_t *current = table->table[i];
    while (current) {
      printf("  %s: %s (addr=%u, size=%d, scope=%d)\n", current->name,
             symbol_type_string(current->type), current->address, current->size,
             current->scope_level);
      current = current->next;
    }
  }
  printf("================================================\n");
}