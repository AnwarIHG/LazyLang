#pragma once

#ifndef _ERROR_H
#define _ERROR_H

#include "types.h"

typedef enum {
    ErrorType_LEXER_INVALID_CHAR,
    ErrorType_LEXER_INVALID_STRING,
} ErrorType;

typedef struct {
    const File* file;
    u32 pos;
    u32 line;
    char message[];
} ErrorContex;

typedef struct {
    ErrorType error_type;
    ErrorContex contex;
} Error;

#endif //_ERROR_H
