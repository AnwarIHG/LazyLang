#pragma once

#include "../types.h"
#include "../macro.h"

#include <stdarg.h>
#include <malloc.h>
#include <stdlib.h>

#ifndef _STRING_LIB_H
#define _STRING_LIB_H

// #define String_malloc malloc;
// #define String_realloc realloc;
// #define String_free free;

void *String_malloc(u64 size);
void *String_realloc(void* ptr, u64 size);
void String_free(void* ptr);

#define STRING_LIB_MAX_PREALLOC (1024*1024)
// extern const char *SDS_NOINIT;

#define STRING_LIB_T_5      0
#define STRING_LIB_T_8      1
#define STRING_LIB_T_16     2
#define STRING_LIB_T_32     3
#define STRING_LIB_T_64     4
#define STRING_LIB_T_MASK   7
#define STRING_LIB_T_BITS   3

// #define STRING_LIB_HDR_VAR(T,s) struct String_##T##_t *sh = (void*)((s)-(sizeof(struct String_##T##_t)));

#define STRING_LIB_HDR_VAR(T,s) struct String_##T##_t *sh = &((struct String_##T##_t *)(s))[-1]
#define STRING_LIB_HDR(T,s) ((struct String_##T##_t *)((s)-(sizeof(struct String_##T##_t))))
#define STRING_LIB_TYPE_5_LEN(f) ((f)>>STRING_LIB_T_BITS)

typedef char* String;
typedef void* String_header;
typedef u8    String_flags;

typedef struct PACKED  String_5_t {
    String_flags flags;
    char buf[];
} String_5_t;

typedef struct PACKED  String_8_t {
    u8 len;
    u8 cap;
    String_flags flags;
    char buf[];
} String_8_t;


typedef struct PACKED  String_16_t {
    u16 len;
    u16 cap;
    String_flags flags;
    char buf[];
} String_16_t;


typedef struct PACKED  String_32_t {
    u32 len;
    u32 cap;
    String_flags flags;
    char buf[];
} String_32_t;


typedef struct PACKED  String_64_t {
    u64 len;
    u64 cap;
    String_flags flags;
    char buf[];
} String_64_t;

extern const char *STRING_LIB_NOINIT;

String String_create_len(const void *init, u64 initlen);
String String_create(const char *init);

String String_dup(const String s);
String String_create_empty(void);

u64 String_len(const String str);
void   String_distroy(String str);

String String_growzero(String s, u64 len);
String String_cat_len(String s, const void *t, u64 len);
String String_cat(String s, const char *t);
String String_cat_String(String s, const String t);
String String_cpy_len(String s, const char *t, u64 len);
String String_cpy(String s, const char *t);

String String_cat_vprintf(String s, const char *fmt, va_list ap);
#ifdef __GNUC__
String String_cat_printf(String s, const char *fmt, ...)
    __attribute__((format(printf, 2, 3)));
#else
String String_catprintf(String s, const char *fmt, ...);
#endif

String String_cat_fmt(String s, char const *fmt, ...);
String String_trim(String s, const char *cset);
void String_range(String s, i64 start, i64 end);
void String_update_len(String s);
void String_clear(String s);
i32 String_cmp(const String s1, const String s2);
String *String_split_len(const char *s, i64 len, const char *sep, i32 seplen, i32 *count);
void String_splitres_distroy(String *tokens, i32 count);
void String_tolower(String s);
void String_toupper(String s);
String String_from_long_long(long long value);
String String_cat_repr(String s, const char *p, u64 len);
String *String_split_args(const char *line, i32 *argc);
String String_map_chars(String s, const char *from, const char *to, u64 setlen);
String String_join(char **argv, i32 argc, char *sep);
String String_join_String(String *argv, i32 argc, const char *sep, u64 seplen);

/* Low level functions exposed to the user API */
String String_MakeRoomFor(String s, u64 addlen);
void String_IncrLen(String s, i64 incr);
String String_RemoveFreeSpace(String s);
u64 String_AllocSize(String s);
void *String_AllocPtr(String s);

/* Export the allocator used by SDS to the program using SDS.
 * Sometimes the program SDS is linked to, may use a different set of
 * allocators, but may want to allocate or free things that SDS will
 * respectively free or allocate. */

#ifdef REDIS_TEST
i32 sdsTest(i32 argc, char *argv[]);
#endif

#endif
