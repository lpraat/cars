#ifndef CARS_TYPES_H
#define CARS_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* Types */
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef float f32;
typedef double f64;
typedef size_t size_t;
typedef uintptr_t uintptr_t;
typedef intptr_t intptr_t;

/* Utilities */
#define UNUSED(arg) (void)arg

void todo(char const* msg, char const* file_name, s32 lineno) {
    fprintf(stderr, "TODO (file: %s, line: %d): %s", file_name, lineno, msg);
    exit(1);
}

#define TODO(msg)                      \
    do {                               \
        todo(msg, __FILE__, __LINE__); \
    } while (0)

void memerr(char const* file_name, s32 lineno) {
    fprintf(stderr, "Memory allocation error (file: %s, line: %d).", file_name, lineno);
    exit(1);
}

#define MEMERR()                    \
    do {                            \
        memerr(__FILE__, __LINE__); \
    } while (0)

#endif
