#ifndef CARS_ALLOCATOR_H
#define CARS_ALLOCATOR_H

#include <stdlib.h>

#include "cars/utils.h"

typedef struct Allocator Allocator;

typedef struct AllocatorVTable {
    void* (*alloc)(Allocator*, size_t);
    void* (*realloc)(Allocator*, void*, size_t);
    void (*free)(Allocator*, void*);
} AllocatorVTable;

struct Allocator {
    AllocatorVTable const* vtable;
};

static AllocatorVTable allocator_vtable = {0};

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

static AllocatorVTable callocator_vtable = {
    .alloc = c_allocator_alloc,
    .realloc = c_allocator_realloc,
    .free = c_allocator_free,
};

static const CAllocator c_allocator = {.base = {.vtable = &callocator_vtable}};

#endif
