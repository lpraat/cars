#ifndef CARS_ERRORS_H_
#define CARS_ERRORS_H_

typedef enum Cars_Error {
    Cars_Success = 1,
    Cars_ENoMem = -1,
} Cars_Error;

static Cars_Error cars_errorno = Cars_Success;

#endif