#include "parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Forward declarations for recursive parsing
static ast_node_t* parse_expression_precedence(parser_t *parser, precedence_t precedence);
static ast_node_t* parse_primary(parser_t *parser);
static ast_node_t* parse_unary(parser_t *parser);
static ast_node_t* parse_binary(parser_t *parser, ast_node_t *left, int op);
static ast_node_t* parse_assignment_expression(parser_t *parser);
static ast_node_t* parse_declaration(parser_t *parser);
static ast_node_t* parse_function(parser_t *parser);
static ast_node_t* parse_variable_declaration(parser_t *parser);

// Token utility functions
static void advance(parser_t *parser);
static int check(parser_t *parser, token_type_t type);
static int match(parser_t *parser, token_type_t type);
static void consume(parser_t *parser, token_type_t type, const char *message);

// Precedence table
static precedence_t get_precedence(token_type_t type) {
    switch (type) {
        case TOKEN_ASSIGN:
        case TOKEN_PLUS_ASSIGN:
        case TOKEN_MINUS_ASSIGN:
        case TOKEN_MULTIPLY_ASSIGN:
        case TOKEN_DIVIDE_ASSIGN:
            return PREC_ASSIGNMENT;
        case TOKEN_LOGICAL_OR:
            return PREC_OR;
        case TOKEN_LOGICAL_AND:
            return PREC_AND;
        case TOKEN_EQUAL:
        case TOKEN_NOT_EQUAL:
            return PREC_EQUALITY;
        case TOKEN_LESS:
        case TOKEN_GREATER:
        case TOKEN_LESS_EQUAL:
        case TOKEN_GREATER_EQUAL:
            return PREC_COMPARISON;
        case TOKEN_PLUS:
        case TOKEN_MINUS:
            return PREC_TERM;
        case TOKEN_MULTIPLY:
        case TOKEN_DIVIDE:
        case TOKEN_MODULO:
            return PREC_FACTOR;
        case TOKEN_LPAREN:
        case TOKEN_LBRACKET:
            return PREC_CALL;
        default:
            return PREC_NONE;
    }
}

// Parser creation and destruction
parser_t* parser_create(lexer_t *lexer) {
    parser_t *parser = malloc(sizeof(parser_t));
    if (!parser) return NULL;
    
    parser->lexer = lexer;
    parser->error_count = 0;
    parser->panic_mode = 0;
    
    // Initialize with first token
    advance(parser);
    
    return parser;
}

void parser_destroy(parser_t *parser) {
    if (parser) {
        free(parser);
    }
}

// Token utilities
static void advance(parser_t *parser) {
    parser->previous_token = parser->current_token;
    parser->current_token = lexer_next_token(parser->lexer);
    
    if (parser->current_token.type == TOKEN_ERROR) {
        parser_error_at_current(parser, parser->current_token.text);
    }
}

static int check(parser_t *parser, token_type_t type) {
    return parser->current_token.type == type;
}

static int match(parser_t *parser, token_type_t type) {
    if (!check(parser, type)) return 0;
    advance(parser);
    return 1;
}

static void consume(parser_t *parser, token_type_t type, const char *message) {
    if (parser->current_token.type == type) {
        advance(parser);
        return;
    }
    parser_error_at_current(parser, message);
}

// AST node creation functions
ast_node_t* ast_create_binary(int op, ast_node_t *left, ast_node_t *right) {
    ast_node_t *node = malloc(sizeof(ast_node_t));
    if (!node) return NULL;
    
    node->type = AST_BINARY_OP;
    node->data.binary.op = op;
    node->data.binary.left = left;
    node->data.binary.right = right;
    node->next = NULL;
    
    return node;
}

ast_node_t* ast_create_unary(int op, ast_node_t *operand) {
    ast_node_t *node = malloc(sizeof(ast_node_t));
    if (!node) return NULL;
    
    node->type = AST_UNARY_OP;
    node->data.unary.op = op;
    node->data.unary.operand = operand;
    node->next = NULL;
    
    return node;
}

