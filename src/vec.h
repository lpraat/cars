#ifndef INCLUDE_VEC_H
#define INCLUDE_VEC_H
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    T* data;
    size_t capacity;
    size_t len;
};
#endif



#ifndef NO_VEC_IMPL
s32 METHOD_NAME(push)(VEC_NAME(T) * vec, T elem) {
    if (vec->len + 1 > vec->capacity) {
        const size_t new_capacity = vec->capacity ? vec->capacity * 2 : 1;

        T* const curr_data = vec->data;

        // TODO: use an allocator
        vec->data = malloc(new_capacity * sizeof(T));

        if (!vec->data) {
            return -1;
        }

        if (curr_data) {
            memcpy(vec->data, curr_data, sizeof(T) * vec->len);
            free(curr_data);
        }

        vec->capacity = new_capacity;
    }

    vec->data[vec->len] = elem;
    vec->len += 1;
    return 0;
}

T METHOD_NAME(pop)(VEC_NAME(T) * vec) {
    vec->len--;
    return vec->data[vec->len];
}

VEC_NAME(T) METHOD_NAME(new)() {
    VEC_NAME(T) t = {0};
    return t;
}

VEC_NAME(T) METHOD_NAME(new_with_capacity)(size_t capacity) {
    VEC_NAME(T) t = {0};
    t.capacity = capacity;
    return t;
}

#endif

#undef VEC_NAME
#undef METHOD_NAME
#undef VEC_IMPLEMENTATION
#undef NO_VEC_DECL
#undef NO_VEC_IMPL