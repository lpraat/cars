#include <stdio.h>

#include "allocator.h"
#include "errors.h"
#include "types.h"

typedef struct Node Node;
struct Node {
    int x;
};

#define T Node
#include "vec.h"

#define V_LEN 10
#define V2_LEN 112

int main(int argc, char* argv[argc + 1]) {
    Allocator a = c_allocator;
    Node_Vec v = Node_Vec_new(&a);

    printf("%d\n", cars_errorno);

    for (size_t i = 0; i < V_LEN; i++) {
        Node_Vec_push(&v, (Node){.x = i});
    }

    Node_Vec v2 = Node_Vec_new(&a);
    for (size_t i = 0; i < V2_LEN; i++) {
        Node_Vec_push(&v, (Node){.x = 10 * i});
    }

    Node_Vec_extend(&v, &v2);

    for (size_t i = 0; i < V_LEN + V2_LEN; i++) {
        printf("El: %d\n", v.data[i].x);
    }

    Node_Vec_drop(&v);
    Node_Vec_drop(&v2);

    return 0;
}
