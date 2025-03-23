#include "base/allocator.h"
#include "base/str.h"
#include "box.h"
#include "color.h"
#include "gfx.h"
#include "ui/core.h"
#include <stdio.h>


int main(void)
{
	Allocator arena = {0};
	arena_allocator_init(&arena, megabytes(10));
	init_window(str_make("serene", &arena), 1280, 720, &arena);
	Event event;
	set_fps(60);
	int quit = 0;
	int x = 0;
	int y = 0;
	while (!quit) {
		begin();
		gfx_clear_background(SERENE_CLEAR_BLACK);
		draw_button();
		/* gfx_draw_pixel(100, 100, SERENE_GREEN); */
		printf("fps: %d\n", get_fps());
		end();
	}
	return 0;
}
