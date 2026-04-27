#pragma once

#include "../common/types.h"
#include "../common/string/string_lib.h"

#ifndef TOKEN_H

typedef enum {
    // Keywords
    TokenKind_LET,
    TokenKind_CONST,
    TokenKind_MUT,
    TokenKind_FUNC,
    TokenKind_RET,
    TokenKind_IF,
    TokenKind_ELSE,
    TokenKind_WHILE,
    TokenKind_FOR,
    TokenKind_MATCH,
    TokenKind_STRUCT,
    TokenKind_ENUM,
    TokenKind_UNION,
    TokenKind_IMPL,
    TokenKind_TRAIT,
    TokenKind_WHERE,
    TokenKind_AS,
    TokenKind_IN,
    TokenKind_TRUE,
    TokenKind_FALSE,
    TokenKind_MOD,
    TokenKind_IMPORT,
    TokenKind_EXTERN,
    
    // Types
    // TokenKind_VOID,
    // TokenKind_BOOL,
    // TokenKind_I8, TokenKind_I16, TokenKind_I32, TokenKind_I64,
    // TokenKind_U8, TokenKind_U16, TokenKind_U32, TokenKind_U64,
    // TokenKind_F32, TokenKind_F64,
    // TokenKind_CHAR,
    // TokenKind_STRING,
    // TokenKind_SELF,
    
    // Identifiers and Literals
    TokenKind_IDENTIFIER,
    TokenKind_NUMBER,
    TokenKind_STRING_LITERAL,
    TokenKind_CHAR_LITERAL,
    
    // Operators
    TokenKind_AMPERSAND,
    TokenKind_AMPERSAND_EQUAL,
    TokenKind_ARROW,
    TokenKind_BANG,
    TokenKind_BANG_EQUAL,
    TokenKind_CARET,
    TokenKind_CARET_EQUAL,
    TokenKind_DOT_DOT,
    TokenKind_DOT_DOT_DOT,
    TokenKind_DOUBLE_ARROW,
    TokenKind_EQUAL,
    TokenKind_EQUAL_EQUAL,
    TokenKind_GREATER,
    TokenKind_GREATER_EQUAL,
    TokenKind_GREATER_GREATER,
    TokenKind_GREATER_GREATER_EQUAL,
    TokenKind_LESS,
    TokenKind_LESS_EQUAL,
    TokenKind_LESS_LESS,
    TokenKind_LESS_LESS_EQUAL,
    TokenKind_MINUS,
    TokenKind_MINUS_EQUAL,
    TokenKind_MINUS_MINUS,
    TokenKind_PERCENT,
    TokenKind_PERCENT_EQUAL,
    TokenKind_PIPE,
    TokenKind_PIPE_EQUAL,
    TokenKind_PIPE_PIPE,
    TokenKind_PLUS,
    TokenKind_PLUS_EQUAL,
    TokenKind_PLUS_PLUS,
    TokenKind_SLASH,
    TokenKind_SLASH_EQUAL,
    TokenKind_STAR,
    TokenKind_STAR_EQUAL,
    TokenKind_TILDE,
    TokenKind_VOID_RETURN,
    
    // Punctuation
    TokenKind_LPAREN,
    TokenKind_RPAREN,
    TokenKind_LBRACE,
    TokenKind_RBRACE,
    TokenKind_LBRACKET,
    TokenKind_RBRACKET,
    TokenKind_COLON,
    TokenKind_SEMICOLON,
    TokenKind_COMMA,
    TokenKind_DOT,
    TokenKind_QUESTION,
    
    // Special
    TokenKind_EOF,
    TokenKind_ERROR,
} TokenKind;

typedef union {
    void* unknown_;
    String litreal_str_;
    i64 int_;
    f64 float_;
} TokenValue; 

typedef struct {
    TokenValue as;
    TokenKind kind;
} Token;

#endif // TOKEN_H
