#include <stdio.h>

#include "base/allocator.h"
#include "base/str.h"
#include "color.h"
#include "components.h"
#include "core.h"
#include "gfx.h"


int main(void)
{
	int width = 1280;
	int height = 720;

	Allocator arena = {0};
	arena_allocator_init(&arena, megabytes(10));
	init_window(str_make("Serene", &arena), width, height, &arena);
	set_fps(60);
	while(!should_close()) {
		BEGIN();
		{
			clear_background(COLOR_BLACK);
			if(button(100, 100, 250, 200))
				gfx_draw_rectangle(20, 20, 100, 100, COLOR_GREEN);
		}
		END();
	}
	deinit_window();
	allocator_deinit(&arena);
	return 0;
}

/* int main(void) */
/* { */
/* 	Allocator arena = {0}; */
/* 	arena_allocator_init(&arena, megabytes(10)); */

/* 	Allocator *arena_heap = arena.alloc(sizeof(Allocator), arena.ctx); */
/* 	arena_allocator_init(arena_heap, megabytes(1)); */

/* 	allocator_deinit(arena_heap); */
/* 	allocator_deinit(&arena); */

/* } */
