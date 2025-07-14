#include "codegen.h"
#include "lexer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Code generator creation and destruction
codegen_t* codegen_create(size_t initial_capacity) {
    codegen_t *codegen = malloc(sizeof(codegen_t));
    if (!codegen) return NULL;
    
    codegen->code = malloc(initial_capacity * sizeof(uint16_t));
    if (!codegen->code) {
        free(codegen);
        return NULL;
    }
    
    codegen->code_capacity = initial_capacity;
    codegen->code_size = 0;
    codegen->symbols = symbol_table_create(0);  // Use default size
    codegen->error_count = 0;
    
    if (!codegen->symbols) {
        free(codegen->code);
        free(codegen);
        return NULL;
    }
    
    return codegen;
}

void codegen_destroy(codegen_t *codegen) {
    if (codegen) {
        free(codegen->code);
        symbol_table_destroy(codegen->symbols);
        free(codegen);
    }
}

// Utility functions
int codegen_emit(codegen_t *codegen, uint16_t instruction) {
    if (codegen->code_size >= codegen->code_capacity) {
        // Expand buffer
        size_t new_capacity = codegen->code_capacity * 2;
        uint16_t *new_code = realloc(codegen->code, new_capacity * sizeof(uint16_t));
        if (!new_code) {
            codegen_error(codegen, "Out of memory expanding code buffer");
            return -1;
        }
        codegen->code = new_code;
        codegen->code_capacity = new_capacity;
    }
    
    codegen->code[codegen->code_size++] = instruction;
    return 0;
}

int codegen_emit_with_operand(codegen_t *codegen, uint16_t opcode, uint16_t operand) {
    if (codegen_emit(codegen, opcode) < 0) return -1;
    return codegen_emit(codegen, operand);
}

size_t codegen_get_current_address(codegen_t *codegen) {
    return codegen->code_size;
}

