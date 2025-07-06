#include "types.h"
#include <stdio.h>

typedef struct Node Node;

#define T Node
#define NO_VEC_IMPL
#include "vec.h"

struct Node {
    int x;
    Node_Vec children;
};

#define VEC_IMPLEMENTATION
#define NO_VEC_DECL
#include "vec.h"

int main(int argc, char* argv[argc + 1]) {
    Node_Vec node_vec = Node_Vec_new();
    Node_Vec_push(&node_vec, (Node){.x = 72, .children = Node_Vec_new()});
    Node n = {.x = 10, .children = node_vec};
    Node popped = Node_Vec_pop(&n.children);
    printf("%d", popped.x);
    return 0;
}
