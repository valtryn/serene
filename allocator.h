#ifndef _ALLOCATOR_H
#define _ALLOCATOR_H

#include <stddef.h>
#include <stdint.h>
#include <pthread.h>

#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT (2*sizeof(void *))
#endif

#define DEFAULT_ALLOC_SIZE 1024

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
        void *ctx;
        enum  AllocatorType type;
        void *(*alloc)(size_t size, void *ctx);
        void *(*realloc)(void *ptr, size_t size, void *ctx);
        void  (*free)(void *ptr, void *ctx);
};


void allocator_deinit(Allocator *allocator);

// GENERAL PURPOSE ALLOCATOR

int general_allocator_init(Allocator *allocator);
void *gpa_alloc(size_t size, void *ctx);
void *gpa_realloc(void *ptr, size_t size, void *ctx);
void  gpa_free(void *ptr, void *ctx);

// TRACKING ALLOCATOR

typedef struct TrackingAllocator TrackingAllocator;
struct TrackingAllocator {
        size_t    len;
        size_t    cap;
        uintptr_t *offsets;
};

int tracking_allocator_init(Allocator *allocator);
void *ta_alloc(size_t size, void *ctx);
void *ta_realloc(void *ptr, size_t size, void *ctx);
void ta_free(void *ptr, void *ctx);

// ARENA ALLOCATOR
typedef struct Arena Arena;
struct Arena {
        unsigned char *buffer;
        size_t         cap;
	size_t         prev_offset;
	size_t         curr_offset;

        // stats
        uintptr_t *relative_offsets;
        size_t    offset_cap;
        size_t    offset_len;

        // multithread support
        pthread_mutex_t mutex;
};

int arena_allocator_init(Allocator *allocator, size_t size);
void *arena_alloc(size_t size, void *ctx);
void arena_free(void *ptr, void *ctx);
void *arena_realloc(void *ptr, size_t size, void *ctx);
size_t arena_get_block_size(uintptr_t ptr, Arena *arena);
uintptr_t arena_abs_offset(Arena *arena, uintptr_t relative_offset);

// 
// TODO: BRIDGE ALLOC
//
void *bridge_alloc(size_t size);
void *bridge_calloc(size_t element_size, size_t size);
void *bridge_realloc(void *ptr, size_t size);
void *bridge_free(size_t size);

#endif /* _ALLOCATOR_H */
