#pragma once


#ifndef LEXER_H

#include "token.h"
#include "../common/file.h"

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
