#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "allocator.h"
#include "ui.h"

static const int width  = 1283;
static const int height = 727;

int main(void)
{
	Allocator arena = {0};
	arena_allocator_init(&arena, megabytes(10));
	
	srn_init_window("serene", width, height, &arena);
	int quit = 0;

	Vector2 pos = {0};
	srn_set_fps(600);
	while (!quit) {
		srn_begin();

		ui_draw_rectangle(width/2, height/2, 100, 100, SERENE_GREEN);
		srn_clear_background(SERENE_CRUST);
		srn_end();
		printf("fps: %d\n", srn_get_fps());
	}
}
