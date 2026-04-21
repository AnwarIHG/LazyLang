#pragma once


#ifndef LEXER_H

#include "Token.h"

// the max amount that allowed to be allocated for the file source
#define BUFFER_LIMIT 524288 // 0.5 MB

typedef struct {
    const char* filename;
    FILE* c_file;
    char* source;
    u64 left;
    u64 size;
} __attribute__((packed)) File;

typedef struct {
    File file;
    char* current;
    u32 line;
    u32 column;
} Lexer;

Lexer* Lexer_init(const char *file_name);
Token Lexer_next_token (Lexer* lex);
void Lexer_term(Lexer *lex);

#endif //LEXER_H
