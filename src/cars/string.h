#ifndef CARS_STRING_H_
#define CARS_STRING_H_

#include <stdio.h>
#include <string.h>

#include "cars/allocator.h"
#include "cars/errors.h"
#include "cars/types.h"

typedef struct String {
    Allocator* allocator;
    u8* data;
    size_t capacity;
    size_t len;
} String;

String String_new(Allocator* allocator) {
    return (String){.allocator = allocator};
}

String String_from_String(Allocator* allocator, String* s) {
    String new_s = {
        .allocator = allocator,
        .capacity = s->len,
        .len = s->len,
        .data = allocator->alloc(allocator, s->len)
    };

    memcpy(new_s.data, s->data, s->len);
    return new_s;
}

String String_from_cstr(Allocator* allocator, char* cstr) {
    size_t len = strlen(cstr);
    String s = {
        .allocator = allocator,
        .capacity = len,
        .len = len,
        .data = allocator->alloc(allocator, len)
    };

    memcpy(s.data, cstr, len);
    return s;
}

void* String_reserve(String* s, size_t additional_capacity) {
    s->data = s->allocator->realloc(
        s->allocator, s->data, (s->capacity + additional_capacity) * sizeof(u8)
    );
    if (!s->data) {
        cars_errorno = Cars_ENoMem;
        return 0;
    }
    s->capacity += additional_capacity;
    return s->data;
}

void String_push_cstr(String* s, char* cstr) {
    size_t cstr_len = strlen(cstr);
    if (s->len + cstr_len > s->capacity) {
        void* reserved = String_reserve(s, cstr_len - (s->capacity - s->len));
        if (!reserved) {
            cars_errorno = Cars_ENoMem;
            return;
        }
    }
    memcpy(s->data + s->len * sizeof(u8), cstr, cstr_len);
    s->len += cstr_len;
}

void String_push_String(String* s, String* str) {
    if (s->len + str->len > s->capacity) {
        void* reserved = String_reserve(s, str->len - (s->capacity - s->len));
        if (!reserved) {
            cars_errorno = Cars_ENoMem;
            return;
        }
    }
    memcpy(s->data + s->len * sizeof(u8), str->data, str->len);
    s->len += str->len;
}

String String_ascii_lowercase(Allocator* a, String* s) {
    String s_lowercase = {
        .allocator = a,
    };
    void* reserved = String_reserve(&s_lowercase, s->len);
    if (!reserved) {
        return s_lowercase;
    }

    for (size_t i = 0; i < s->len; i++) {
        if (s->data[i] >= 'A' && s->data[i] <= 'Z') {
            s_lowercase.data[i] = s->data[i] - ('A' - 'a');
        } else {
            s_lowercase.data[i] = s->data[i];
        }
    }
    s_lowercase.len = s->len;
    return s_lowercase;
}

String String_ascii_uppercase(Allocator* a, String* s) {
    String s_uppercase = {
        .allocator = a,
    };
    void* reserved = String_reserve(&s_uppercase, s->len);
    if (!reserved) {
        return s_uppercase;
    }

    for (size_t i = 0; i < s->len; i++) {
        if (s->data[i] >= 'a' && s->data[i] <= 'z') {
            s_uppercase.data[i] = s->data[i] + ('A' - 'a');
        } else {
            s_uppercase.data[i] = s->data[i];
        }
    }
    s_uppercase.len = s->len;
    return s_uppercase;
}

void String_drop(String* s) { s->allocator->free(s->allocator, s->data); }

void String_print(String* s) {
    fwrite(s->data, sizeof(u8), s->len, stdout);
    fflush(stdout);
}

#endif
