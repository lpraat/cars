#ifndef CARS_VEC_H
#define CARS_VEC_H

/* Dynamic array implementation */

#include <stdlib.h>
#include <string.h>

#include "cars/allocator.h"
#include "cars/types.h"
#include "cars/utils.h"

#define CONCAT_(a, b) a##b
#define CONCAT(a, b) CONCAT_(a, b)
#endif

#if defined(T_VEC_NAME)
#define VEC_NAME(T) T_VEC_NAME
#define VEC_METHOD(name) CONCAT(CONCAT(T_VEC_NAME, _), name)
#else
#define VEC_NAME(T) CONCAT(T, _Vec)
#define VEC_METHOD(name) CONCAT(CONCAT(T, _vec_), name)
#endif

#ifndef NO_VEC_DECL
typedef struct VEC_NAME(T) {
    Allocator* allocator;
    T* data;
    size_t capacity;
    size_t len;
} VEC_NAME(T);
#endif

#ifndef NO_VEC_IMPL
void* VEC_METHOD(reserve)(VEC_NAME(T)* vec, size_t additional_capacity) {
    vec->data = vec->allocator->vtable->realloc(
        vec->allocator, vec->data,
        (vec->capacity + additional_capacity) * sizeof(T)
    );
    if (!vec->data) {
        MEMERR();
    }
    vec->capacity += additional_capacity;
    return vec->data;
}

void VEC_METHOD(push)(VEC_NAME(T)* vec, T elem) {
    if (vec->len + 1 > vec->capacity) {
        const size_t new_capacity = vec->capacity ? vec->capacity * 2 : 1;
        VEC_METHOD(reserve)(vec, new_capacity - vec->capacity);
    }

    vec->data[vec->len] = elem;
    vec->len += 1;
}

T VEC_METHOD(pop)(VEC_NAME(T)* vec) {
    vec->len--;
    return vec->data[vec->len];
}

void VEC_METHOD(extend)(VEC_NAME(T)* vec, VEC_NAME(T) const* other) {
    if (vec->capacity - vec->len < other->len) {
        VEC_METHOD(reserve)(vec, other->len - (vec->capacity - vec->len));
    }
    memcpy(
        vec->data + vec->len * sizeof(T), other->data, other->len * sizeof(T)
    );
}

void VEC_METHOD(drop)(VEC_NAME(T)* vec) {
    vec->allocator->vtable->free(vec->allocator, vec->data);
}

VEC_NAME(T) VEC_METHOD(new)(Allocator* a) {
    VEC_NAME(T) t = {
        .allocator = a,
        .data = 0,
        .capacity = 0,
        .len = 0,
    };
    return t;
}

VEC_NAME(T) VEC_METHOD(new_with_capacity)(Allocator* a, size_t capacity) {
    VEC_NAME(T) vec = {.allocator = a};
    VEC_METHOD(reserve)(&vec, capacity);
    return vec;
}

void VEC_METHOD(sort)(
    VEC_NAME(T)* vec, s32 (*cmp)(void const* p1, void const* p2)
) {
    qsort(vec->data, vec->len, sizeof(T), cmp);
}

#endif

#undef VEC_NAME_T
#undef NO_VEC_DECL
#undef NO_VEC_IMPL
#undef T

#undef VEC_NAME
#undef VEC_METHOD
