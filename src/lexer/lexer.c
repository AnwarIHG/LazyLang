#include "lexer.h"
#include "token.h"
#include "../common/logger.h"

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

// #include "keywords.h"

Lexer* Lexer_init(const char *file_name) {
    Lexer *lex = (Lexer*)malloc(sizeof(Lexer));
    if (!lex) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return NULL;
    }

    lex->file = File_create(file_name);
    File_read(&lex->file);

    lex->current = lex->file.source;
    lex->line = 1;
    lex->column = 0;

    return lex;
}

void Lexer_term(Lexer *lex) {
    File_destroy(&lex->file);
    if (lex) free(lex);
}

// --- lexer utils ---
static inline char advance(Lexer* lexer) {
    lexer->current++;
    lexer->column++;
    return lexer->current[-1];
}

static inline char move(Lexer* lexer,i32 offset) {
    lexer->current += offset;
    lexer->column += offset;
    return lexer->current[-1];
}

static inline i32 is_at_end(Lexer* lexer) {
    return *lexer->current == '\0';
}

static void read_next_part(Lexer* lex) {
    if (is_at_end(lex) && lex->file.left > 0) {
        if (File_read(&lex->file) > 0) { 
            lex->current = lex->file.source;
        }
    }
}

static inline char peek(Lexer* lexer) {
    read_next_part(lexer);
    return *lexer->current;
}

static inline char peek_next(Lexer* lexer) {
    if (peek(lexer) == '\0') return '\0';
    return lexer->current[1];
}

static i32 match(Lexer* lexer, char expected) {
    if (is_at_end(lexer)) return 0;
    if (*lexer->current != expected) return 0;
    lexer->current++;
    lexer->column++;
    return 1;
}

static void skip_commint_single(Lexer* lexer){
    while (peek(lexer) != '\0' && peek(lexer) != '\n') {
        advance(lexer);
    }
}

static void skip_whitespace(Lexer* lexer) {
    while (1) {
        char c = peek(lexer);
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance(lexer);
                break;
            case '\n':
                lexer->line++;
                lexer->column = 0;
                advance(lexer);
                break;
            // Skip comment
            case '/':
                if (peek_next(lexer) == '/') {
                    move(lexer, 2);
                    skip_commint_single(lexer);
                } else {
                    return;
                }
                break;
            default:
                read_next_part(lexer);
                return;
        }
    }
}

Token static inline _make_token(TokenKind kind,TokenValue value){
    return (Token){
        .kind = kind,
        .as = value
    };
}

#define make_token(kind, value) (_make_token(kind, (TokenValue){value}))

// --- parse ---

static Token parse_identifier(Lexer* lex) {
    char c = 0;
    String buf = String_create_empty();

    while (1) {
        c = advance(lex); 
        if (isalpha(c)) {
            buf = String_cat_len(buf, &c,1);
        } else {
            break;
        }

        read_next_part(lex);
    }

    return make_token(TokenKind_IDENTIFIER, .litreal_str_ = buf);
}

static Token parse_number(Lexer* lex) {
    String buf = String_create_empty();
    // char has_dot = 0;

    char c = 0;
    c = advance(lex); 

    while (isdigit(c)) {
        buf = String_cat_len(buf, &c,1);

        if (c == '.' && isdigit(peek(lex))) {
            buf = String_cat_len(buf, ".",1);
            continue;
        }

        if (c == '.') {
            // has_dot = 1;

            while (isdigit(c) && c == '.') {  
                buf = String_cat_len(buf, ".",1);
                c = advance(lex);
            }
        }

        c = advance(lex); 
        read_next_part(lex);
    }

    return make_token(TokenKind_NUMBER, .litreal_str_ = buf);
}

// static Token parse_string_litreal(Lexer* lex) {
//     String buf = String_create_empty();
//     char c = 0;
//
//     while (isalpha(c)) {
//         c = advance(lex); 
//
//         buf = String_cat_len(buf, &c,1);
//
//         read_next_part(lex);
//     }
//
//     return make_token(TokenKind_STRING_LITERAL, buf);
// }

