#include <stdio.h>
#include "ui.h"
#include "menubar.h"

static const int width  = 1280;
static const int height = 720;

void render_thumbnail_panel(void)
{
	int panel_width = 1240;
	int panel_height = 500;

	Vector2 panel = { .x = 20, .y = 120 };
	srn_draw_rectangle_outline(panel.x, panel.y, panel_width, panel_height, SERENE_BLUE);

	int thumbnail_width = 160;
	int thumbnail_height = 225;
	Vector2 thumbnail_origin = { .x = 35, .y = 135 };
	int accum = 0;
	for (int i = 0; i < 7;i++) {
		srn_draw_rectangle_outline(thumbnail_origin.x + accum, thumbnail_origin.y, thumbnail_width, thumbnail_height, SERENE_BLUE);
		accum += thumbnail_width + 15;
	}
	accum = 0;
	thumbnail_origin.y += thumbnail_height + 15;
	for (int i = 0; i < 7;i++) {
		srn_draw_rectangle_outline(thumbnail_origin.x + accum, thumbnail_origin.y, thumbnail_width, thumbnail_height, SERENE_BLUE);
		accum += thumbnail_width + 15;
	}
}

int main(void)
{
	Allocator arena = {0};
	arena_allocator_init(&arena, megabytes(10));
	
	srn_init_window("serene", width, height, &arena);
	int quit = 0;

	int x;
	int y;
	srn_set_fps(0);
	while (!quit) {
		srn_begin();

		srn_clear_background(SERENE_CRUST);

		menu_bar();
		/* render_thumbnail_panel(); */

		printf("fps: %d\n", srn_get_fps());
		srn_end();
	}
}
