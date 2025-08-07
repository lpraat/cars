#ifndef CARS_HASH_H
#define CARS_HASH_H

#include <_stdlib.h>
#include <string.h>

#include "cars/types.h"
#include "cars/utils.h"

/*
 * Hash functions taken from:
 * https://github.com/gcc-mirror/gcc/blob/master/libstdc++-v3/libsupc++/hash_bytes.cc
 */
static inline size_t unaligned_load(const char* p) {
    size_t result;
    __builtin_memcpy(&result, p, sizeof(result));
    return result;
}

#if __SIZEOF_SIZE_T__ == 4

// Implementation of Murmur hash for 32-bit size_t.
size_t hash_bytes(const void* ptr, size_t len, size_t seed) {
    const size_t m = 0x5bd1e995;
    size_t hash = seed ^ len;
    const char* buf = (const char*)(ptr);

    // Mix 4 bytes at a time into the hash.
    while (len >= 4) {
        size_t k = unaligned_load(buf);
        k *= m;
        k ^= k >> 24;
        k *= m;
        hash *= m;
        hash ^= k;
        buf += 4;
        len -= 4;
    }

    size_t k;
    // Handle the last few bytes of the input array.
    switch (len) {
        case 3:
            k = (unsigned char)(buf[2]);
            hash ^= k << 16;
        case 2:
            k = (unsigned char)(buf[1]);
            hash ^= k << 8;
        case 1:
            k = (unsigned char)(buf[0]);
            hash ^= k;
            hash *= m;
    };

    // Do a few final mixes of the hash.
    hash ^= hash >> 13;
    hash *= m;
    hash ^= hash >> 15;
    return hash;
}

#elif __SIZEOF_SIZE_T__ == 8

static inline size_t load_bytes(const char* p, int n) {
    size_t result = 0;
    --n;
    do result = (result << 8) + (unsigned char)(p[n]);
    while (--n >= 0);
    return result;
}

static inline size_t shift_mix(size_t v) { return v ^ (v >> 47); }

// Implementation of Murmur hash for 64-bit size_t.
size_t hash_bytes(const void* ptr, size_t len, size_t seed) {
    static const size_t mul =
        (((size_t)0xc6a4a793UL) << 32UL) + (size_t)0x5bd1e995UL;
    const char* const buf = (const char*)(ptr);

    // Remove the bytes not divisible by the sizeof(size_t).  This
    // allows the main loop to process the data as 64-bit integers.
    const size_t len_aligned = len & ~(size_t)0x7;
    const char* const end = buf + len_aligned;
    size_t hash = seed ^ (len * mul);
    for (const char* p = buf; p != end; p += 8) {
        const size_t data = shift_mix(unaligned_load(p) * mul) * mul;
        hash ^= data;
        hash *= mul;
    }
    if ((len & 0x7) != 0) {
        const size_t data = load_bytes(end, len & 0x7);
        hash ^= data;
        hash *= mul;
    }
    hash = shift_mix(hash) * mul;
    hash = shift_mix(hash);
    return hash;
}

#else

/* http://www.cse.yorku.ca/~oz/hash.html */
size_t hash_bytes(const void* ptr, size_t len) {
    size_t hash = 5381;

    for (size_t i = 0; i < len; i++) {
        hash = ((hash << 5) + hash) + *((u8*)ptr + i);
    }
    return hash;
}

#endif

typedef struct HashContext HashContext;

typedef struct HashContextVTable {
    size_t (*hash_bytes)(HashContext*, void const*, size_t len);
    bool (*are_equal)(HashContext*, void const*, void const*, size_t);
} HashContextVTable;

struct HashContext {
    HashContextVTable const* vtable;
};

static HashContextVTable hash_ctx_vtable = {0};

/* Default hash context */
typedef struct DefaultHashContext {
    HashContext base;
    size_t seed;
} DefaultHashContext;

static size_t default_hash_ctx_hash_bytes(
    HashContext* self, void const* ptr, size_t len
) {
    DefaultHashContext* default_ctx = (DefaultHashContext*)self;
    return hash_bytes(ptr, len, default_ctx->seed);
}

static bool default_hash_ctx_are_equal(
    HashContext* self, void const* p1, void const* p2, size_t size
) {
    UNUSED(self);
    return !memcmp(p1, p2, size);
}

static HashContextVTable default_hash_ctx_vtable = {
    .hash_bytes = default_hash_ctx_hash_bytes,
    .are_equal = default_hash_ctx_are_equal
};

DefaultHashContext default_hash_ctx_new() {
    return (DefaultHashContext){.base = {.vtable = &default_hash_ctx_vtable},
                                .seed = arc4random()};
}

#endif