Token Lexer_next_token (Lexer* lex) {

    if (!lex) return make_token(TokenKind_ERROR, .int_ = 0);

    skip_whitespace(lex);

    if (is_at_end(lex) && lex->file.left == 0) {
        return make_token(TokenKind_EOF, .int_ = 0);
    }

    // char c = *lex->current;

    if (isalpha(peek(lex))) {
        return parse_identifier(lex);
    } else if (isdigit(peek(lex))) {
        return parse_number(lex);
    }

    switch (peek(lex)) {

        case '~':
            advance(lex);
            return make_token(TokenKind_TILDE, .int_ = 0);

        case ')':
            advance(lex);
            return make_token(TokenKind_RPAREN, .int_ = 0);

        case '(':
            advance(lex);
            return make_token(TokenKind_LPAREN, .int_ = 0);

        case '}':
            advance(lex);
            return make_token(TokenKind_RBRACE, .int_ = 0);

        case '{':
            advance(lex);
            return make_token(TokenKind_LBRACE, .int_ = 0);

        case ']':
            advance(lex);
            return make_token(TokenKind_RBRACKET, .int_ = 0);

        case '[':
            advance(lex);
            return make_token(TokenKind_LBRACKET, .int_ = 0);

        case ':':
            advance(lex);
            return make_token(TokenKind_COLON, .int_ = 0);

        case ';':
            advance(lex);
            return make_token(TokenKind_SEMICOLON, .int_ = 0);

        case ',':
            advance(lex);
            return make_token(TokenKind_COMMA, .int_ = 0);

        case '?':
            advance(lex);
            return make_token(TokenKind_QUESTION, .int_ = 0);

        case '&':
            if (match(lex, '=')) {
                return make_token(TokenKind_AMPERSAND_EQUAL, .int_ = 0);
            }

            advance(lex);
            return make_token(TokenKind_AMPERSAND, .int_ = 0);

        case '!':
            if (match(lex, '=')) {
                return make_token(TokenKind_BANG_EQUAL, .int_ = 0);
            }

            advance(lex);
            return make_token(TokenKind_BANG, .int_ = 0);

        case '^':
            if (match(lex, '=')) {
                return make_token(TokenKind_CARET_EQUAL, .int_ = 0);
            }

            advance(lex);
            return make_token(TokenKind_CARET, .int_ = 0);

        case '>':
            if (match(lex, '=')) {
                return make_token(TokenKind_GREATER_EQUAL, .int_ = 0);
            } else if (match(lex, '>')) {
                if (match(lex, '=')) {
                    return make_token(TokenKind_GREATER_GREATER_EQUAL, .int_ = 0);
                }

                return make_token(TokenKind_GREATER_GREATER, .int_ = 0);
            }

            advance(lex);
            return make_token(TokenKind_GREATER, .int_ = 0);


        case '<':
            if (match(lex, '=')) {
                return make_token(TokenKind_LESS_EQUAL, .int_ = 0);
            } else if (match(lex, '<')) {
                if (match(lex, '=')) {
                    return make_token(TokenKind_LESS_LESS_EQUAL, .int_ = 0);
                }

                return make_token(TokenKind_LESS_LESS, .int_ = 0);
            }

            advance(lex);
            return make_token(TokenKind_LESS, .int_ = 0);

        case '%':
            if (match(lex, '=')) {            
                return make_token(TokenKind_PERCENT_EQUAL, .int_ = 0);
            }

            advance(lex);
            return make_token(TokenKind_PERCENT, .int_ = 0);

        case '/':
            if (match(lex, '=')) {        
                return make_token(TokenKind_SLASH_EQUAL, .int_ = 0);
            }

            advance(lex);
            return make_token(TokenKind_SLASH, .int_ = 0);

        case '*':
            if (match(lex, '=')) {        
                return make_token(TokenKind_STAR_EQUAL, .int_ = 0);
            }

            advance(lex);
            return make_token(TokenKind_STAR, .int_ = 0);
        
        case '.':
            if (match(lex,'.')) {
                if (match(lex,'.')) {
                    return make_token(TokenKind_DOT_DOT_DOT, .int_ = 0);
                }

                return make_token(TokenKind_DOT_DOT, .int_ = 0);
            }

            advance(lex);
            return make_token(TokenKind_DOT, .int_ = 0);

        case '|':
            if (match(lex, '|')) {
                return make_token(TokenKind_PIPE_PIPE, .int_ = 0);
            } else if (match(lex, '=')) {
                return make_token(TokenKind_PIPE_EQUAL, .int_ = 0);
            }

            advance(lex);
            return make_token(TokenKind_PIPE, .int_ = 0);

        case '=':
            if (match(lex, '=')){
                return make_token(TokenKind_EQUAL_EQUAL, .int_ = 0);
            } else if (match(lex,'>')) {
                return make_token(TokenKind_ARROW, .int_ = 0);
            }

            advance(lex);
            return make_token(TokenKind_EQUAL, .int_ = 0);

        case '+':
            if (match(lex, '+')) {
                return make_token(TokenKind_PLUS_PLUS, .int_ = 0);
            } else if (match(lex, '=')) {
                return make_token(TokenKind_PIPE_EQUAL, .int_ = 0);
            }

            advance(lex);
            return make_token(TokenKind_PLUS, .int_ = 0);

        case '-':
            if (match(lex, '-')) {
                return make_token(TokenKind_MINUS_MINUS, .int_ = 0);
            } else if (match(lex, '=')) {
                return make_token(TokenKind_MINUS_EQUAL, .int_ = 0);
            }

            advance(lex);
            return make_token(TokenKind_MINUS, .int_ = 0);

        default:
            WARN("invalid char [ '%c' | %d ]",peek(lex),peek(lex));
            advance(lex);
            break;
    }

    return make_token(TokenKind_ERROR, .int_ = 0);
}

#undef make_token
