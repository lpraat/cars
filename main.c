#include <stdio.h>

#include "cars/allocator.h"
#include "cars/arena.h"
#include "cars/base.h"
#include "cars/hash.h"
#include "cars/string.h"

typedef struct Node Node;

/* Define the Node_pointers struct (a Vec of Node pointers) */
#define T Node*
#define T_VEC_NAME Node_pointers
#define NO_VEC_IMPL
#include "cars/vec.h"

struct Node {
    u32 value;
    Node_pointers children;
};

/* Define the methods to operate on Node_pointers */
#define T Node*
#define T_VEC_NAME Node_pointers
#define NO_VEC_DECL
#include "cars/vec.h"

/* Define a Vec of Nodes */
#define T Node
#include "cars/vec.h"

/* Define a HashMap to map Strings to integers */
#define K String
#define V s32
#include "cars/hashmap.h"

#define N_NODES 10

int main(int argc, char* argv[argc + 1]) {
    UNUSED(argv);

    /* Allocators */
    ArenaAllocator a = arena_allocator_new(1 << 30);

    // CAllocator a = c_allocator;  // To use the standard C allocator

    /* Work with dynamic arrays (Vecs) */
    Node_Vec all_nodes = Node_vec_new_with_capacity((Allocator*)&a, N_NODES);
    for (size_t i = 0; i < N_NODES; i++) {
        Node n = {.value = i, .children = Node_pointers_new((Allocator*)&a)};
        Node_vec_push(&all_nodes, n);
    }

    for (size_t i = 0; i < N_NODES; i++) {
        for (size_t j = 0; j < N_NODES; j++) {
            if (i != j && all_nodes.data[i].value % all_nodes.data[j].value == 0) {
                Node_pointers_push(&all_nodes.data[i].children, &all_nodes.data[j]);
            }
        }
    }

    for (size_t i = 0; i < N_NODES; i++) {
        printf("Node %d children are:\n", all_nodes.data[i].value);
        for (size_t j = 0; j < all_nodes.data[i].children.len; j++) {
            printf("\t- %d\n", all_nodes.data[i].children.data[j]->value);
        }
    };

    // These drops are no-ops since we are using the arena allocator
    for (size_t i = 0; i < N_NODES; i++) {
        Node_pointers_drop(&all_nodes.data[i].children);
    };
    Node_vec_drop(&all_nodes);

    /* Work with Strings (e.g., using a temporary inner arena) */
    InnerArena inner = arena_allocator_new_inner(&a);

    String s = string_from_cstr((Allocator*)inner.arena, "ab");
    String s_lower = string_ascii_uppercase((Allocator*)inner.arena, &s);

    printf("Before: %.*s\n", (s32)s.len, s.bytes);
    string_push_string(&s, &s_lower);
    printf("After: %.*s\n", (s32)s.len, s.bytes);

    String out = string_concat((Allocator*)inner.arena, &s, &s_lower);
    printf("Out: %.*s\n", (s32)out.len, out.bytes);

    string_drop(&s);
    string_drop(&s_lower);
    string_drop(&out);
    arena_allocator_drop_inner(&a, &inner);

    /* Work with HashMaps */
    // The key is a String, so we use the string hash context
    StringHashContext hash_ctx = string_hash_ctx_new();
    String_s32_HashMap map = String_s32_hashmap_new((Allocator*)&a, (HashContext*)&hash_ctx);

    String foo = string_from_cstr((Allocator*)&a, "foo");
    String_s32_hashmap_insert(&map, foo, 11);

    String bar = string_from_cstr((Allocator*)&a, "bar");
    String_s32_hashmap_insert(&map, bar, 22);

    s32* bar_value_ptr = String_s32_hashmap_get(&map, &bar);
    printf("Value for key bar is: %d\n", *bar_value_ptr);

    String_s32_hashmap_remove(&map, &bar);
    bar_value_ptr = String_s32_hashmap_get(&map, &bar);
    printf("This ptr should now be null: %p\n", bar_value_ptr);
    assert(!bar_value_ptr);

    String_s32_hashmap_drop(&map);
    string_drop(&foo);
    string_drop(&bar);

    /* Free all the memory backing the arena */
    arena_allocator_drop(&a);

    return 0;
}