ast_node_t* ast_create_number(int value) {
    ast_node_t *node = malloc(sizeof(ast_node_t));
    if (!node) return NULL;
    
    node->type = AST_NUMBER;
    node->data.number.value = value;
    node->next = NULL;
    
    return node;
}

ast_node_t* ast_create_identifier(const char *name) {
    ast_node_t *node = malloc(sizeof(ast_node_t));
    if (!node) return NULL;
    
    node->type = AST_IDENTIFIER;
    node->data.identifier.name = malloc(strlen(name) + 1);
    if (!node->data.identifier.name) {
        free(node);
        return NULL;
    }
    strcpy(node->data.identifier.name, name);
    node->next = NULL;
    
    return node;
}

ast_node_t* ast_create_assignment(ast_node_t *target, ast_node_t *value) {
    ast_node_t *node = malloc(sizeof(ast_node_t));
    if (!node) return NULL;
    
    node->type = AST_ASSIGNMENT;
    node->data.assignment.target = target;
    node->data.assignment.value = value;
    node->next = NULL;
    
    return node;
}

ast_node_t* ast_create_variable_decl(const char *name, int size) {
    ast_node_t *node = malloc(sizeof(ast_node_t));
    if (!node) return NULL;
    
    node->type = AST_VARIABLE_DECL;
    node->data.var_decl.name = malloc(strlen(name) + 1);
    if (!node->data.var_decl.name) {
        free(node);
        return NULL;
    }
    strcpy(node->data.var_decl.name, name);
    node->data.var_decl.size = size;
    node->next = NULL;
    
    return node;
}

ast_node_t* ast_create_function(const char *name, ast_node_t *params, ast_node_t *body) {
    ast_node_t *node = malloc(sizeof(ast_node_t));
    if (!node) return NULL;
    
    node->type = AST_FUNCTION;
    node->data.function.name = malloc(strlen(name) + 1);
    if (!node->data.function.name) {
        free(node);
        return NULL;
    }
    strcpy(node->data.function.name, name);
    node->data.function.params = params;
    node->data.function.body = body;
    node->next = NULL;
    
    return node;
}

ast_node_t* ast_create_return(ast_node_t *expression) {
    ast_node_t *node = malloc(sizeof(ast_node_t));
    if (!node) return NULL;
    
    node->type = AST_RETURN_STMT;
    node->data.return_stmt.expression = expression;
    node->next = NULL;
    
    return node;
}

ast_node_t* ast_create_block(ast_node_t *statements) {
    ast_node_t *node = malloc(sizeof(ast_node_t));
    if (!node) return NULL;
    
    node->type = AST_BLOCK;
    node->data.block.statements = statements;
    node->next = NULL;
    
    return node;
}

ast_node_t* ast_create_array_decl(const char *name, int size) {
    ast_node_t *node = malloc(sizeof(ast_node_t));
    if (!node) return NULL;
    
    node->type = AST_ARRAY_DECL;
    node->data.array_decl.name = malloc(strlen(name) + 1);
    if (!node->data.array_decl.name) {
        free(node);
        return NULL;
    }
    strcpy(node->data.array_decl.name, name);
    node->data.array_decl.size = size;
    node->next = NULL;
    
    return node;
}

ast_node_t* ast_create_array_access(ast_node_t *array, ast_node_t *index) {
    ast_node_t *node = malloc(sizeof(ast_node_t));
    if (!node) return NULL;
    
    node->type = AST_ARRAY_ACCESS;
    node->data.array_access.array = array;
    node->data.array_access.index = index;
    node->next = NULL;
    
    return node;
}

