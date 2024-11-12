// TODO: 
//	- page allocator
//	- tracking allocator
//	- better error handling
//	- rename library to mem
// Resources:
//		- https://zig.guide/standard-library/allocators
//		- https://www.gingerbill.org/series/memory-allocation-strategies/
//		- https://bytesbeneath.com/articles
#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT (2*sizeof(void *))
#endif

#define bytes(size)     ((size_t)(size) * (1))
#define kilobytes(size) ((size_t)(size) * (1 << 10))
#define megabytes(size) ((size_t)(size) * (1 << 20))
#define gigabytes(size) ((size_t)(size) * (1 << 30))

enum AllocatorType {
	ARENA_ALLOCATOR = 0,
	GENERAL_PURPOSE_ALLOCATOR,
};

typedef struct Allocator Allocator;
struct Allocator {
	void *(*alloc)(size_t size, void *ctx);
	void (*free)(void *ptr, void *ctx);
	void *(*realloc)(void *ptr, size_t old_size, size_t size, void *ctx);
	void *ctx;
	enum AllocatorType type;
};

typedef struct GenAlloc GenAlloc;
struct GenAlloc {
	size_t     len;
	size_t     cap;
	uintptr_t *ptr;
};

typedef struct Arena Arena;
struct Arena {
	unsigned char *buffer;
	size_t         buf_len;
	size_t         prev_offset;
	size_t         curr_offset;
	size_t         commited; // TODO: implement
};

void allocator_init(Allocator *allocator, enum AllocatorType type, size_t initial_size);
void allocator_deinit(Allocator *allocator);
// Arena Allocator
void  arena_init(Arena *arena, void *buffer, size_t size);
void* arena_alloc(size_t size, void *ctx);
void* arena_resize(void *old_memory, size_t old_size, size_t new_size, void *ctx);
void  arena_free(void *ptr, void *ctx);
// General Purpose Allocator (malloc/free)
// TODO: 
//	- track alloc/free - implemented but shitty
//	- unified free for all allocated data - implemented but shitty
//
// TODO: maybe rename to ga_(func)?
void  gen_init(GenAlloc *ga, size_t size);
void  gen_deinit(GenAlloc *ga);
void *gen_alloc(size_t size, void *ctx);
void  gen_free(void *ptr, void *ctx);
void  gen_free_all(Allocator *allocator);
void *gen_realloc(void *ptr, size_t old_size, size_t size, void *ctx);
// utility functions
uintptr_t align_forward(uintptr_t ptr, size_t align);



#endif /*ALLOCATOR_H */
