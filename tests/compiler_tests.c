#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "codegen.h"
#include "../src/vm/miniforth.h"

// Test result tracking
static int tests_run = 0;
static int tests_passed = 0;

#define TEST(name) \
    do { \
        printf("Running test: %s\n", #name); \
        tests_run++; \
        name(); \
        tests_passed++; \
        printf("  PASSED\n"); \
    } while (0)

// Test 1: Basic tokens
void test_lexer_basic_tokens() {
    const char *source = "int x = 42;";
    lexer_t *lexer = lexer_create(source);
    assert(lexer != NULL);
    
    token_t token;
    
    // int
    token = lexer_next_token(lexer);
    assert(token.type == TOKEN_INT);
    
    // x
    token = lexer_next_token(lexer);
    assert(token.type == TOKEN_IDENTIFIER);
    assert(token.length == 1);
    assert(strncmp(token.text, "x", 1) == 0);
    
    // =
    token = lexer_next_token(lexer);
    assert(token.type == TOKEN_ASSIGN);
    
    // 42
    token = lexer_next_token(lexer);
    assert(token.type == TOKEN_NUMBER);
    assert(token.value == 42);
    
    // ;
    token = lexer_next_token(lexer);
    assert(token.type == TOKEN_SEMICOLON);
    
    // EOF
    token = lexer_next_token(lexer);
    assert(token.type == TOKEN_EOF);
    
    lexer_destroy(lexer);
}

// Test 2: Operators
void test_lexer_operators() {
    const char *source = "+ - * / % == != < > <= >= && || !";
    lexer_t *lexer = lexer_create(source);
    assert(lexer != NULL);
    
    token_type_t expected[] = {
        TOKEN_PLUS, TOKEN_MINUS, TOKEN_MULTIPLY, TOKEN_DIVIDE, TOKEN_MODULO,
        TOKEN_EQUAL, TOKEN_NOT_EQUAL, TOKEN_LESS, TOKEN_GREATER,
        TOKEN_LESS_EQUAL, TOKEN_GREATER_EQUAL, TOKEN_LOGICAL_AND,
        TOKEN_LOGICAL_OR, TOKEN_LOGICAL_NOT, TOKEN_EOF
    };
    
    for (int i = 0; expected[i] != TOKEN_EOF; i++) {
        token_t token = lexer_next_token(lexer);
        assert(token.type == expected[i]);
    }
    
    // Check EOF
    token_t token = lexer_next_token(lexer);
    assert(token.type == TOKEN_EOF);
    
    lexer_destroy(lexer);
}

// Test 3: Keywords
void test_lexer_keywords() {
    const char *source = "if else while for do int char return break continue static";
    lexer_t *lexer = lexer_create(source);
    assert(lexer != NULL);
    
    token_type_t expected[] = {
        TOKEN_IF, TOKEN_ELSE, TOKEN_WHILE, TOKEN_FOR, TOKEN_DO,
        TOKEN_INT, TOKEN_CHAR, TOKEN_RETURN, TOKEN_BREAK, 
        TOKEN_CONTINUE, TOKEN_STATIC, TOKEN_EOF
    };
    
    for (int i = 0; expected[i] != TOKEN_EOF; i++) {
        token_t token = lexer_next_token(lexer);
        assert(token.type == expected[i]);
    }
    
    // Check EOF
    token_t token = lexer_next_token(lexer);
    assert(token.type == TOKEN_EOF);
    
    lexer_destroy(lexer);
}

// Test 4: Numbers and identifiers
void test_lexer_numbers_identifiers() {
    const char *source = "variable123 42 _underscore 0 999";
    lexer_t *lexer = lexer_create(source);
    assert(lexer != NULL);
    
    // variable123
    token_t token = lexer_next_token(lexer);
    assert(token.type == TOKEN_IDENTIFIER);
    assert(strncmp(token.text, "variable123", 11) == 0);
    
    // 42
    token = lexer_next_token(lexer);
    assert(token.type == TOKEN_NUMBER);
    assert(token.value == 42);
    
    // _underscore
    token = lexer_next_token(lexer);
    assert(token.type == TOKEN_IDENTIFIER);
    assert(strncmp(token.text, "_underscore", 11) == 0);
    
    // 0
    token = lexer_next_token(lexer);
    assert(token.type == TOKEN_NUMBER);
    assert(token.value == 0);
    
    // 999
    token = lexer_next_token(lexer);
    assert(token.type == TOKEN_NUMBER);
    assert(token.value == 999);
    
    lexer_destroy(lexer);
}

// Test 5: Complex expression
void test_lexer_expression() {
    const char *source = "result = (a + b) * 2;";
    lexer_t *lexer = lexer_create(source);
    assert(lexer != NULL);
    
    token_type_t expected[] = {
        TOKEN_IDENTIFIER,  // result
        TOKEN_ASSIGN,      // =
        TOKEN_LPAREN,      // (
        TOKEN_IDENTIFIER,  // a
        TOKEN_PLUS,        // +
        TOKEN_IDENTIFIER,  // b
        TOKEN_RPAREN,      // )
        TOKEN_MULTIPLY,    // *
        TOKEN_NUMBER,      // 2
        TOKEN_SEMICOLON,   // ;
        TOKEN_EOF
    };
    
    for (int i = 0; expected[i] != TOKEN_EOF; i++) {
        token_t token = lexer_next_token(lexer);
        assert(token.type == expected[i]);
    }
    
    lexer_destroy(lexer);
}

// Test 6: Comments
void test_lexer_comments() {
    const char *source = 
        "int x; // Line comment\n"
        "/* Block comment */ int y;";
    lexer_t *lexer = lexer_create(source);
    assert(lexer != NULL);
    
    token_type_t expected[] = {
        TOKEN_INT,         // int
        TOKEN_IDENTIFIER,  // x
        TOKEN_SEMICOLON,   // ;
        TOKEN_INT,         // int
        TOKEN_IDENTIFIER,  // y
        TOKEN_SEMICOLON,   // ;
        TOKEN_EOF
    };
    
    for (int i = 0; expected[i] != TOKEN_EOF; i++) {
        token_t token = lexer_next_token(lexer);
        assert(token.type == expected[i]);
    }
    
    lexer_destroy(lexer);
}

// Parser tests
void test_parser_expressions() {
    // Test 1: Simple arithmetic expression
    const char *source = "2 + 3 * 4";
    lexer_t *lexer = lexer_create(source);
    parser_t *parser = parser_create(lexer);
    
    ast_node_t *expr = parse_expression(parser);
    assert(expr != NULL);
    assert(expr->type == AST_BINARY_OP);
    assert(expr->data.binary.op == TOKEN_PLUS);
    
    // Left side should be number 2
    assert(expr->data.binary.left->type == AST_NUMBER);
    assert(expr->data.binary.left->data.number.value == 2);
    
    // Right side should be multiplication
    assert(expr->data.binary.right->type == AST_BINARY_OP);
    assert(expr->data.binary.right->data.binary.op == TOKEN_MULTIPLY);
    
    ast_destroy(expr);
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    // Test 2: Parenthesized expression
    source = "(2 + 3) * 4";
    lexer = lexer_create(source);
    parser = parser_create(lexer);
    
    expr = parse_expression(parser);
    assert(expr != NULL);
    assert(expr->type == AST_BINARY_OP);
    assert(expr->data.binary.op == TOKEN_MULTIPLY);
    
    // Left side should be addition
    assert(expr->data.binary.left->type == AST_BINARY_OP);
    assert(expr->data.binary.left->data.binary.op == TOKEN_PLUS);
    
    // Right side should be number 4
    assert(expr->data.binary.right->type == AST_NUMBER);
    assert(expr->data.binary.right->data.number.value == 4);
    
    ast_destroy(expr);
    parser_destroy(parser);
    lexer_destroy(lexer);
}

void test_parser_variables() {
    // Test variable declaration using parse_program
    const char *source = "int x;";
    lexer_t *lexer = lexer_create(source);
    parser_t *parser = parser_create(lexer);
    
    ast_node_t *program = parse_program(parser);
    assert(program != NULL);
    assert(program->type == AST_PROGRAM);
    
    ast_node_t *stmt = program->data.block.statements;
    assert(stmt != NULL);
    assert(stmt->type == AST_VARIABLE_DECL);
    assert(strcmp(stmt->data.var_decl.name, "x") == 0);
    
    ast_destroy(program);
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    // Test variable declaration with assignment
    source = "int y = 42;";
    lexer = lexer_create(source);
    parser = parser_create(lexer);
    
    program = parse_program(parser);
    assert(program != NULL);
    assert(program->type == AST_PROGRAM);
    
    stmt = program->data.block.statements;
    assert(stmt != NULL);
    assert(stmt->type == AST_ASSIGNMENT);
    
    // Target should be variable declaration
    assert(stmt->data.assignment.target->type == AST_VARIABLE_DECL);
    assert(strcmp(stmt->data.assignment.target->data.var_decl.name, "y") == 0);
    
    // Value should be number 42
    assert(stmt->data.assignment.value->type == AST_NUMBER);
    assert(stmt->data.assignment.value->data.number.value == 42);
    
    ast_destroy(program);
    parser_destroy(parser);
    lexer_destroy(lexer);
}

void test_parser_statements() {
    // Test return statement
    const char *source = "return 42;";
    lexer_t *lexer = lexer_create(source);
    parser_t *parser = parser_create(lexer);
    
    ast_node_t *stmt = parse_statement(parser);
    assert(stmt != NULL);
    assert(stmt->type == AST_RETURN_STMT);
    assert(stmt->data.return_stmt.expression != NULL);
    assert(stmt->data.return_stmt.expression->type == AST_NUMBER);
    assert(stmt->data.return_stmt.expression->data.number.value == 42);
    
    ast_destroy(stmt);
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    // Test block statement
    source = "{ x = 5; y = 10; }";
    lexer = lexer_create(source);
    parser = parser_create(lexer);
    
    stmt = parse_statement(parser);
    assert(stmt != NULL);
    assert(stmt->type == AST_BLOCK);
    assert(stmt->data.block.statements != NULL);
    
    // First statement should be assignment
    ast_node_t *first = stmt->data.block.statements;
    assert(first->type == AST_BINARY_OP);
    assert(first->data.binary.op == TOKEN_ASSIGN);
    
    // Second statement should be linked
    ast_node_t *second = first->next;
    assert(second != NULL);
    assert(second->type == AST_BINARY_OP);
    assert(second->data.binary.op == TOKEN_ASSIGN);
    
    ast_destroy(stmt);
    parser_destroy(parser);
    lexer_destroy(lexer);
}

void test_parser_basic() {
    test_parser_expressions();
    test_parser_variables();
    test_parser_statements();
}

void test_codegen_expressions() {
    // Test 1: Simple number literal
    const char *source = "42";
    lexer_t *lexer = lexer_create(source);
    parser_t *parser = parser_create(lexer);
    
    ast_node_t *expr = parse_expression(parser);
    assert(expr != NULL);
    
    codegen_t *codegen = codegen_create(64);
    assert(codegen != NULL);
    
    int result = codegen_expression(codegen, expr);
    assert(result == 0);
    assert(codegen->code_size == 2);  // LIT + operand
    assert(codegen->code[0] == OP_LIT);
    assert(codegen->code[1] == 42);
    
    ast_destroy(expr);
    codegen_destroy(codegen);
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    // Test 2: Simple addition
    source = "2 + 3";
    lexer = lexer_create(source);
    parser = parser_create(lexer);
    
    expr = parse_expression(parser);
    assert(expr != NULL);
    
    codegen = codegen_create(64);
    assert(codegen != NULL);
    
    result = codegen_expression(codegen, expr);
    assert(result == 0);
    assert(codegen->code_size == 5);  // LIT 2, LIT 3, ADD
    assert(codegen->code[0] == OP_LIT);
    assert(codegen->code[1] == 2);
    assert(codegen->code[2] == OP_LIT);
    assert(codegen->code[3] == 3);
    assert(codegen->code[4] == OP_ADD);
    
    ast_destroy(expr);
    codegen_destroy(codegen);
    parser_destroy(parser);
    lexer_destroy(lexer);
    
    // Test 3: Complex expression with precedence
    source = "2 + 3 * 4";
    lexer = lexer_create(source);
    parser = parser_create(lexer);
    
    expr = parse_expression(parser);
    assert(expr != NULL);
    
    codegen = codegen_create(64);
    assert(codegen != NULL);
    
    result = codegen_expression(codegen, expr);
    assert(result == 0);
    assert(codegen->code_size == 8);  // LIT 2, LIT 3, LIT 4, MUL, ADD
    assert(codegen->code[0] == OP_LIT);
    assert(codegen->code[1] == 2);
    assert(codegen->code[2] == OP_LIT);
    assert(codegen->code[3] == 3);
    assert(codegen->code[4] == OP_LIT);
    assert(codegen->code[5] == 4);
    assert(codegen->code[6] == OP_MUL);
    assert(codegen->code[7] == OP_ADD);
    
    ast_destroy(expr);
    codegen_destroy(codegen);
    parser_destroy(parser);
    lexer_destroy(lexer);
}

void test_codegen_statements() {
    // Test return statement
    const char *source = "return 42;";
    lexer_t *lexer = lexer_create(source);
    parser_t *parser = parser_create(lexer);
    
    ast_node_t *stmt = parse_statement(parser);
    assert(stmt != NULL);
    
    codegen_t *codegen = codegen_create(64);
    assert(codegen != NULL);
    
    int result = codegen_statement(codegen, stmt);
    assert(result == 0);
    assert(codegen->code_size == 3);  // LIT 42, EXIT
    assert(codegen->code[0] == OP_LIT);
    assert(codegen->code[1] == 42);
    assert(codegen->code[2] == OP_EXIT);
    
    ast_destroy(stmt);
    codegen_destroy(codegen);
    parser_destroy(parser);
    lexer_destroy(lexer);
}

void test_codegen_programs() {
    // Test simple program
    const char *source = "return 2 + 3;";
    lexer_t *lexer = lexer_create(source);
    parser_t *parser = parser_create(lexer);
    
    ast_node_t *program = parse_program(parser);
    assert(program != NULL);
    
    codegen_t *codegen = codegen_create(64);
    assert(codegen != NULL);
    
    int result = codegen_compile_ast(codegen, program);
    assert(result == 0);
    assert(codegen->code_size == 6);  // LIT 2, LIT 3, ADD, EXIT
    assert(codegen->code[0] == OP_LIT);
    assert(codegen->code[1] == 2);
    assert(codegen->code[2] == OP_LIT);
    assert(codegen->code[3] == 3);
    assert(codegen->code[4] == OP_ADD);
    assert(codegen->code[5] == OP_EXIT);
    
    ast_destroy(program);
    codegen_destroy(codegen);
    parser_destroy(parser);
    lexer_destroy(lexer);
}

void test_codegen_integration() {
    printf("  Testing end-to-end compilation and execution...\n");
    
    // Test a complete C program that calculates a mathematical expression
    const char *source = "return (5 + 3) * (10 - 6) / 2;";
    
    // Step 1: Lex
    lexer_t *lexer = lexer_create(source);
    assert(lexer != NULL);
    
    // Step 2: Parse
    parser_t *parser = parser_create(lexer);
    assert(parser != NULL);
    ast_node_t *program = parse_program(parser);
    assert(program != NULL);
    assert(parser->error_count == 0);
    
    // Step 3: Generate code
    codegen_t *codegen = codegen_create(128);
    assert(codegen != NULL);
    int result = codegen_compile_ast(codegen, program);
    assert(result == 0);
    assert(codegen->error_count == 0);
    
    // Step 4: Create VM and execute
    miniforth_vm_t *vm = vm_create(128, 64, 32);
    assert(vm != NULL);
    
    result = vm_load_program(vm, codegen->code, codegen->code_size);
    assert(result == VM_OK);
    
    vm_error_t exec_result = vm_run(vm);
    assert(exec_result == VM_OK);
    
    // Verify the result: (5 + 3) * (10 - 6) / 2 = 8 * 4 / 2 = 32 / 2 = 16
    assert(vm->sp > 0);
    assert(vm->data_stack[vm->sp - 1] == 16);
    
    // Cleanup
    vm_destroy(vm);
    codegen_destroy(codegen);
    ast_destroy(program);
    parser_destroy(parser);
    lexer_destroy(lexer);
}

void test_codegen_basic() {
    test_codegen_expressions();
    test_codegen_statements();
    test_codegen_programs();
    test_codegen_integration();
}

int main() {
    printf("Running Miniforth Compiler Tests\n");
    printf("=================================\n");
    
    TEST(test_lexer_basic_tokens);
    TEST(test_lexer_operators);
    TEST(test_lexer_keywords);
    TEST(test_lexer_numbers_identifiers);
    TEST(test_lexer_expression);
    TEST(test_lexer_comments);
    TEST(test_parser_basic);
    TEST(test_codegen_basic);
    
    printf("\nTest Results:\n");
    printf("Tests run: %d\n", tests_run);
    printf("Tests passed: %d\n", tests_passed);
    
    if (tests_passed == tests_run) {
        printf("ALL TESTS PASSED!\n");
        return 0;
    } else {
        printf("SOME TESTS FAILED!\n");
        return 1;
    }
}