// Expression compilation
int codegen_expression(codegen_t *codegen, ast_node_t *expr) {
    if (!expr) {
        codegen_error(codegen, "NULL expression");
        return -1;
    }
    
    switch (expr->type) {
        case AST_NUMBER:
            // Emit LIT instruction with the number value
            return codegen_emit_with_operand(codegen, OP_LIT, expr->data.number.value);
            
        case AST_IDENTIFIER: {
            // Look up variable in symbol table
            char name[256];
            int len = strlen(expr->data.identifier.name);
            if (len >= 256) len = 255;
            strncpy(name, expr->data.identifier.name, len);
            name[len] = '\0';
            
            symbol_t *symbol = symbol_table_lookup(codegen->symbols, name);
            if (!symbol) {
                codegen_error(codegen, "Undefined variable");
                return -1;
            }
            
            if (symbol->type != SYMBOL_VARIABLE && symbol->type != SYMBOL_LOCAL && symbol->type != SYMBOL_PARAMETER) {
                codegen_error(codegen, "Identifier is not a variable");
                return -1;
            }
            
            // Emit LIT <address>, FETCH to load variable value
            if (codegen_emit_with_operand(codegen, OP_LIT, symbol->address) < 0) return -1;
            return codegen_emit(codegen, OP_FETCH);
        }
            
        case AST_BINARY_OP: {
            // Handle assignment separately
            if (expr->data.binary.op == TOKEN_ASSIGN) {
                // Compile the right operand (value) first
                if (codegen_expression(codegen, expr->data.binary.right) < 0) return -1;
                
                // Handle assignment target (left operand)
                ast_node_t *target = expr->data.binary.left;
                if (target->type == AST_IDENTIFIER) {
                    symbol_t *symbol = symbol_table_lookup(codegen->symbols, target->data.identifier.name);
                    if (!symbol) {
                        codegen_error(codegen, "Undefined variable in assignment");
                        return -1;
                    }
                    
                    // Duplicate the value for the assignment result
                    if (codegen_emit(codegen, OP_DUP) < 0) return -1;
                    
                    // Store the value: value addr STORE
                    if (codegen_emit_with_operand(codegen, OP_LIT, symbol->address) < 0) return -1;
                    if (codegen_emit(codegen, OP_STORE) < 0) return -1;
                    
                    // The duplicated value remains on stack as the result
                    return 0;
                } else if (target->type == AST_ARRAY_ACCESS) {
                    // Array assignment: arr[index] = value
                    ast_node_t *array_node = target->data.array_access.array;
                    if (array_node->type != AST_IDENTIFIER) {
                        codegen_error(codegen, "Array assignment requires identifier");
                        return -1;
                    }
                    
                    symbol_t *array_symbol = symbol_table_lookup(codegen->symbols, array_node->data.identifier.name);
                    if (!array_symbol) {
                        codegen_error(codegen, "Undefined array in assignment");
                        return -1;
                    }
                    
                    // Duplicate the value for the assignment result
                    if (codegen_emit(codegen, OP_DUP) < 0) return -1;
                    
                    // Compile the index expression
                    if (codegen_expression(codegen, target->data.array_access.index) < 0) return -1;
                    
                    // Add the base address to the index: base_addr + index
                    if (codegen_emit_with_operand(codegen, OP_LIT, array_symbol->address) < 0) return -1;
                    if (codegen_emit(codegen, OP_ADD) < 0) return -1;
                    
                    // Store the value at the computed address
                    if (codegen_emit(codegen, OP_STORE) < 0) return -1;
                    
                    // The duplicated value remains on stack as the result
                    return 0;
                } else {
                    codegen_error(codegen, "Invalid assignment target");
                    return -1;
                }
            }
            
            // Regular binary operations
            // Compile left operand
            if (codegen_expression(codegen, expr->data.binary.left) < 0) return -1;
            
            // Compile right operand
            if (codegen_expression(codegen, expr->data.binary.right) < 0) return -1;
            
            // Emit operation
            switch (expr->data.binary.op) {
                case TOKEN_PLUS:
                    return codegen_emit(codegen, OP_ADD);
                case TOKEN_MINUS:
                    return codegen_emit(codegen, OP_SUB);
                case TOKEN_MULTIPLY:
                    return codegen_emit(codegen, OP_MUL);
                case TOKEN_DIVIDE:
                    return codegen_emit(codegen, OP_DIV);
                case TOKEN_EQUAL:
                    return codegen_emit(codegen, OP_EQ);
                case TOKEN_LESS:
                    return codegen_emit(codegen, OP_LT);
                default:
                    codegen_error(codegen, "Unsupported binary operator");
                    return -1;
            }
        }
        
        case AST_UNARY_OP: {
            // Compile operand
            if (codegen_expression(codegen, expr->data.unary.operand) < 0) return -1;
            
            // Emit operation
            switch (expr->data.unary.op) {
                case TOKEN_MINUS:
                    // Unary minus: push 0, swap, subtract
                    if (codegen_emit_with_operand(codegen, OP_LIT, 0) < 0) return -1;
                    if (codegen_emit(codegen, OP_SWAP) < 0) return -1;
                    return codegen_emit(codegen, OP_SUB);
                case TOKEN_PLUS:
                    // Unary plus: no-op
                    return 0;
                default:
                    codegen_error(codegen, "Unsupported unary operator");
                    return -1;
            }
        }
        
        case AST_ARRAY_ACCESS: {
            // Array access: array[index]
            // First get the array base address (from the identifier)
            ast_node_t *array_node = expr->data.array_access.array;
            if (array_node->type != AST_IDENTIFIER) {
                codegen_error(codegen, "Array access requires identifier");
                return -1;
            }
            
            symbol_t *array_symbol = symbol_table_lookup(codegen->symbols, array_node->data.identifier.name);
            if (!array_symbol) {
                codegen_error(codegen, "Undefined array");
                return -1;
            }
            
            // Compile the index expression
            if (codegen_expression(codegen, expr->data.array_access.index) < 0) return -1;
            
            // Add the base address to the index: base_addr + index
            if (codegen_emit_with_operand(codegen, OP_LIT, array_symbol->address) < 0) return -1;
            if (codegen_emit(codegen, OP_ADD) < 0) return -1;
            
            // Fetch the value from the computed address
            return codegen_emit(codegen, OP_FETCH);
        }
        
        default:
            codegen_error(codegen, "Unsupported expression type");
            return -1;
    }
}

