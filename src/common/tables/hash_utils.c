#include "hash_utils.h"

#include <stdint.h>
#include <string.h>

static uint64_t rotl64(uint64_t x, uint32_t r) {
    return (x << r) | (x >> (64 - r));
}

static uint32_t fmix32(uint32_t k) {
    k ^= k >> 16;
    k *= 0x85ebca6b;
    k ^= k >> 13;
    k *= 0xc2b2ae35;
    k ^= k >> 16;
    return k;
}

static uint64_t fmix64(uint64_t k) {
    k ^= k >> 33;
    k *= 0xff51afd7ed558ccdULL;
    k ^= k >> 33;
    k *= 0xc4ceb9fe1a85ec53ULL;
    k ^= k >> 33;
    return k;
}

uint64_t hash_string(const char* str, size_t len) {
    uint64_t hash = 14695981039346656037ULL;
    for (size_t i = 0; i < len; i++) {
        hash ^= (uint64_t)(unsigned char)str[i];
        hash *= 1099511628211ULL;
    }
    return hash;
}

Hash_state hash_init(uint64_t seed, hash_func_t func) {
    Hash_state state;
    state.seed = seed;
    state.func = func;
    return state;
}

uint64_t hash_compute(Hash_state* state, const void* key, size_t len) {
    if (!state || !state->func) return 0;
    return state->func(key, len, state->seed);
}

uint64_t hash_djb2(const void* key, size_t len, uint64_t seed) {
    const uint8_t* data = (const uint8_t*)key;
    uint64_t hash = 5381 + seed;
    
    for (size_t i = 0; i < len; i++) {
        hash = ((hash << 5) + hash) + data[i];
    }
    
    return hash;
}

uint64_t hash_fnv1a(const void* key, size_t len, uint64_t seed) {
    const uint8_t* data = (const uint8_t*)key;
    uint64_t hash = 14695981039346656037ULL ^ seed;
    
    for (size_t i = 0; i < len; i++) {
        hash ^= data[i];
        hash *= 1099511628211ULL;
    }
    
    return hash;
}

uint64_t hash_murmur3_32(const void* key, size_t len, uint64_t seed) {
    const uint8_t* data = (const uint8_t*)key;
    const uint32_t c1 = 0xcc9e2d51;
    const uint32_t c2 = 0x1b873593;
    const int r1 = 15;
    const int r2 = 13;
    const uint32_t m = 5;
    const uint32_t n = 0xe6546b64;
    
    uint32_t h1 = (uint32_t)seed;
    
    const int nblocks = (int)(len / 4);
    const uint32_t* blocks = (const uint32_t*)data;
    
    for (int i = 0; i < nblocks; i++) {
        uint32_t k1 = blocks[i];
        
        k1 *= c1;
        k1 = rotl64(k1, r1);
        k1 *= c2;
        
        h1 ^= k1;
        h1 = rotl64(h1, r2);
        h1 = h1 * m + n;
    }
    
    const uint8_t* tail = (const uint8_t*)(data + nblocks * 4);
    uint32_t k1 = 0;
    
    switch (len & 3) {
        case 3: k1 ^= tail[2] << 16;
            __attribute__((fallthrough));
        case 2: k1 ^= tail[1] << 8;
            __attribute__((fallthrough));
        case 1: k1 ^= tail[0];
            k1 *= c1;
            k1 = rotl64(k1, r1);
            k1 *= c2;
            h1 ^= k1;
    }
    
    h1 ^= len;
    h1 = fmix32(h1);
    
    return h1;
}

