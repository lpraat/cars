#ifndef CARS_ALLOCATOR_H_
#define CARS_ALLOCATOR_H_

#include <stdlib.h>

#include "cars/utils.h"

typedef struct Allocator {
    void* (*alloc)(struct Allocator*, size_t);
    void* (*realloc)(struct Allocator*, void*, size_t);
    void (*free)(struct Allocator*, void*);
} Allocator;

typedef struct CAllocator {
    Allocator base;
} CAllocator;

static void* c_allocator_alloc(Allocator* self, size_t size) {
    UNUSED(self);
    return malloc(size);
}

static void* c_allocator_realloc(Allocator* self, void* ptr, size_t size) {
    UNUSED(self);
    return realloc(ptr, size);
}

static void c_allocator_free(Allocator* self, void* ptr) {
    UNUSED(self);
    free(ptr);
}

static const CAllocator c_allocator = {
    .base = {
        .alloc = c_allocator_alloc,
        .realloc = c_allocator_realloc,
        .free = c_allocator_free,
    }
};

#endif
