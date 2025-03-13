#include "../allocator.h"
#include "../ds.h"
#include <pthread.h>
#include <stddef.h>
#include <stdio.h>

#define SIZE 100000
void* threadFunction(void* arg) {
	Allocator arena = *(Allocator*)arg;
	Array arr = {0};
	array_init(&arr, sizeof(size_t), 2, &arena);

	for (size_t i = 0; i < SIZE; i++) {
		array_append(&arr, &i);
	}

	/* for (size_t i = 0; i < SIZE; i++) { */
	/* 	size_t elem = *(int*)array_at(&arr, i); */
	/* 	printf("thread1: elem: %zu\n", elem); */
	/* } */

	return NULL;
}

void* threadFunction2(void* arg) {
	Allocator arena = *(Allocator*)arg;
	Array arr = {0};
	array_init(&arr, sizeof(int), 2, &arena);

	for (int i = 0; i < SIZE; i++) {
		array_append(&arr, &i);
	}

	/* for (int i = 0; i < SIZE; i++) { */
	/* 	int elem = *(int*)array_at(&arr, i); */
	/* 	printf("thread2: elem: %d\n", elem); */
	/* } */
	return NULL;
}
int main(void)
{
	Allocator arena = {0};
	arena_allocator_init(&arena, megabytes(1000));
	pthread_t thread;
	pthread_create(&thread, NULL, threadFunction, &arena);
	pthread_t thread2;
	pthread_create(&thread2, NULL, threadFunction2, &arena);
	pthread_join(thread, NULL);
	pthread_join(thread2, NULL);
	Array arr = {0};
	array_init(&arr, sizeof(int), 2, &arena);

	for (int i = 0; i < SIZE; i++) {
		array_append(&arr, &i);
	}
		Array arr2 = {0};
	array_init(&arr2, sizeof(size_t), 2, &arena);

	for (size_t i = 0; i < SIZE; i++) {
		array_append(&arr2, &i);
	}
	allocator_deinit(&arena);
}





