#pragma once

#include <stdlib.h>

// used to define type-safe dynamic arrays
#define DEFINE_ARRAY_TYPE(Name, Type)                                          \
    typedef struct {                                                           \
        Type *data;                                                            \
        int capacity;                                                          \
        int count;                                                             \
    } Name;                                                                    \
                                                                               \
    static inline void Name##_init(Name *a) {                                  \
        a->data = NULL;                                                        \
        a->capacity = 0;                                                       \
        a->count = 0;                                                          \
    }                                                                          \
                                                                               \
    static inline void Name##_add(Name *a, Type item) {                        \
        if (a->count >= a->capacity) {                                         \
            a->capacity = a->capacity ? a->capacity * 2 : 1;                   \
            a->data = realloc(a->data, a->capacity * sizeof(Type));            \
        }                                                                      \
        a->data[a->count] = item;                                              \
        a->count++;                                                            \
    }                                                                          \
                                                                               \
    static inline void Name##_clear(Name *a) {                                 \
        a->count = 0;                                                          \
    }                                                                          \
                                                                               \
    static inline void Name##_free(Name *a) {                                  \
        if (a->data) { free(a->data); }                                        \
    }
