#ifndef VEC_H_
#define VEC_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "allocator.h"
#include "types.h"
#define CONCAT_(a, b) a##b
#define CONCAT(a, b) CONCAT_(a, b)
#endif

#if defined(T_VEC_NAME)
#define VEC_NAME(T) T_VEC_NAME
#define METHOD_NAME(name) CONCAT(CONCAT(T_VEC_NAME, _), name)
#else
#define VEC_NAME(T) CONCAT(CONCAT(T, _), Vec)
#define METHOD_NAME(name) CONCAT(VEC_NAME(T), CONCAT(_, name))
#endif

#ifndef NO_VEC_DECL
typedef struct VEC_NAME(T) VEC_NAME(T);
struct VEC_NAME(T) {
    Allocator* allocator;
    T* data;
    size_t capacity;
    size_t len;
};
#endif

#ifndef NO_VEC_IMPL
void METHOD_NAME(reserve)(VEC_NAME(T) * vec, size_t additional_capacity) {
    vec->data = vec->allocator->realloc(
        vec->allocator, vec->data,
        (vec->capacity + additional_capacity) * sizeof(T)
    );
}

void METHOD_NAME(push)(VEC_NAME(T) * vec, T elem) {
    if (vec->len + 1 > vec->capacity) {
        const size_t new_capacity = vec->capacity ? vec->capacity * 2 : 1;
        METHOD_NAME(reserve)(vec, new_capacity - vec->capacity);

        if (!vec->data) {
            // TODO: panic?
            return;
        }
        vec->capacity = new_capacity;
    }

    vec->data[vec->len] = elem;
    vec->len += 1;
}

T METHOD_NAME(pop)(VEC_NAME(T) * vec) {
    vec->len--;
    return vec->data[vec->len];
}

void METHOD_NAME(extend)(VEC_NAME(T) * vec, VEC_NAME(T) * other) {
    if (vec->capacity - vec->len < other->len) {
        METHOD_NAME(reserve)(vec, other->len - (vec->capacity - vec->len));
        if (!vec->data) {
            // TODO: panic?
            return;
        }
    }
    memcpy(
        vec->data + vec->len * sizeof(T), other->data, other->len * sizeof(T)
    );
}

VEC_NAME(T)
METHOD_NAME(new)(Allocator* a) {
    VEC_NAME(T)
    t = {
        .allocator = a,
        .data = 0,
        .capacity = 0,
        .len = 0,
    };
    return t;
}

VEC_NAME(T)
METHOD_NAME(new_with_capacity)(size_t capacity) {
    VEC_NAME(T)
    t = {0};
    t.capacity = capacity;
    return t;
}

#endif

#undef VEC_NAME
#undef METHOD_NAME
#undef VEC_IMPLEMENTATION
#undef NO_VEC_DECL
#undef NO_VEC_IMPL
