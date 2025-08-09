#ifndef CARS_HASHMAP_H
#define CARS_HASHMAP_H

/* HashMap implementation with linear probing */

#include <assert.h>
#include <string.h>

#include "cars/allocator.h"
#include "cars/base.h"
#include "cars/hash.h"

#define INITIAL_N_SLOTS 16
#define LOAD_FACTOR 0.75

typedef enum HashMapSlotState {
    HashMapSlotState_Empty = 0,
    HashMapSlotState_Full,
    HashMapSlotState_Deleted,
} HashMapSlotState;

#define CONCAT_(a, b) a##b
#define CONCAT(a, b) CONCAT_(a, b)
#endif

#if defined(KV_HASHMAP_NAME)
#define HASHMAP_NAME(K, V) KV_HASHMAP_NAME
#define HASHMAP_METHOD(name) CONCAT(CONCAT(KV_HASHMAP_NAME, _), name)
#else
#define HASHMAP_NAME(K, V) CONCAT(CONCAT(CONCAT(K, _), V), _HashMap)
#define HASHMAP_METHOD(name) CONCAT(CONCAT(CONCAT(CONCAT(K, _), V), _hashmap_), name)
#endif
#define HASHMAP_SLOT(K, V) CONCAT(HASHMAP_NAME(K, V), _Entry)

#ifndef NO_HASHMAP_DECL

typedef struct HASHMAP_SLOT(K, V) {
    K key;
    V value;
    HashMapSlotState state;
} HASHMAP_SLOT(K, V);

typedef struct HASHMAP_NAME(K, V) {
    Allocator* allocator;
    HashContext* hash_ctx;
    HASHMAP_SLOT(K, V) * slots;
    size_t n_slots;
    size_t n_full;
} HASHMAP_NAME(K, V);
#endif

#ifndef NO_HASHMAP_IMPL

static void HASHMAP_METHOD(initialize)(HASHMAP_NAME(K, V)* hashmap, size_t n_slots) {
    assert(hashmap->n_full == 0 && hashmap->n_slots == 0);
    hashmap->slots =
        hashmap->allocator->vtable->alloc(hashmap->allocator, n_slots * sizeof(HASHMAP_SLOT(K, V)));

    if (!hashmap->slots) {
        MEMERR();
    }

    // Zero memory so that slot.state == HashMapSlotState_Empty
    memset(hashmap->slots, 0, n_slots * sizeof(HASHMAP_SLOT(K, V)));
    hashmap->n_slots = n_slots;
}

HASHMAP_NAME(K, V) HASHMAP_METHOD(new)(Allocator* allocator, HashContext* hash_ctx) {
    return (HASHMAP_NAME(K, V)){
        .allocator = allocator, .slots = 0, .hash_ctx = hash_ctx, .n_slots = 0, .n_full = 0
    };
}

HASHMAP_NAME(K, V) HASHMAP_METHOD(new_with_n_slots)(
    Allocator* allocator, HashContext* hash_ctx, size_t n_slots
) {
    HASHMAP_NAME(K, V) map = {
        .allocator = allocator, .slots = 0, .hash_ctx = hash_ctx, .n_slots = 0, .n_full = 0
    };
    HASHMAP_METHOD(initialize)(&map, n_slots);
    return map;
}

V* HASHMAP_METHOD(get)(HASHMAP_NAME(K, V)* hashmap, K* key) {
    size_t hash = hashmap->hash_ctx->vtable->hash_bytes(hashmap->hash_ctx, key, sizeof(*key));
    size_t start_probe = hash % hashmap->n_slots;
    for (size_t i = 0; i < hashmap->n_slots; i++) {
        HASHMAP_SLOT(K, V)* slot = &hashmap->slots[(start_probe + i) % hashmap->n_slots];
        if (slot->state == HashMapSlotState_Empty) {
            return 0;
        }
        if (slot->state == HashMapSlotState_Full &&
            hashmap->hash_ctx->vtable->are_equal(
                hashmap->hash_ctx, &slot->key, key, sizeof(*key)
            )) {
            return &slot->value;
        }
    }
    return 0;
}

bool HASHMAP_METHOD(contains_key)(HASHMAP_NAME(K, V)* hashmap, K* key) {
    return HASHMAP_METHOD(get)(hashmap, key);
}

