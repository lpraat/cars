#ifndef CARS_ERRORS_H_
#define CARS_ERRORS_H_

#include <stdio.h>
#include <stdlib.h>

#include "cars/types.h"

typedef enum Cars_Error {
    Cars_Success = 1,
    Cars_MemErr = -1,
} Cars_Error;

static Cars_Error cars_errorno = Cars_Success;

void memerr(char const* file_name, s32 lineno) {
    cars_errorno = Cars_MemErr;
    fprintf(stderr, "Memory error (file: %s, line: %d).", file_name, lineno);
    exit(1);
}

#define MEMERR()                    \
    do {                            \
        memerr(__FILE__, __LINE__); \
    } while (0)

#endif