// AST destruction
void ast_destroy(ast_node_t *node) {
    if (!node) return;
    
    switch (node->type) {
        case AST_BINARY_OP:
            ast_destroy(node->data.binary.left);
            ast_destroy(node->data.binary.right);
            break;
        case AST_UNARY_OP:
            ast_destroy(node->data.unary.operand);
            break;
        case AST_ASSIGNMENT:
            ast_destroy(node->data.assignment.target);
            ast_destroy(node->data.assignment.value);
            break;
        case AST_IDENTIFIER:
            free(node->data.identifier.name);
            break;
        case AST_VARIABLE_DECL:
            free(node->data.var_decl.name);
            break;
        case AST_ARRAY_DECL:
            free(node->data.array_decl.name);
            break;
        case AST_ARRAY_ACCESS:
            ast_destroy(node->data.array_access.array);
            ast_destroy(node->data.array_access.index);
            break;
        case AST_FUNCTION:
            free(node->data.function.name);
            ast_destroy(node->data.function.params);
            ast_destroy(node->data.function.body);
            break;
        case AST_RETURN_STMT:
            ast_destroy(node->data.return_stmt.expression);
            break;
        case AST_BLOCK:
            ast_destroy(node->data.block.statements);
            break;
        default:
            break;
    }
    
    // Destroy linked list
    ast_destroy(node->next);
    free(node);
}

// Expression parsing using Pratt parser
static ast_node_t* parse_primary(parser_t *parser) {
    if (match(parser, TOKEN_NUMBER)) {
        return ast_create_number(parser->previous_token.value);
    }
    
    if (match(parser, TOKEN_IDENTIFIER)) {
        char name[256];
        int len = parser->previous_token.length;
        if (len >= 256) len = 255;
        strncpy(name, parser->previous_token.text, len);
        name[len] = '\0';
        
        ast_node_t *identifier = ast_create_identifier(name);
        
        // Check for array access: identifier[index]
        if (check(parser, TOKEN_LBRACKET)) {
            advance(parser);  // consume '['
            ast_node_t *index = parse_expression(parser);
            consume(parser, TOKEN_RBRACKET, "Expected ']' after array index");
            return ast_create_array_access(identifier, index);
        }
        
        return identifier;
    }
    
    if (match(parser, TOKEN_LPAREN)) {
        ast_node_t *expr = parse_expression(parser);
        consume(parser, TOKEN_RPAREN, "Expected ')' after expression");
        return expr;
    }
    
    parser_error_at_current(parser, "Expected expression");
    return NULL;
}

static ast_node_t* parse_unary(parser_t *parser) {
    if (match(parser, TOKEN_LOGICAL_NOT) || match(parser, TOKEN_MINUS) || match(parser, TOKEN_PLUS)) {
        int op = parser->previous_token.type;
        ast_node_t *operand = parse_unary(parser);
        return ast_create_unary(op, operand);
    }
    
    return parse_primary(parser);
}

static ast_node_t* parse_binary(parser_t *parser, ast_node_t *left, int op) {
    precedence_t precedence = get_precedence(op);
    ast_node_t *right = parse_expression_precedence(parser, precedence + 1);
    return ast_create_binary(op, left, right);
}

static ast_node_t* parse_expression_precedence(parser_t *parser, precedence_t precedence) {
    ast_node_t *left = parse_unary(parser);
    
    while (get_precedence(parser->current_token.type) >= precedence) {
        token_type_t op = parser->current_token.type;
        advance(parser);
        left = parse_binary(parser, left, op);
    }
    
    return left;
}

ast_node_t* parse_expression(parser_t *parser) {
    return parse_expression_precedence(parser, PREC_ASSIGNMENT);
}

