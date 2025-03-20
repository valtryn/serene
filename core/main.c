#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "allocator.h"
#include "ui.h"

static const int width  = 1280;
static const int height = 720;

int main(void)
{
	Allocator arena = {0};
	arena_allocator_init(&arena, megabytes(10));
	
	srn_init_window("serene", width, height, &arena);
	int quit = 0;

	int x;
	int y;
	srn_set_fps(60);
	while (!quit) {
		srn_begin();

		srn_clear_background(SERENE_CRUST);
		Vector2 pos = srn_get_mouse_position();
		printf("x: %f | y: %f\n", pos.x, pos.y);
		srn_draw_rectangle(pos.x-50, pos.y-50, 100, 100, SERENE_GREEN);

		srn_end();
		printf("fps: %d\n", srn_get_fps());
	}
}
