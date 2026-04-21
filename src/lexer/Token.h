#pragma once

#include "../common/types.h"
#include "../common/string/string_lib.h"

#ifndef TOKEN_H

typedef enum {
    // Keywords
    Token_type_LET,
    Token_type_CONST,
    Token_type_MUT,
    Token_type_FUNC,
    Token_type_RET,
    Token_type_IF,
    Token_type_ELSE,
    Token_type_WHILE,
    Token_type_FOR,
    Token_type_MATCH,
    Token_type_STRUCT,
    Token_type_ENUM,
    Token_type_UNION,
    Token_type_IMPL,
    Token_type_TRAIT,
    Token_type_WHERE,
    Token_type_AS,
    Token_type_IN,
    Token_type_TRUE,
    Token_type_FALSE,
    Token_type_MOD,
    Token_type_IMPORT,
    Token_type_EXTERN,
    
    // Types
    Token_type_VOID,
    Token_type_BOOL,
    Token_type_I8, Token_type_I16, Token_type_I32, Token_type_I64,
    Token_type_U8, Token_type_U16, Token_type_U32, Token_type_U64,
    Token_type_F32, Token_type_F64,
    Token_type_CHAR,
    Token_type_STRING,
    Token_type_SELF,
    
    // Identifiers and Literals
    Token_type_IDENTIFIER,
    Token_type_NUMBER,
    Token_type_STRING_LITERAL,
    Token_type_CHAR_LITERAL,
    
    // Operators
    Token_type_PLUS,
    Token_type_MINUS,
    Token_type_STAR,
    Token_type_SLASH,
    Token_type_PERCENT,
    Token_type_AMPERSAND,
    Token_type_PIPE,
    Token_type_CARET,
    Token_type_TILDE,
    Token_type_EXCLAM,
    Token_type_EQUAL,
    Token_type_EQUAL_EQUAL,
    Token_type_BANG_EQUAL,
    Token_type_LESS,
    Token_type_LESS_EQUAL,
    Token_type_GREATER,
    Token_type_GREATER_EQUAL,
    Token_type_LESS_LESS,
    Token_type_GREATER_GREATER,
    Token_type_PLUS_EQUAL,
    Token_type_MINUS_EQUAL,
    Token_type_STAR_EQUAL,
    Token_type_SLASH_EQUAL,
    Token_type_PERCENT_EQUAL,
    Token_type_AMPERSAND_EQUAL,
    Token_type_PIPE_EQUAL,
    Token_type_CARET_EQUAL,
    Token_type_LESS_LESS_EQUAL,
    Token_type_GREATER_GREATER_EQUAL,
    Token_type_PLUS_PLUS,
    Token_type_MINUS_MINUS,
    Token_type_ARROW,
    Token_type_DOUBLE_ARROW,
    Token_type_VOID_RETURN,
    Token_type_DOT_DOT,
    Token_type_DOT_DOT_DOT,
    
    // Punctuation
    Token_type_LPAREN,
    Token_type_RPAREN,
    Token_type_LBRACE,
    Token_type_RBRACE,
    Token_type_LBRACKET,
    Token_type_RBRACKET,
    Token_type_COLON,
    Token_type_SEMICOLON,
    Token_type_COMMA,
    Token_type_DOT,
    Token_type_QUESTION,
    
    // Special
    Token_type_EOF,
    Token_type_ERROR,
} Token_type;

typedef union {
    void* unknown_;
    String litreal_str_;
    i64 int_;
    f64 float_;
} Token_value; 

typedef struct {
    Token_value as;
    Token_type type;
} Token;

#endif // TOKEN_H
