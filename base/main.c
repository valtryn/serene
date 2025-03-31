#include <stdio.h>

#include "base/allocator.h"
#include "base/str.h"
#include "color.h"
#include "components.h"
#include "core.h"
#include "gfx.h"
#include "keycode.h"

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
			if(button(100, 100, 250, 200)) {
			}
			/* gfx_draw_rectangle(pos.x - 50, pos.y - 50, 100, 100, COLOR_CRUST); */
			/* printf("fps: %d\n", get_fps()); */
			/* printf("mouse pos: x: %f - x: %f\n", pos.x, pos.y); */
		}
		END();
	}
	allocator_deinit(&arena);
	return 0;
}

