#include <stdalign.h>
#include <stdio.h>
#include <string.h>

#include "cars/allocator.h"
#include "cars/arena.h"
#include "cars/errors.h"
#include "cars/string.h"
#include "cars/types.h"

typedef struct Node Node;
struct Node {
    int x;
};

#define T Node
#include "cars/vec.h"

#define V_LEN 10
#define V2_LEN 30

int main(int argc, char* argv[argc + 1]) {
    ArenaAllocator a = arena_allocator_new(1e9);

    InnerArena inner = arena_allocator_new_inner(&a);

    String s = string_from_cstr((Allocator*)inner.arena, "AB");
    String s_lower = string_ascii_uppercase((Allocator*)inner.arena, &s);

    printf("Before: %.*s\n", (s32)s.len, s.bytes);

    string_push_string(&s, &s_lower);

    printf("After: %.*s\n", (s32)s.len, s.bytes);

    String out = string_concat((Allocator*)inner.arena, &s, &s_lower);
    printf("%zu %zu", out.len, out.capacity);
    printf("Out: %.*s\n", (s32)out.len, out.bytes);

    string_drop(&s);
    string_drop(&s_lower);
    string_drop(&out);
    arena_allocator_drop_inner(&a, &inner);

    Node_Vec v = Node_vec_new((Allocator*)&a);

    printf("%d\n", cars_errorno);

    for (size_t i = 0; i < V_LEN; i++) {
        Node_vec_push(&v, (Node){.x = i});
    }

    for (size_t i = 0; i < V_LEN; i++) {
        printf("el: %d\n", v.data[i].x);
    }

    Node_Vec v2 = Node_vec_new((Allocator*)&a);
    for (size_t i = 0; i < V2_LEN; i++) {
        Node_vec_push(&v, (Node){.x = 10 * i});
    }

    Node_vec_extend(&v, &v2);

    for (size_t i = 0; i < V_LEN + V2_LEN; i++) {
        printf("El: %d\n", v.data[i].x);
    }

    Node_vec_drop(&v);
    Node_vec_drop(&v2);
    arena_allocator_drop(&a);

    return 0;
}