// Statement parsing
ast_node_t* parse_statement(parser_t *parser) {
    if (match(parser, TOKEN_RETURN)) {
        ast_node_t *expr = NULL;
        if (!check(parser, TOKEN_SEMICOLON)) {
            expr = parse_expression(parser);
        }
        consume(parser, TOKEN_SEMICOLON, "Expected ';' after return statement");
        return ast_create_return(expr);
    }
    
    if (match(parser, TOKEN_LBRACE)) {
        ast_node_t *statements = NULL;
        ast_node_t *last = NULL;
        
        while (!check(parser, TOKEN_RBRACE) && !check(parser, TOKEN_EOF)) {
            ast_node_t *stmt = parse_statement(parser);
            if (stmt) {
                if (!statements) {
                    statements = stmt;
                    last = stmt;
                } else {
                    last->next = stmt;
                    last = stmt;
                }
            }
        }
        
        consume(parser, TOKEN_RBRACE, "Expected '}' after block");
        return ast_create_block(statements);
    }
    
    // Expression statement
    ast_node_t *expr = parse_expression(parser);
    consume(parser, TOKEN_SEMICOLON, "Expected ';' after expression");
    return expr;
}

// Program parsing
ast_node_t* parse_program(parser_t *parser) {
    ast_node_t *statements = NULL;
    ast_node_t *last = NULL;
    
    while (!check(parser, TOKEN_EOF)) {
        ast_node_t *stmt = NULL;
        
        // Parse declarations and statements
        if (check(parser, TOKEN_INT) || check(parser, TOKEN_CHAR)) {
            stmt = parse_declaration(parser);
        } else {
            stmt = parse_statement(parser);
        }
        
        if (stmt) {
            if (!statements) {
                statements = stmt;
                last = stmt;
            } else {
                last->next = stmt;
                last = stmt;
            }
        }
        
        if (parser->panic_mode) {
            // Skip to next statement on error
            while (!check(parser, TOKEN_SEMICOLON) && !check(parser, TOKEN_RBRACE) && !check(parser, TOKEN_EOF)) {
                advance(parser);
            }
            if (match(parser, TOKEN_SEMICOLON)) {
                parser->panic_mode = 0;
            }
        }
    }
    
    ast_node_t *program = malloc(sizeof(ast_node_t));
    if (program) {
        program->type = AST_PROGRAM;
        program->data.block.statements = statements;
        program->next = NULL;
    }
    
    return program;
}

static ast_node_t* parse_declaration(parser_t *parser) {
    if (match(parser, TOKEN_INT) || match(parser, TOKEN_CHAR)) {
        return parse_variable_declaration(parser);
    }
    
    return parse_statement(parser);
}

static ast_node_t* parse_variable_declaration(parser_t *parser) {
    consume(parser, TOKEN_IDENTIFIER, "Expected variable name");
    
    char name[256];
    int len = parser->previous_token.length;
    if (len >= 256) len = 255;
    strncpy(name, parser->previous_token.text, len);
    name[len] = '\0';
    
    // Check for array declaration: name[size]
    if (match(parser, TOKEN_LBRACKET)) {
        if (!check(parser, TOKEN_NUMBER)) {
            parser_error_at_current(parser, "Expected array size");
            return NULL;
        }
        
        int size = parser->current_token.value;
        advance(parser);
        consume(parser, TOKEN_RBRACKET, "Expected ']' after array size");
        
        ast_node_t *decl = ast_create_array_decl(name, size);
        consume(parser, TOKEN_SEMICOLON, "Expected ';' after array declaration");
        return decl;
    }
    
    // Regular variable declaration
    ast_node_t *decl = ast_create_variable_decl(name, 1);
    
    if (match(parser, TOKEN_ASSIGN)) {
        ast_node_t *value = parse_expression(parser);
        consume(parser, TOKEN_SEMICOLON, "Expected ';' after variable declaration");
        return ast_create_assignment(decl, value);
    }
    
    consume(parser, TOKEN_SEMICOLON, "Expected ';' after variable declaration");
    return decl;
}

