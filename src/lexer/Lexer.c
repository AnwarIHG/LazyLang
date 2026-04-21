#include "Lexer.h"
#include "Token.h"
#include "../common/macro.h"

#include <endian.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "keywords.h"

/* 
 * this function will open the file
 * */
static File File_create(const char* filepath) {

    File file = {0};
    file.filename = filepath;

    file.c_file = fopen(filepath, "rb");

    // get the file size
    fseek(file.c_file, 0, SEEK_END);
    file.size = ftell(file.c_file);
    file.left = file.size;

    rewind(file.c_file);
    return file;
}

static u64 File_read(File* file){

    if (!file->source) { 
        file->source = (char*)malloc(MIN(file->size, BUFFER_LIMIT) + 1);

        if (!file->source) {
            fclose(file->c_file);
            fprintf(stderr, "Error: Memory allocation failed\n");
            return 0;
        }
    }

    u64 bytes_read = 0;
    if (file->left > 0) {
        bytes_read = fread(file->source, 1, MIN(file->left, BUFFER_LIMIT), file->c_file);

        file->left -= bytes_read;
        file->source[bytes_read] = '\0';
    } else {
        return bytes_read;
    }

    return bytes_read;
}

/* free the allocated memroy for the file*/
static void File_destroy(File *file) {
    if (file->c_file) fclose(file->c_file);
    if (file->source) free(file->source);
}

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

    return (Token) {
        .type = Token_type_IDENTIFIER,
        .as.litreal_str_ = buf,
    };
}

static Token parse_number(Lexer* lex) {
    char c = 0;
    String buf = String_create_empty();

    while (1) {
        c = advance(lex); 

        if (c == '.' && isdigit(peek(lex))) {
            buf = String_cat_len(buf, ".",1);
            continue;
        }

        if (isdigit(c)) {
            buf = String_cat_len(buf, &c,1);
        } else {
            break;
        }

        read_next_part(lex);
    }

    return (Token) {
        .type = Token_type_NUMBER,
        .as.litreal_str_ = buf,
    };
}

static Token parse_string_litreal(Lexer* lex) {
    String buf = String_create_empty();
    char c = 0;

    while (1) {
        c = advance(lex); 

        if (isalpha(c)) {
            buf = String_cat_len(buf, &c,1);
        } else {
            break;
        }

        read_next_part(lex);
    }

    return (Token) {
        .type = Token_type_STRING_LITERAL,
        .as.litreal_str_ = buf,
    };
}

Token Lexer_next_token (Lexer* lex) {

    if (!lex) {
        return (Token) {
            .type = Token_type_EOF,
            .as.int_ = 0,
        };
    }

    skip_whitespace(lex);

    if (is_at_end(lex) && lex->file.left == 0) {
        return (Token) {
            .type = Token_type_EOF,
            .as.int_ = 0,
        };
    }

    // char c = *lex->current;

    if (isalpha(peek(lex))) {
        return parse_identifier(lex);
    } else if (isdigit(peek(lex))) {
        return parse_number(lex);
    }

    switch (peek(lex)) {
        case '.':
            advance(lex);
            return (Token) {
                .type = Token_type_DOT,
                .as.unknown_ = NULL,
            };

        case '|':
            advance(lex);
            return (Token) {
                .type = Token_type_PIPE,
                .as.unknown_ = NULL,
            };

        case '=':
            if (match(lex, '=')){
                return (Token) {
                    .type=Token_type_EQUAL_EQUAL,
                    .as.unknown_=NULL
                };
            } else if (match(lex,'>')) {
                return (Token) {
                    .type=Token_type_ARROW,
                    .as.unknown_=NULL
                };
            }

            return (Token) {
                .type=Token_type_EQUAL,
                .as.unknown_=NULL
            };

        case '+':
            advance(lex);
            return (Token) {
                .type=Token_type_PLUS,
                .as.unknown_=NULL
            };

        case '-':
            advance(lex);
            return (Token) {
                .type=Token_type_MINUS,
                .as.unknown_=NULL
            };
        default:
            printf("[ '%c' | %d ]\n",peek(lex),peek(lex));
            advance(lex);
            break;
    }

    return (Token) {
        .type = Token_type_ERROR,
        .as.int_ = 0,
    };
}