uint64_t hash_murmur3_64(const void* key, size_t len, uint64_t seed) {
    const uint8_t* data = (const uint8_t*)key;
    const uint64_t c1 = 0x87c37b91114253d5ULL;
    const uint64_t c2 = 0x4cf5ad432745937fULL;
    const int r1 = 31;
    const int r2 = 27;
    const uint64_t m = 5;
    const uint64_t n1 = 0x52dce729;
    const uint64_t n2 = 0x38495ab5;
    
    uint64_t h1 = seed;
    uint64_t h2 = seed;
    
    const int nblocks = (int)(len / 16);
    
    for (int i = 0; i < nblocks; i++) {
        uint64_t k1 = 0;
        uint64_t k2 = 0;
        const uint8_t* block = data + i * 16;
        for (int j = 0; j < 8; j++) {
            k1 |= ((uint64_t)block[j]) << (j * 8);
        }
        for (int j = 0; j < 8; j++) {
            k2 |= ((uint64_t)block[8 + j]) << (j * 8);
        }
        
        k1 *= c1;
        k1 = rotl64(k1, r1);
        k1 *= c2;
        h1 ^= k1;
        
        h1 = rotl64(h1, r2);
        h1 += h2;
        h1 = h1 * m + n1;
        
        k2 *= c2;
        k2 = rotl64(k2, r1);
        k2 *= c1;
        h2 ^= k2;
        
        h2 = rotl64(h2, r2);
        h2 += h1;
        h2 = h2 * m + n2;
    }
    
    const uint8_t* tail = (const uint8_t*)(data + nblocks * 16);
    uint64_t k1 = 0;
    uint64_t k2 = 0;
    
    switch (len & 15) {
        case 15: k2 ^= ((uint64_t)tail[14]) << 48;
            __attribute__((fallthrough));
        case 14: k2 ^= ((uint64_t)tail[13]) << 40;
            __attribute__((fallthrough));
        case 13: k2 ^= ((uint64_t)tail[12]) << 32;
            __attribute__((fallthrough));
        case 12: k2 ^= ((uint64_t)tail[11]) << 24;
            __attribute__((fallthrough));
        case 11: k2 ^= ((uint64_t)tail[10]) << 16;
            __attribute__((fallthrough));
        case 10: k2 ^= ((uint64_t)tail[9]) << 8;
            __attribute__((fallthrough));
        case 9:  k2 ^= ((uint64_t)tail[8]) << 0;
                 k2 *= c2;
                 k2 = rotl64(k2, r1);
                 k2 *= c1;
                 h2 ^= k2;
                 __attribute__((fallthrough));
        case 8:  k1 ^= ((uint64_t)tail[7]) << 56;
            __attribute__((fallthrough));
        case 7:  k1 ^= ((uint64_t)tail[6]) << 48;
            __attribute__((fallthrough));
        case 6:  k1 ^= ((uint64_t)tail[5]) << 40;
            __attribute__((fallthrough));
        case 5:  k1 ^= ((uint64_t)tail[4]) << 32;
            __attribute__((fallthrough));
        case 4:  k1 ^= ((uint64_t)tail[3]) << 24;
            __attribute__((fallthrough));
        case 3:  k1 ^= ((uint64_t)tail[2]) << 16;
            __attribute__((fallthrough));
        case 2:  k1 ^= ((uint64_t)tail[1]) << 8;
            __attribute__((fallthrough));
        case 1:  k1 ^= ((uint64_t)tail[0]) << 0;
                 k1 *= c1;
                 k1 = rotl64(k1, r1);
                 k1 *= c2;
                 h1 ^= k1;
    }
    
    h1 ^= len;
    h2 ^= len;
    
    h1 += h2;
    h2 += h1;
    
    h1 = fmix64(h1);
    h2 = fmix64(h2);
    
    h1 += h2;
    h2 += h1;
    
    return h1;
}

uint64_t hash_siphash24(const void* key, size_t len, uint64_t seed) {
    const uint8_t* data = (const uint8_t*)key;
    
    uint64_t v0 = 0x736f6d6570736575ULL ^ seed;
    uint64_t v1 = 0x646f72616e646f6dULL ^ seed;
    uint64_t v2 = 0x6c7967656e657261ULL ^ seed;
    uint64_t v3 = 0x7465646279746573ULL ^ seed;
    
    uint64_t k0 = seed;
    uint64_t k1 = seed ^ 0xdeadbeefdeadbeefULL;
    
    #define SIPROUND do { \
        v0 += v1; v1 = rotl64(v1, 13); v1 ^= v0; v0 = rotl64(v0, 32); \
        v2 += v3; v3 = rotl64(v3, 16); v3 ^= v2; \
        v0 += v3; v3 = rotl64(v3, 21); v3 ^= v0; v0 = rotl64(v0, 32); \
        v2 += v1; v1 = rotl64(v1, 17); v1 ^= v2; v2 = rotl64(v2, 32); \
    } while (0)
    
    v3 ^= k1;
    v2 ^= k0;
    v1 ^= k1;
    v0 ^= k0;
    
    const int nblocks = (int)(len / 8);
    const uint64_t* blocks = (const uint64_t*)data;
    
    for (int i = 0; i < nblocks; i++) {
        uint64_t mi = blocks[i];
        v3 ^= mi;
        
        SIPROUND;
        SIPROUND;
        
        v0 ^= mi;
    }
    
    const uint8_t* tail = (const uint8_t*)(data + nblocks * 8);
    uint64_t b = ((uint64_t)len) << 56;
    
    switch (len & 7) {
        case 7: b |= ((uint64_t)tail[6]) << 48;
            __attribute__((fallthrough));
        case 6: b |= ((uint64_t)tail[5]) << 40;
            __attribute__((fallthrough));
        case 5: b |= ((uint64_t)tail[4]) << 32;
            __attribute__((fallthrough));
        case 4: b |= ((uint64_t)tail[3]) << 24;
            __attribute__((fallthrough));
        case 3: b |= ((uint64_t)tail[2]) << 16;
            __attribute__((fallthrough));
        case 2: b |= ((uint64_t)tail[1]) << 8;
            __attribute__((fallthrough));
        case 1: b |= ((uint64_t)tail[0]) << 0;
    }
    
    v3 ^= b;
    
    SIPROUND;
    SIPROUND;
    
    v0 ^= b;
    
    v2 ^= 0xff;
    
    SIPROUND;
    SIPROUND;
    SIPROUND;
    SIPROUND;
    
    return (v0 ^ v1) ^ (v2 ^ v3);
    
    #undef SIPROUND
}

uint64_t hash_integer(uint64_t key, uint64_t seed) {
    key ^= seed;
    key = (~key) + (key << 21);
    key = key ^ (key >> 24);
    key = (key + (key << 3)) + (key << 8);
    key = key ^ (key >> 14);
    key = (key + (key << 2)) + (key << 4);
    key = key ^ (key >> 28);
    key = key + (key << 31);
    return key;
}
