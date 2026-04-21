#pragma once

#include <stdint.h>
#include <stddef.h>

#ifndef HASH_UTILS_H
#define HASH_UTILS_H

typedef uint64_t (*hash_func_t)(const void* key, size_t len, uint64_t seed);

typedef struct {
    uint64_t seed;
    hash_func_t func;
} Hash_state;

Hash_state hash_init(uint64_t seed, hash_func_t func);
uint64_t hash_compute(Hash_state* state, const void* key, size_t len);

uint64_t hash_djb2(const void* key, size_t len, uint64_t seed);
uint64_t hash_fnv1a(const void* key, size_t len, uint64_t seed);
uint64_t hash_murmur3_32(const void* key, size_t len, uint64_t seed);
uint64_t hash_murmur3_64(const void* key, size_t len, uint64_t seed);
uint64_t hash_siphash24(const void* key, size_t len, uint64_t seed);

uint64_t hash_integer(uint64_t key, uint64_t seed);
uint64_t hash_string(const char* str, size_t len);

#endif // HASH_UTILS_H
