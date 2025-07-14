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
    codegen->error_count = 0;
    
    return codegen;
}

void codegen_destroy(codegen_t *codegen) {
    if (codegen) {
        free(codegen->code);
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
            
        case AST_IDENTIFIER:
            // For now, just emit error - variables need symbol table
            codegen_error(codegen, "Variables not yet supported");
            return -1;
            
        case AST_BINARY_OP: {
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
            // Compile all statements in the block
            ast_node_t *current = stmt->data.block.statements;
            while (current) {
                if (codegen_statement(codegen, current) < 0) return -1;
                current = current->next;
            }
            return 0;
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