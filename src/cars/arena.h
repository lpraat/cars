#ifndef CARS_ARENA_H
#define CARS_ARENA_H

#include <stdalign.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/errno.h>

#include "cars/allocator.h"
#include "cars/utils.h"

typedef struct ArenaAllocator {
    Allocator base;
    void* base_ptr;
    size_t size;
    size_t offset;
    size_t prev_offset;
} ArenaAllocator;

static inline uintptr_t next_aligned_ptr(ArenaAllocator* self, size_t align) {
    uintptr_t ptr = (uintptr_t)self->base_ptr + self->offset;

    uintptr_t modulo = ptr % align;
    if (modulo) {
        ptr += (align - modulo);
    }
    return ptr;
}

static void* arena_allocator_alloc(Allocator* self, size_t size) {
    ArenaAllocator* arena = (ArenaAllocator*)self;
    uintptr_t ptr = next_aligned_ptr(arena, alignof(max_align_t));
    uintptr_t next = ptr + size;

    if (next <= (uintptr_t)arena->base_ptr + arena->size) {
        uintptr_t offset = next - (uintptr_t)arena->base_ptr;
        arena->offset = offset;
        arena->prev_offset = offset - size;
        return (void*)ptr;
    }

    // not enough space for `size` bytes
    return 0;
}

static void* arena_allocator_realloc(Allocator* self, void* ptr, size_t size) {
    if (!ptr) {
        return arena_allocator_alloc(self, size);
    }

    ArenaAllocator* arena = (ArenaAllocator*)self;

    size_t prev_size = arena->offset - arena->prev_offset;

    if (arena->base_ptr + arena->prev_offset == ptr) {
        uintptr_t next = (uintptr_t)ptr + size;

        if (next > (uintptr_t)arena->base_ptr + arena->size) {
            return 0;
        }

        arena->offset = next - (uintptr_t)arena->base_ptr;

        if (size > prev_size) {
            memset(ptr + prev_size, 0, size - prev_size);
        }
        return ptr;
    }

    void* new_ptr = arena_allocator_alloc(self, size);
    if (!new_ptr) {
        return new_ptr;
    }
    memmove(new_ptr, ptr, size - prev_size);
    return new_ptr;
}

static void arena_allocator_free(Allocator* self, void* ptr) {
    UNUSED(self);
    UNUSED(ptr);
    /* Do nothing */
}

static AllocatorVTable arena_allocator_vtable = {
    .alloc = arena_allocator_alloc,
    .realloc = arena_allocator_realloc,
    .free = arena_allocator_free,
};

ArenaAllocator arena_allocator_new(size_t arena_size) {
    return (ArenaAllocator){.base = {.vtable = &arena_allocator_vtable},
                            .size = arena_size,
                            .base_ptr = malloc(arena_size)};
}

void arena_allocator_reset(ArenaAllocator* self) {
    self->offset = 0;
    self->prev_offset = 0;
}

void arena_allocator_drop(ArenaAllocator* self) { free(self->base_ptr); }

typedef struct InnerArena {
    ArenaAllocator* arena;
    size_t ckpt_offset;
    size_t ckpt_prev_offset;
} InnerArena;

InnerArena arena_allocator_new_inner(ArenaAllocator* arena) {
    return (InnerArena){
        .arena = arena,
        .ckpt_offset = arena->offset,
        .ckpt_prev_offset = arena->prev_offset,
    };
}

void arena_allocator_drop_inner(ArenaAllocator* arena, InnerArena* inner) {
    arena->prev_offset = inner->ckpt_prev_offset;
    arena->offset = inner->ckpt_offset;
}

#endif
