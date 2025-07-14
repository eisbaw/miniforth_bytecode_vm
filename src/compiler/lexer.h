#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stdint.h>

// Token types for C subset lexer
typedef enum {
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_PLUS, TOKEN_MINUS, TOKEN_MULTIPLY, TOKEN_DIVIDE, TOKEN_MODULO,
    TOKEN_EQUAL, TOKEN_NOT_EQUAL, TOKEN_LESS, TOKEN_GREATER,
    TOKEN_LESS_EQUAL, TOKEN_GREATER_EQUAL,
    TOKEN_LOGICAL_AND, TOKEN_LOGICAL_OR, TOKEN_LOGICAL_NOT,
    TOKEN_ASSIGN,
    TOKEN_PLUS_ASSIGN, TOKEN_MINUS_ASSIGN, TOKEN_MULTIPLY_ASSIGN, TOKEN_DIVIDE_ASSIGN,
    TOKEN_INCREMENT, TOKEN_DECREMENT,
    TOKEN_SEMICOLON, TOKEN_COMMA, TOKEN_DOT,
    TOKEN_LPAREN, TOKEN_RPAREN,
    TOKEN_LBRACE, TOKEN_RBRACE,
    TOKEN_LBRACKET, TOKEN_RBRACKET,
    TOKEN_AMPERSAND,  // Address-of operator
    TOKEN_ASTERISK,   // Dereference operator (same as multiply, context dependent)
    
    // Keywords
    TOKEN_IF, TOKEN_ELSE, TOKEN_WHILE, TOKEN_FOR, TOKEN_DO,
    TOKEN_INT, TOKEN_CHAR, TOKEN_RETURN, TOKEN_BREAK, TOKEN_CONTINUE,
    TOKEN_STATIC,
    
    // Special tokens
    TOKEN_EOF,
    TOKEN_ERROR,
    TOKEN_NEWLINE
} token_type_t;

// Token structure
typedef struct {
    token_type_t type;
    char *text;           // Token text (owned by lexer)
    int value;            // For numeric tokens
    int line;             // Line number
    int column;           // Column number
    int length;           // Token length
} token_t;

// Lexer state
typedef struct {
    const char *source;   // Source code
    const char *current;  // Current position
    const char *start;    // Start of current token
    int line;             // Current line
    int column;           // Current column
    int error_count;      // Number of errors
} lexer_t;

// Lexer functions
lexer_t* lexer_create(const char *source);
void lexer_destroy(lexer_t *lexer);
token_t lexer_next_token(lexer_t *lexer);
token_t lexer_peek_token(lexer_t *lexer);
const char* token_type_string(token_type_t type);
void lexer_error(lexer_t *lexer, const char *message);

// Utility functions
int is_keyword(const char *text, int length);
int is_alpha(char c);
int is_digit(char c);
int is_alnum(char c);

#endif // LEXER_H