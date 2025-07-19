#ifndef CARS_STRING_H
#define CARS_STRING_H

#include <stdio.h>
#include <string.h>

#include "cars/allocator.h"
#include "cars/errors.h"
#include "cars/types.h"

typedef struct String {
    Allocator* allocator;
    u8* bytes;
    size_t capacity;
    size_t len;
} String;

String string_new(Allocator* allocator) {
    return (String){.allocator = allocator};
}

void* string_reserve(String* s, size_t additional_capacity) {
    s->bytes = s->allocator->realloc(
        s->allocator, s->bytes, (s->capacity + additional_capacity) * sizeof(u8)
    );
    if (!s->bytes) {
        MEMERR();
    }
    s->capacity += additional_capacity;
    return s->bytes;
}

String string_new_with_capacity(Allocator* allocator, size_t capacity) {
    String s = {
        .allocator = allocator,
    };

    string_reserve(&s, capacity);
    return s;
}

String string_from_string(Allocator* allocator, String const* s) {
    String new_s = string_new_with_capacity(allocator, s->len);
    new_s.len = s->len;
    memcpy(new_s.bytes, s->bytes, s->len);
    return new_s;
}

String string_from_cstr(Allocator* allocator, char const* cstr) {
    size_t len = strlen(cstr);
    String s = string_new_with_capacity(allocator, len);
    s.len = len;
    memcpy(s.bytes, cstr, len);
    return s;
}

String string_concat(Allocator* allocator, String const* s1, String const* s2) {
    String s = string_new_with_capacity(allocator, s1->len + s2->len);
    memcpy(s.bytes, s1->bytes, s1->len);
    memcpy(s.bytes + s1->len * sizeof(u8), s2->bytes, s2->len);
    s.len = s1->len + s2->len;
    return s;
}

void string_push_cstr(String* s, char const* cstr) {
    size_t cstr_len = strlen(cstr);
    if (s->len + cstr_len > s->capacity) {
        string_reserve(s, cstr_len - (s->capacity - s->len));
    }
    memcpy(s->bytes + s->len * sizeof(u8), cstr, cstr_len);
    s->len += cstr_len;
}

void string_push_string(String* s, String const* str) {
    if (s->len + str->len > s->capacity) {
        string_reserve(s, str->len - (s->capacity - s->len));
    }
    memcpy(s->bytes + s->len * sizeof(u8), str->bytes, str->len);
    s->len += str->len;
}

String string_ascii_lowercase(Allocator* allocator, String const* s) {
    String s_lowercase = string_new_with_capacity(allocator, s->len);
    for (size_t i = 0; i < s->len; i++) {
        if (s->bytes[i] >= 'A' && s->bytes[i] <= 'Z') {
            s_lowercase.bytes[i] = s->bytes[i] - ('A' - 'a');
        } else {
            s_lowercase.bytes[i] = s->bytes[i];
        }
    }
    s_lowercase.len = s->len;
    return s_lowercase;
}

String string_ascii_uppercase(Allocator* allocator, String const* s) {
    String s_uppercase = string_new_with_capacity(allocator, s->len);
    for (size_t i = 0; i < s->len; i++) {
        if (s->bytes[i] >= 'a' && s->bytes[i] <= 'z') {
            s_uppercase.bytes[i] = s->bytes[i] + ('A' - 'a');
        } else {
            s_uppercase.bytes[i] = s->bytes[i];
        }
    }
    s_uppercase.len = s->len;
    return s_uppercase;
}

void string_drop(String* s) { s->allocator->free(s->allocator, s->bytes); }

void string_print(String const* s) {
    fwrite(s->bytes, sizeof(u8), s->len, stdout);
    fflush(stdout);
}

#endif
