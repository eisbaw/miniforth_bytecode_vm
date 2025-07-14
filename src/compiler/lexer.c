#include "lexer.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Keywords table
static const struct {
    const char *text;
    token_type_t type;
} keywords[] = {
    {"if", TOKEN_IF},
    {"else", TOKEN_ELSE},
    {"while", TOKEN_WHILE},
    {"for", TOKEN_FOR},
    {"do", TOKEN_DO},
    {"int", TOKEN_INT},
    {"char", TOKEN_CHAR},
    {"return", TOKEN_RETURN},
    {"break", TOKEN_BREAK},
    {"continue", TOKEN_CONTINUE},
    {"static", TOKEN_STATIC},
    {NULL, TOKEN_IDENTIFIER}  // End marker
};

// Utility functions
int is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

int is_digit(char c) {
    return c >= '0' && c <= '9';
}

int is_alnum(char c) {
    return is_alpha(c) || is_digit(c);
}

int is_keyword(const char *text, int length) {
    for (int i = 0; keywords[i].text != NULL; i++) {
        if (strlen(keywords[i].text) == length && 
            strncmp(keywords[i].text, text, length) == 0) {
            return i;
        }
    }
    return -1;  // Not a keyword
}

// Create lexer
lexer_t* lexer_create(const char *source) {
    lexer_t *lexer = malloc(sizeof(lexer_t));
    if (!lexer) return NULL;
    
    lexer->source = source;
    lexer->current = source;
    lexer->start = source;
    lexer->line = 1;
    lexer->column = 1;
    lexer->error_count = 0;
    
    return lexer;
}

// Destroy lexer
void lexer_destroy(lexer_t *lexer) {
    if (lexer) {
        free(lexer);
    }
}

// Advance to next character
static char advance(lexer_t *lexer) {
    if (*lexer->current == '\0') return '\0';
    
    char c = *lexer->current++;
    if (c == '\n') {
        lexer->line++;
        lexer->column = 1;
    } else {
        lexer->column++;
    }
    return c;
}

// Peek at current character without advancing
static char peek(lexer_t *lexer) {
    return *lexer->current;
}

// Peek at next character without advancing
static char peek_next(lexer_t *lexer) {
    if (*lexer->current == '\0') return '\0';
    return *(lexer->current + 1);
}

// Check if we're at end of source
static int is_at_end(lexer_t *lexer) {
    return *lexer->current == '\0';
}

// Skip whitespace
static void skip_whitespace(lexer_t *lexer) {
    while (!is_at_end(lexer)) {
        char c = peek(lexer);
        if (c == ' ' || c == '\t' || c == '\r') {
            advance(lexer);
        } else if (c == '\n') {
            advance(lexer);
        } else if (c == '/' && peek_next(lexer) == '/') {
            // Skip line comment
            while (peek(lexer) != '\n' && !is_at_end(lexer)) {
                advance(lexer);
            }
        } else if (c == '/' && peek_next(lexer) == '*') {
            // Skip block comment
            advance(lexer); // Skip '/'
            advance(lexer); // Skip '*'
            while (!is_at_end(lexer)) {
                if (peek(lexer) == '*' && peek_next(lexer) == '/') {
                    advance(lexer); // Skip '*'
                    advance(lexer); // Skip '/'
                    break;
                }
                advance(lexer);
            }
        } else {
            break;
        }
    }
}

// Create token
static token_t make_token(lexer_t *lexer, token_type_t type) {
    token_t token;
    token.type = type;
    token.text = (char*)lexer->start;
    token.length = (int)(lexer->current - lexer->start);
    token.line = lexer->line;
    token.column = lexer->column - token.length;
    token.value = 0;
    return token;
}

// Create error token
static token_t error_token(lexer_t *lexer, const char *message) {
    token_t token;
    token.type = TOKEN_ERROR;
    token.text = (char*)message;
    token.length = strlen(message);
    token.line = lexer->line;
    token.column = lexer->column;
    token.value = 0;
    lexer->error_count++;
    return token;
}

// Scan string literal
static token_t string_literal(lexer_t *lexer) {
    while (peek(lexer) != '"' && !is_at_end(lexer)) {
        if (peek(lexer) == '\n') lexer->line++;
        advance(lexer);
    }
    
    if (is_at_end(lexer)) {
        return error_token(lexer, "Unterminated string");
    }
    
    advance(lexer); // Closing quote
    return make_token(lexer, TOKEN_STRING);
}

