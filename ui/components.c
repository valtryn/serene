#include <X11/Xlib.h>
#include <stdio.h>

#include "components.h"
#include "base/allocator.h"
#include "base/types.h"
#include "color.h"
#include "core.h"
#include "gfx.h"
#include "keycode.h"

Component *create_component(void)
{
	Allocator *alloc = CONTEXT.frame_allocator;
	Component *c = alloc->alloc(sizeof(Component), alloc->ctx); // WHY DOES THIS SEGFAULTS
	// DEFAULT
	c->x             = 0;
	c->y             = 0;
	c->width         = 100;
	c->height        = 100;
	c->padding       = 0;
	c->margin        = 0;

	c->border.width  = 1;
	c->border.color  = COLOR_BLUE;
	set_border_active(c, BORDER_TOP | BORDER_RIGHT | BORDER_BOTTOM | BORDER_LEFT);
	set_border_position(c, BORDER_POSITION_OUTSIDE);

	return c;
}

void draw_border(Component *c)
{
	if (c->border.mask & BORDER_TOP)
		gfx_draw_line(VEC2(c->x, c->y), VEC2(c->x + c->width, c->y), c->border.color);
	if (c->border.mask & BORDER_RIGHT)
		gfx_draw_line(VEC2(c->x + c->width, c->y),  VEC2(c->x + c->width, c->y + c->height), c->border.color);
	if (c->border.mask & BORDER_BOTTOM)
		gfx_draw_line(VEC2(c->x, c->y + c->height), VEC2(c->x + c->width, c->y + c->height), c->border.color);
	if (c->border.mask & BORDER_LEFT)
		gfx_draw_line(VEC2(c->x, c->y + c->height), VEC2(c->x, c->y), c->border.color);
}

void set_border_active(Component *c, U32 mask)
{
	/* c->border.mask &= ~(BORDER_TOP | BORDER_RIGHT | BORDER_BOTTOM | BORDER_LEFT); */
	c->border.mask |= mask;
}

// TODO: make sure there is only 1 position passed
void set_border_position(Component *c, U32 mask)
{
	c->border.mask &= ~(BORDER_POSITION_CENTER | BORDER_POSITION_INSIDE | BORDER_POSITION_OUTSIDE);
	c->border.mask |= mask;
}

bool button(int x, int y, int width, int height) {
	static float threshold = 0;
	Component *c = create_component();
	c->x = x;
	c->y = y;
	c->width = width;
	c->height = height;
	Vector2 pos = get_mouse_position();
	c->state.is_hovered = mouse_in_rect(pos, c);
	c->state.is_clicked = (c->state.is_hovered && is_mouse_btn_press(SRN_BTN_LEFT));
	if (is_mouse_btn_down(SRN_BTN_LEFT)) {
		threshold += 25 * get_frame_time();
		if (threshold >= 10)
			c->state.is_dragged = true;
	}
	if (c->state.is_dragged && is_mouse_btn_released(SRN_BTN_LEFT))
		threshold = 0, c->state.is_dragged = false;
	if (c->state.is_dragged) c->border.color = COLOR_GREEN, c->x = pos.x, c->y = pos.y;
	draw_border(c);
	return c->state.is_clicked;
}

bool mouse_in_rect(Vector2 pos, Component *rect)
{
	return pos.x >= rect->x && 
	       pos.x <= rect->x + rect->width && 
	       pos.y >= rect->y && 
	       pos.y <= rect->y + rect->height;
}

