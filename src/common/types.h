#pragma once

#ifndef _DEFINES_H
#define _DEFINES_H

#include <stdint.h>

#ifdef __CLANG_STDINT_H
// unsigned int type
typedef uint8_t      u8;
typedef uint16_t    u16;
typedef uint32_t    u32;
typedef uint64_t    u64;

// signed int type
typedef int8_t   i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

// Boolean types
typedef int8_t  b8;
typedef int32_t b32;
#else
// unsigned int type
typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;

// signed int type
typedef char      i8;
typedef short     i16;
typedef int       i32;
typedef long long i64;

// Boolean types
typedef char    b8;
typedef int     b32;
#endif

// Floating point types
typedef float  f32;
typedef double f64;


// Properly define static assertions
#if defined (__Clang__) || defined (__gcc__)
#define STATIC_ASSERT _Static_assert
#else
#include <assert.h>
#define STATIC_ASSERT static_assert
#endif

STATIC_ASSERT(sizeof(u8) == 1,  "Expected u8 to be 1 byte.");
STATIC_ASSERT(sizeof(u16) == 2, "Expected u8 to be 2 byte.");
STATIC_ASSERT(sizeof(u32) == 4, "Expected u8 to be 4 byte.");
STATIC_ASSERT(sizeof(u64) == 8, "Expected u8 to be 8 byte.");

STATIC_ASSERT(sizeof(i8) == 1,  "Expected u8 to be 1 byte.");
STATIC_ASSERT(sizeof(i16) == 2, "Expected u8 to be 2 byte.");
STATIC_ASSERT(sizeof(i32) == 4, "Expected u8 to be 4 byte.");
STATIC_ASSERT(sizeof(i64) == 8, "Expected u8 to be 8 byte.");

STATIC_ASSERT(sizeof(b8) == 1,  "Expected u8 to be 1 byte.");
STATIC_ASSERT(sizeof(b32) == 4, "Expected u8 to be 4 byte.");

STATIC_ASSERT(sizeof(f32) == 4, "Expected u8 to be 4 byte.");
STATIC_ASSERT(sizeof(f64) == 8, "Expected u8 to be 8 byte.");

#endif //_DEFINES_H
