#include <stdbool.h>
#include <stdio.h>

#include "core.h"
#include "gfx.h"
#include "box.h"

void box_init(Box *box)
{
	box->pos    = VEC2(0, 0);

	box->width  = 100;
	box->height = 100;

	box->background_color    = SERENE_CRUST;

	box->border_width        = 1;
	box->border_color_top    = SERENE_OVERLAY2;
	box->border_color_bottom = SERENE_OVERLAY2;
	box->border_color_left   = SERENE_OVERLAY2;
	box->border_color_right  = SERENE_OVERLAY2;

	box->state.hovered = false;
	box->state.clicked = false;
}

/* void draw_button(void) */
/* { */
/* 	static int drag = 0; */
/* 	static int px = 0; */
/* 	static int py = 0; */
/* 	Box box = {0}; */
/* 	box_init(&box); */
/* 	box.pos.x = px; */
/* 	box.pos.y = py; */
/* 	box.state.hovered = box_is_hovered(&box); */
/* 	if (box.state.hovered && global_ctx.Mouse.State.press == 1) { */
/* 		drag = 1; */
/* 	} */
/* 	if (global_ctx.Mouse.State.press == 0) { */
/* 		drag = 0; */
/* 	} */
/* 	if (drag == 1) { */
/* 		Vector2 pos = get_mouse_position(); */
/* 		px = pos.x-(box.height/2); */
/* 		py = pos.y-(box.width/2); */
/* 	} */
/* 	// draw background */
/* 	gfx_draw_rectangle(box.pos.x, box.pos.y, box.width, box.height, box.background_color); */
/* 	// top */
/* 	gfx_draw_line(box.pos.x, box.pos.y, box.pos.x + box.width, box.pos.y, box.border_color_top); */
/* 	// right */
/* 	gfx_draw_line(box.pos.x + box.width, box.pos.y, box.pos.x + box.width, box.pos.y + box.height, box.border_color_right); */
/* 	// bottom */
/* 	gfx_draw_line(box.pos.x, box.pos.y + box.height, box.pos.x + box.width, box.pos.y + box.height, box.border_color_bottom); */
/* 	// left */
/* 	gfx_draw_line(box.pos.x, box.pos.y, box.pos.x, box.pos.y + box.height, box.border_color_left); */
/* } */

bool box_is_hovered(Box *box)
{
	Vector2 pos = get_mouse_position();
	if (pos.x >= box->pos.x && pos.x <= box->pos.x + box->width && pos.y >= box->pos.y && pos.y <= box->pos.y + box->height)
		return true;
	else
		return false;
}

