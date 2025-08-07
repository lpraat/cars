#ifndef CARS_UTILS_H
#define CARS_UTILS_H

#include <stdio.h>
#define UNUSED(arg) (void)arg

void todo(char const* msg) { fprintf(stderr, "TODO: %s", msg); }

#define TODO(msg)  \
    do {           \
        todo(msg); \
        exit(1);   \
    } while (0)

#endif
