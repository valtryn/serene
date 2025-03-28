#include <stdio.h>

#include "base/allocator.h"
#include "base/str.h"
#include "color.h"
#include "core.h"
#include "gfx.h"

int main(void)
{
	int width = 1280;
	int height = 720;

	Allocator arena = {0};
	arena_allocator_init(&arena, megabytes(10));
	init_window(str_make("Serene", &arena), width, height, &arena);
	set_fps(-1);
	while(!should_close()) {
		begin();
		clear_background(SERENE_CLEAR_BLACK);
		/* gfx_draw_rectangle(0, 0, 250, 250, SERENE_BLUE); */
		printf("fps: %d\n", get_fps());
		end();
	}
	allocator_deinit(&arena);
	return 0;
}