// Utility functions
const char* ast_node_type_string(ast_node_type_t type) {
    switch (type) {
        case AST_PROGRAM: return "PROGRAM";
        case AST_FUNCTION: return "FUNCTION";
        case AST_VARIABLE_DECL: return "VARIABLE_DECL";
        case AST_ARRAY_DECL: return "ARRAY_DECL";
        case AST_BINARY_OP: return "BINARY_OP";
        case AST_UNARY_OP: return "UNARY_OP";
        case AST_ASSIGNMENT: return "ASSIGNMENT";
        case AST_FUNCTION_CALL: return "FUNCTION_CALL";
        case AST_IF_STMT: return "IF_STMT";
        case AST_WHILE_STMT: return "WHILE_STMT";
        case AST_FOR_STMT: return "FOR_STMT";
        case AST_RETURN_STMT: return "RETURN_STMT";
        case AST_BLOCK: return "BLOCK";
        case AST_IDENTIFIER: return "IDENTIFIER";
        case AST_NUMBER: return "NUMBER";
        case AST_ARRAY_ACCESS: return "ARRAY_ACCESS";
        default: return "UNKNOWN";
    }
}

void ast_print(ast_node_t *node, int depth) {
    if (!node) return;
    
    for (int i = 0; i < depth; i++) printf("  ");
    
    printf("%s", ast_node_type_string(node->type));
    
    switch (node->type) {
        case AST_NUMBER:
            printf(": %d", node->data.number.value);
            break;
        case AST_IDENTIFIER:
            printf(": %s", node->data.identifier.name);
            break;
        case AST_BINARY_OP:
            printf(": %s", token_type_string(node->data.binary.op));
            break;
        case AST_UNARY_OP:
            printf(": %s", token_type_string(node->data.unary.op));
            break;
        case AST_VARIABLE_DECL:
            printf(": %s", node->data.var_decl.name);
            break;
        case AST_ARRAY_DECL:
            printf(": %s[%d]", node->data.array_decl.name, node->data.array_decl.size);
            break;
        default:
            break;
    }
    
    printf("\n");
    
    // Print children
    switch (node->type) {
        case AST_BINARY_OP:
            ast_print(node->data.binary.left, depth + 1);
            ast_print(node->data.binary.right, depth + 1);
            break;
        case AST_UNARY_OP:
            ast_print(node->data.unary.operand, depth + 1);
            break;
        case AST_ASSIGNMENT:
            ast_print(node->data.assignment.target, depth + 1);
            ast_print(node->data.assignment.value, depth + 1);
            break;
        case AST_RETURN_STMT:
            if (node->data.return_stmt.expression) {
                ast_print(node->data.return_stmt.expression, depth + 1);
            }
            break;
        case AST_BLOCK:
        case AST_PROGRAM:
            ast_print(node->data.block.statements, depth + 1);
            break;
        default:
            break;
    }
    
    // Print next in list
    if (node->next) {
        ast_print(node->next, depth);
    }
}

// Error handling
void parser_error(parser_t *parser, const char *message) {
    parser_error_at_current(parser, message);
}

void parser_error_at_current(parser_t *parser, const char *message) {
    if (parser->panic_mode) return;
    parser->panic_mode = 1;
    
    fprintf(stderr, "Parse error at line %d: %s\n", 
            parser->current_token.line, message);
    
    if (parser->current_token.type == TOKEN_EOF) {
        fprintf(stderr, " at end of file\n");
    } else if (parser->current_token.type == TOKEN_ERROR) {
        // Nothing
    } else {
        fprintf(stderr, " at '%.*s'\n", 
                parser->current_token.length, parser->current_token.text);
    }
    
    parser->error_count++;
}

void parser_error_at_previous(parser_t *parser, const char *message) {
    if (parser->panic_mode) return;
    parser->panic_mode = 1;
    
    fprintf(stderr, "Parse error at line %d: %s at '%.*s'\n", 
            parser->previous_token.line, message,
            parser->previous_token.length, parser->previous_token.text);
    
    parser->error_count++;
}