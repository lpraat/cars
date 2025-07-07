#include "allocator.h"
#include "types.h"
#include <stdio.h>

typedef struct Node Node;
struct Node {
    int x;
};

#define T Node
#include "vec.h"

#define V_LEN 0
#define V2_LEN 1

int main(int argc, char* argv[argc + 1]) {
    Allocator a = c_allocator;
    
    Node_Vec v = Node_Vec_new(&a);
    for (size_t i = 0; i < V_LEN; i++) {
        Node_Vec_push(&v, (Node){.x=i});    
    }
    
    Node_Vec v2 = Node_Vec_new(&a);
    for (size_t i = 0; i < V2_LEN; i++) {
        Node_Vec_push(&v, (Node){.x=10*i});    
    }
    
    Node_Vec_extend(&v, &v2);
    
    for (size_t i = 0; i < V_LEN + V2_LEN; i++) {
        printf("El: %d\n", v.data[i].x);
    }
    

    return 0;
}