void HASHMAP_METHOD(remove)(HASHMAP_NAME(K, V)* hashmap, K* key) {
    size_t hash = hashmap->hash_ctx->vtable->hash_bytes(hashmap->hash_ctx, key, sizeof(*key));
    size_t start_probe = hash % hashmap->n_slots;
    for (size_t i = 0; i < hashmap->n_slots; i++) {
        HASHMAP_SLOT(K, V)* slot = &hashmap->slots[(start_probe + i) % hashmap->n_slots];
        if (slot->state == HashMapSlotState_Full &&
            hashmap->hash_ctx->vtable->are_equal(
                hashmap->hash_ctx, &slot->key, key, sizeof(*key)
            )) {
            slot->state = HashMapSlotState_Deleted;
            hashmap->n_full--;
            return;
        }
    }
}

void HASHMAP_METHOD(insert)(HASHMAP_NAME(K, V)* hashmap, K key, V value) {
    if (hashmap->n_slots == 0) {
        HASHMAP_METHOD(initialize)(hashmap, INITIAL_N_SLOTS);
    }

    if ((f64)hashmap->n_full / hashmap->n_slots >= LOAD_FACTOR) {
        // Create new slots and rehash
        size_t new_n_slots = hashmap->n_slots * 2;
        HASHMAP_SLOT(K, V)* new_slots = hashmap->allocator->vtable->alloc(
            hashmap->allocator, new_n_slots * sizeof(HASHMAP_SLOT(K, V))
        );
        if (!new_slots) {
            MEMERR();
        }

        // Zero memory so that slot.state == HashMapSlotState_Empty
        memset(new_slots, 0, new_n_slots * sizeof(HASHMAP_SLOT(K, V)));

        size_t n_full = 0;
        for (size_t i = 0; i < hashmap->n_slots; i++) {
            if (hashmap->slots[i].state == HashMapSlotState_Full) {
                K* key_to_rehash = &hashmap->slots[i].key;
                size_t hash = hashmap->hash_ctx->vtable->hash_bytes(
                    hashmap->hash_ctx, key_to_rehash, sizeof(*key_to_rehash)
                );
                size_t start_probe = hash % new_n_slots;
                for (size_t j = 0; j < new_n_slots; j++) {
                    size_t slot_idx = (start_probe + j) % new_n_slots;
                    if (new_slots[slot_idx].state == HashMapSlotState_Empty) {
                        new_slots[slot_idx] = hashmap->slots[i];
                        n_full += 1;
                        break;
                    }
                }
            }
        }

        hashmap->allocator->vtable->free(hashmap->allocator, hashmap->slots);
        hashmap->slots = new_slots;
        hashmap->n_slots = new_n_slots;
        hashmap->n_full = n_full;
    }

    // Insert new element
    size_t hash = hashmap->hash_ctx->vtable->hash_bytes(hashmap->hash_ctx, &key, sizeof(key));
    size_t start_probe = hash % hashmap->n_slots;
    bool using_deleted_slot = false;
    size_t insert_slot;

    for (size_t i = 0; i < hashmap->n_slots; i++) {
        size_t slot_idx = (start_probe + i) % hashmap->n_slots;
        HASHMAP_SLOT(K, V)* slot = &hashmap->slots[slot_idx];

        if (slot->state == HashMapSlotState_Deleted) {
            insert_slot = slot_idx;
            using_deleted_slot = true;
        } else if (slot->state == HashMapSlotState_Full &&
                   hashmap->hash_ctx->vtable->are_equal(
                       hashmap->hash_ctx, &key, &slot->key, sizeof(key)
                   )) {
            // Key is already present
            insert_slot = slot_idx;
            break;
        } else if (slot->state == HashMapSlotState_Empty) {
            if (!using_deleted_slot) {
                insert_slot = slot_idx;
            }
            break;
        }
    }

    hashmap->slots[insert_slot] =
        (HASHMAP_SLOT(K, V)){.key = key, .value = value, .state = HashMapSlotState_Full};
    hashmap->n_full += 1;
}

void HASHMAP_METHOD(drop)(HASHMAP_NAME(K, V)* hashmap) {
    hashmap->allocator->vtable->free(hashmap->allocator, hashmap->slots);
}

#endif

#undef KV_HASHMAP_NAME
#undef NO_HASHMAP_DECL
#undef NO_HASHMAP_IMPL
#undef K
#undef V

#undef HASHMAP_NAME
#undef HASHMAP_METHOD
