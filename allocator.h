#ifndef _ALLOCATOR_H
#define _ALLOCATOR_H

#include <stddef.h>
#include <stdint.h>

#define NO_SIZE 0

#define bytes(size)     ((size_t)(size) * (1))
#define kilobytes(size) ((size_t)(size) * (1 << 10))
#define megabytes(size) ((size_t)(size) * (1 << 20))
#define gigabytes(size) ((size_t)(size) * (1 << 30))

enum AllocatorType {
        ARENA_ALLOCATOR = 0,
        GENERAL_PURPOSE_ALLOCATOR,
        TRACKING_ALLOCATOR,
};

typedef struct Allocator Allocator;
struct Allocator {
        void* (*alloc)(size_t size, void *ctx);
        void* (*realloc)(void *ptr, size_t size, size_t old_size, void *ctx);
        void  (*free)(void *ptr, void *ctx);
        void* ctx;
        enum  AllocatorType type;
};


void allocator_deinit(Allocator *allocator);

// GENERAL PURPOSE ALLOCATOR

int general_allocator_init(Allocator *allocator);
void* gpa_alloc(size_t size, void *ctx);
void* gpa_realloc(void *ptr, size_t size, size_t old_size, void *ctx);
void  gpa_free(void *ptr, void *ctx);

// TRACKING ALLOCATOR

typedef struct TrackingAllocator TrackingAllocator;
struct TrackingAllocator {
        size_t    len;
        size_t    cap;
        uintptr_t *container;
};

int tracking_allocator_init(Allocator *allocator);
void *ta_alloc(size_t size, void *ctx);
void *ta_realloc(void *ptr, size_t size, size_t old_size, void *ctx);
void ta_free(void *ptr, void *ctx);

#endif /* _ALLOCATOR_h */
