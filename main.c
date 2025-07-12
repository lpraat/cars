#include <stdio.h>
#include <string.h>

#include "cars/allocator.h"
#include "cars/errors.h"
#include "cars/string.h"
#include "cars/types.h"

typedef struct Node Node;
struct Node {
    int x;
};

#define T Node
#include "cars/vec.h"

#define V_LEN 2
#define V2_LEN 3

int main(int argc, char* argv[argc + 1]) {
    Allocator a = c_allocator;

    String s = String_from_cstr(&a, "AB");
    String s_lower = String_ascii_uppercase(&a, &s);

    printf("Before: %.*s\n", (s32)s.len, s.data);

    String_push_String(&s, &s_lower);

    printf("After: %.*s\n", (s32)s.len, s.data);

    String_drop(&s);
    String_drop(&s_lower);

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