// Statement compilation
int codegen_statement(codegen_t *codegen, ast_node_t *stmt) {
    if (!stmt) return 0;
    
    switch (stmt->type) {
        case AST_RETURN_STMT:
            if (stmt->data.return_stmt.expression) {
                // Compile return expression
                if (codegen_expression(codegen, stmt->data.return_stmt.expression) < 0) return -1;
            }
            // Emit EXIT instruction
            return codegen_emit(codegen, OP_EXIT);
            
        case AST_BLOCK: {
            // Enter new scope for block
            symbol_table_enter_scope(codegen->symbols);
            
            // Compile all statements in the block
            ast_node_t *current = stmt->data.block.statements;
            while (current) {
                if (codegen_statement(codegen, current) < 0) {
                    symbol_table_exit_scope(codegen->symbols);
                    return -1;
                }
                current = current->next;
            }
            
            // Exit scope
            symbol_table_exit_scope(codegen->symbols);
            return 0;
        }
        
        case AST_VARIABLE_DECL: {
            // Declare variable in symbol table
            symbol_t *symbol = symbol_table_declare(codegen->symbols, 
                                                   stmt->data.var_decl.name, 
                                                   SYMBOL_VARIABLE, 
                                                   stmt->data.var_decl.size);
            if (!symbol) {
                codegen_error(codegen, "Variable already declared or allocation failed");
                return -1;
            }
            
            // Initialize variable to 0
            if (codegen_emit_with_operand(codegen, OP_LIT, 0) < 0) return -1;
            if (codegen_emit_with_operand(codegen, OP_LIT, symbol->address) < 0) return -1;
            return codegen_emit(codegen, OP_STORE);
        }
        
        case AST_ARRAY_DECL: {
            // Declare array in symbol table
            symbol_t *symbol = symbol_table_declare(codegen->symbols, 
                                                   stmt->data.array_decl.name, 
                                                   SYMBOL_VARIABLE, 
                                                   stmt->data.array_decl.size);
            if (!symbol) {
                codegen_error(codegen, "Array already declared or allocation failed");
                return -1;
            }
            
            // Initialize array elements to 0
            for (int i = 0; i < stmt->data.array_decl.size; i++) {
                if (codegen_emit_with_operand(codegen, OP_LIT, 0) < 0) return -1;
                if (codegen_emit_with_operand(codegen, OP_LIT, symbol->address + i) < 0) return -1;
                if (codegen_emit(codegen, OP_STORE) < 0) return -1;
            }
            return 0;
        }
        
        case AST_ASSIGNMENT: {
            // Compile the value expression first
            if (codegen_expression(codegen, stmt->data.assignment.value) < 0) return -1;
            
            // Handle assignment target
            ast_node_t *target = stmt->data.assignment.target;
            if (target->type == AST_IDENTIFIER) {
                // Simple variable assignment
                symbol_t *symbol = symbol_table_lookup(codegen->symbols, target->data.identifier.name);
                if (!symbol) {
                    codegen_error(codegen, "Undefined variable in assignment");
                    return -1;
                }
                
                // Emit LIT <address>, STORE
                if (codegen_emit_with_operand(codegen, OP_LIT, symbol->address) < 0) return -1;
                return codegen_emit(codegen, OP_STORE);
            } else if (target->type == AST_VARIABLE_DECL) {
                // Variable declaration with assignment
                symbol_t *symbol = symbol_table_declare(codegen->symbols, 
                                                       target->data.var_decl.name, 
                                                       SYMBOL_VARIABLE, 
                                                       target->data.var_decl.size);
                if (!symbol) {
                    codegen_error(codegen, "Variable already declared or allocation failed");
                    return -1;
                }
                
                // Store the value at the variable address
                if (codegen_emit_with_operand(codegen, OP_LIT, symbol->address) < 0) return -1;
                return codegen_emit(codegen, OP_STORE);
            } else if (target->type == AST_ARRAY_ACCESS) {
                // Array assignment: arr[index] = value
                // Value is already on stack, now compute address
                
                ast_node_t *array_node = target->data.array_access.array;
                if (array_node->type != AST_IDENTIFIER) {
                    codegen_error(codegen, "Array assignment requires identifier");
                    return -1;
                }
                
                symbol_t *array_symbol = symbol_table_lookup(codegen->symbols, array_node->data.identifier.name);
                if (!array_symbol) {
                    codegen_error(codegen, "Undefined array in assignment");
                    return -1;
                }
                
                // Compile the index expression
                if (codegen_expression(codegen, target->data.array_access.index) < 0) return -1;
                
                // Add the base address to the index: base_addr + index
                if (codegen_emit_with_operand(codegen, OP_LIT, array_symbol->address) < 0) return -1;
                if (codegen_emit(codegen, OP_ADD) < 0) return -1;
                
                // Store the value at the computed address
                return codegen_emit(codegen, OP_STORE);
            } else {
                codegen_error(codegen, "Invalid assignment target");
                return -1;
            }
        }
        
        default:
            // For expressions used as statements, compile and drop result
            if (codegen_expression(codegen, stmt) < 0) return -1;
            return codegen_emit(codegen, OP_DROP);
    }
}

// Main compilation function
int codegen_compile_ast(codegen_t *codegen, ast_node_t *ast) {
    if (!ast) {
        codegen_error(codegen, "NULL AST");
        return -1;
    }
    
    switch (ast->type) {
        case AST_PROGRAM: {
            // Compile all statements in the program
            ast_node_t *current = ast->data.block.statements;
            while (current) {
                if (codegen_statement(codegen, current) < 0) return -1;
                current = current->next;
            }
            // End program with EXIT if not already present
            if (codegen->code_size == 0 || codegen->code[codegen->code_size - 1] != OP_EXIT) {
                if (codegen_emit(codegen, OP_EXIT) < 0) return -1;
            }
            return 0;
        }
        
        default:
            return codegen_statement(codegen, ast);
    }
}

// Error handling
void codegen_error(codegen_t *codegen, const char *message) {
    fprintf(stderr, "Code generation error: %s\n", message);
    codegen->error_count++;
}