// Scan number literal
static token_t number_literal(lexer_t *lexer) {
    while (is_digit(peek(lexer))) {
        advance(lexer);
    }
    
    token_t token = make_token(lexer, TOKEN_NUMBER);
    
    // Parse the numeric value
    int value = 0;
    for (int i = 0; i < token.length; i++) {
        value = value * 10 + (token.text[i] - '0');
    }
    token.value = value;
    
    return token;
}

// Scan identifier or keyword
static token_t identifier_or_keyword(lexer_t *lexer) {
    while (is_alnum(peek(lexer))) {
        advance(lexer);
    }
    
    int length = (int)(lexer->current - lexer->start);
    int keyword_index = is_keyword(lexer->start, length);
    
    if (keyword_index >= 0) {
        return make_token(lexer, keywords[keyword_index].type);
    } else {
        return make_token(lexer, TOKEN_IDENTIFIER);
    }
}

// Get next token
token_t lexer_next_token(lexer_t *lexer) {
    skip_whitespace(lexer);
    
    lexer->start = lexer->current;
    
    if (is_at_end(lexer)) {
        return make_token(lexer, TOKEN_EOF);
    }
    
    char c = advance(lexer);
    
    // Identifiers and keywords
    if (is_alpha(c)) {
        return identifier_or_keyword(lexer);
    }
    
    // Numbers
    if (is_digit(c)) {
        return number_literal(lexer);
    }
    
    // Single character tokens and multi-character operators
    switch (c) {
        case '(': return make_token(lexer, TOKEN_LPAREN);
        case ')': return make_token(lexer, TOKEN_RPAREN);
        case '{': return make_token(lexer, TOKEN_LBRACE);
        case '}': return make_token(lexer, TOKEN_RBRACE);
        case '[': return make_token(lexer, TOKEN_LBRACKET);
        case ']': return make_token(lexer, TOKEN_RBRACKET);
        case ';': return make_token(lexer, TOKEN_SEMICOLON);
        case ',': return make_token(lexer, TOKEN_COMMA);
        case '.': return make_token(lexer, TOKEN_DOT);
        case '%': return make_token(lexer, TOKEN_MODULO);
        
        case '+':
            if (peek(lexer) == '+') {
                advance(lexer);
                return make_token(lexer, TOKEN_INCREMENT);
            } else if (peek(lexer) == '=') {
                advance(lexer);
                return make_token(lexer, TOKEN_PLUS_ASSIGN);
            }
            return make_token(lexer, TOKEN_PLUS);
            
        case '-':
            if (peek(lexer) == '-') {
                advance(lexer);
                return make_token(lexer, TOKEN_DECREMENT);
            } else if (peek(lexer) == '=') {
                advance(lexer);
                return make_token(lexer, TOKEN_MINUS_ASSIGN);
            }
            return make_token(lexer, TOKEN_MINUS);
            
        case '*':
            if (peek(lexer) == '=') {
                advance(lexer);
                return make_token(lexer, TOKEN_MULTIPLY_ASSIGN);
            }
            return make_token(lexer, TOKEN_MULTIPLY);
            
        case '/':
            if (peek(lexer) == '=') {
                advance(lexer);
                return make_token(lexer, TOKEN_DIVIDE_ASSIGN);
            }
            return make_token(lexer, TOKEN_DIVIDE);
            
        case '=':
            if (peek(lexer) == '=') {
                advance(lexer);
                return make_token(lexer, TOKEN_EQUAL);
            }
            return make_token(lexer, TOKEN_ASSIGN);
            
        case '!':
            if (peek(lexer) == '=') {
                advance(lexer);
                return make_token(lexer, TOKEN_NOT_EQUAL);
            }
            return make_token(lexer, TOKEN_LOGICAL_NOT);
            
        case '<':
            if (peek(lexer) == '=') {
                advance(lexer);
                return make_token(lexer, TOKEN_LESS_EQUAL);
            }
            return make_token(lexer, TOKEN_LESS);
            
        case '>':
            if (peek(lexer) == '=') {
                advance(lexer);
                return make_token(lexer, TOKEN_GREATER_EQUAL);
            }
            return make_token(lexer, TOKEN_GREATER);
            
        case '&':
            if (peek(lexer) == '&') {
                advance(lexer);
                return make_token(lexer, TOKEN_LOGICAL_AND);
            }
            return make_token(lexer, TOKEN_AMPERSAND);
            
        case '|':
            if (peek(lexer) == '|') {
                advance(lexer);
                return make_token(lexer, TOKEN_LOGICAL_OR);
            }
            return error_token(lexer, "Unexpected character '|'");
            
        case '"':
            return string_literal(lexer);
            
        default:
            return error_token(lexer, "Unexpected character");
    }
}

