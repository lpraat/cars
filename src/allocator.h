#ifndef ALLOCATOR_H_
#define ALLOCATOR_H_

#include <stdio.h>
#include <stdlib.h>

#include "types.h"

typedef struct Allocator {
    void* (*alloc)(struct Allocator*, size_t);
    void* (*realloc)(struct Allocator*, void*, size_t);
    void (*free)(struct Allocator*, void*);
    void (*reset)(struct Allocator*);
} Allocator;

typedef struct CAllocator {
    Allocator base;
} CAllocator;

static void* c_allocator_alloc(Allocator* self, size_t size) {
    return malloc(size);
}

static void* c_allocator_realloc(Allocator* self, void* ptr, size_t size) {
    return realloc(ptr, size);
}

static void c_allocator_free(Allocator* self, void* ptr) { free(ptr); }

static const Allocator c_allocator = {
    .alloc = c_allocator_alloc,
    .realloc = c_allocator_realloc,
    .free = c_allocator_free,
    .reset = 0
};

#endif
