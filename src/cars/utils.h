#ifndef CARS_UTILS_H
#define CARS_UTILS_H

#include <stdio.h>
#include <stdlib.h>

#include "cars/types.h"
#define UNUSED(arg) (void)arg

void todo(char const* msg) { fprintf(stderr, "TODO: %s", msg); }

#define TODO(msg)  \
    do {           \
        todo(msg); \
        exit(1);   \
    } while (0)

void memerr(char const* file_name, s32 lineno) {
    fprintf(
        stderr, "Memory allocation error (file: %s, line: %d).", file_name,
        lineno
    );
    exit(1);
}

#define MEMERR()                    \
    do {                            \
        memerr(__FILE__, __LINE__); \
    } while (0)

#endif