// Peek at next token without consuming it
token_t lexer_peek_token(lexer_t *lexer) {
    // Save lexer state
    const char *saved_current = lexer->current;
    const char *saved_start = lexer->start;
    int saved_line = lexer->line;
    int saved_column = lexer->column;
    
    // Get next token
    token_t token = lexer_next_token(lexer);
    
    // Restore lexer state
    lexer->current = saved_current;
    lexer->start = saved_start;
    lexer->line = saved_line;
    lexer->column = saved_column;
    
    return token;
}

// Get string representation of token type
const char* token_type_string(token_type_t type) {
    switch (type) {
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_NUMBER: return "NUMBER";
        case TOKEN_STRING: return "STRING";
        case TOKEN_PLUS: return "PLUS";
        case TOKEN_MINUS: return "MINUS";
        case TOKEN_MULTIPLY: return "MULTIPLY";
        case TOKEN_DIVIDE: return "DIVIDE";
        case TOKEN_MODULO: return "MODULO";
        case TOKEN_EQUAL: return "EQUAL";
        case TOKEN_NOT_EQUAL: return "NOT_EQUAL";
        case TOKEN_LESS: return "LESS";
        case TOKEN_GREATER: return "GREATER";
        case TOKEN_LESS_EQUAL: return "LESS_EQUAL";
        case TOKEN_GREATER_EQUAL: return "GREATER_EQUAL";
        case TOKEN_LOGICAL_AND: return "LOGICAL_AND";
        case TOKEN_LOGICAL_OR: return "LOGICAL_OR";
        case TOKEN_LOGICAL_NOT: return "LOGICAL_NOT";
        case TOKEN_ASSIGN: return "ASSIGN";
        case TOKEN_PLUS_ASSIGN: return "PLUS_ASSIGN";
        case TOKEN_MINUS_ASSIGN: return "MINUS_ASSIGN";
        case TOKEN_MULTIPLY_ASSIGN: return "MULTIPLY_ASSIGN";
        case TOKEN_DIVIDE_ASSIGN: return "DIVIDE_ASSIGN";
        case TOKEN_INCREMENT: return "INCREMENT";
        case TOKEN_DECREMENT: return "DECREMENT";
        case TOKEN_SEMICOLON: return "SEMICOLON";
        case TOKEN_COMMA: return "COMMA";
        case TOKEN_DOT: return "DOT";
        case TOKEN_LPAREN: return "LPAREN";
        case TOKEN_RPAREN: return "RPAREN";
        case TOKEN_LBRACE: return "LBRACE";
        case TOKEN_RBRACE: return "RBRACE";
        case TOKEN_LBRACKET: return "LBRACKET";
        case TOKEN_RBRACKET: return "RBRACKET";
        case TOKEN_AMPERSAND: return "AMPERSAND";
        case TOKEN_ASTERISK: return "ASTERISK";
        case TOKEN_IF: return "IF";
        case TOKEN_ELSE: return "ELSE";
        case TOKEN_WHILE: return "WHILE";
        case TOKEN_FOR: return "FOR";
        case TOKEN_DO: return "DO";
        case TOKEN_INT: return "INT";
        case TOKEN_CHAR: return "CHAR";
        case TOKEN_RETURN: return "RETURN";
        case TOKEN_BREAK: return "BREAK";
        case TOKEN_CONTINUE: return "CONTINUE";
        case TOKEN_STATIC: return "STATIC";
        case TOKEN_EOF: return "EOF";
        case TOKEN_ERROR: return "ERROR";
        case TOKEN_NEWLINE: return "NEWLINE";
        default: return "UNKNOWN";
    }
}

// Report lexer error
void lexer_error(lexer_t *lexer, const char *message) {
    fprintf(stderr, "Lexer error at line %d, column %d: %s\n", 
            lexer->line, lexer->column, message);
    lexer->error_count++;
}