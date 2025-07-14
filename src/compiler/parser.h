#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include <stdint.h>

// AST node types (from PRD)
typedef enum {
    AST_PROGRAM,
    AST_FUNCTION,
    AST_VARIABLE_DECL,
    AST_ARRAY_DECL,
    AST_BINARY_OP,
    AST_UNARY_OP,
    AST_ASSIGNMENT,
    AST_FUNCTION_CALL,
    AST_IF_STMT,
    AST_WHILE_STMT,
    AST_FOR_STMT,
    AST_RETURN_STMT,
    AST_BLOCK,
    AST_IDENTIFIER,
    AST_NUMBER,
    AST_ARRAY_ACCESS
} ast_node_type_t;

// Forward declaration
typedef struct ast_node ast_node_t;

// AST node structure
struct ast_node {
    ast_node_type_t type;
    union {
        struct { char *name; ast_node_t *params; ast_node_t *body; } function;
        struct { char *name; int size; } var_decl;
        struct { int op; ast_node_t *left; ast_node_t *right; } binary;
        struct { int op; ast_node_t *operand; } unary;
        struct { ast_node_t *target; ast_node_t *value; } assignment;
        struct { char *name; ast_node_t *args; } call;
        struct { ast_node_t *condition; ast_node_t *then_stmt; ast_node_t *else_stmt; } if_stmt;
        struct { ast_node_t *condition; ast_node_t *body; } while_stmt;
        struct { ast_node_t *init; ast_node_t *condition; ast_node_t *increment; ast_node_t *body; } for_stmt;
        struct { ast_node_t *expression; } return_stmt;
        struct { char *name; } identifier;
        struct { int value; } number;
        struct { ast_node_t *array; ast_node_t *index; } array_access;
        struct { ast_node_t *statements; } block;
    } data;
    ast_node_t *next;  // For lists
};

// Parser state
typedef struct {
    lexer_t *lexer;
    token_t current_token;
    token_t previous_token;
    int error_count;
    int panic_mode;
} parser_t;

// Operator precedence levels
typedef enum {
    PREC_NONE,
    PREC_ASSIGNMENT,  // =
    PREC_OR,          // ||
    PREC_AND,         // &&
    PREC_EQUALITY,    // == !=
    PREC_COMPARISON,  // < > <= >=
    PREC_TERM,        // + -
    PREC_FACTOR,      // * / %
    PREC_UNARY,       // ! - +
    PREC_CALL,        // . () []
    PREC_PRIMARY
} precedence_t;

// Parser functions
parser_t* parser_create(lexer_t *lexer);
void parser_destroy(parser_t *parser);
ast_node_t* parse_program(parser_t *parser);
ast_node_t* parse_expression(parser_t *parser);
ast_node_t* parse_statement(parser_t *parser);

// AST node creation functions
ast_node_t* ast_create_binary(int op, ast_node_t *left, ast_node_t *right);
ast_node_t* ast_create_unary(int op, ast_node_t *operand);
ast_node_t* ast_create_number(int value);
ast_node_t* ast_create_identifier(const char *name);
ast_node_t* ast_create_assignment(ast_node_t *target, ast_node_t *value);
ast_node_t* ast_create_variable_decl(const char *name, int size);
ast_node_t* ast_create_function(const char *name, ast_node_t *params, ast_node_t *body);
ast_node_t* ast_create_return(ast_node_t *expression);
ast_node_t* ast_create_block(ast_node_t *statements);

// AST utility functions
void ast_destroy(ast_node_t *node);
void ast_print(ast_node_t *node, int depth);
const char* ast_node_type_string(ast_node_type_t type);

// Error handling
void parser_error(parser_t *parser, const char *message);
void parser_error_at_current(parser_t *parser, const char *message);
void parser_error_at_previous(parser_t *parser, const char *message);

#endif // PARSER_H