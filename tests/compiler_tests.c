#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "lexer.h"

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

// Placeholder tests (will be implemented later)
void test_parser_basic() {
    printf("  Parser tests not yet implemented\n");
}

void test_codegen_basic() {
    printf("  Code generation tests not yet implemented\n");
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