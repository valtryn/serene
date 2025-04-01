#include <X11/Xlib.h>
#include <stdbool.h>

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
	set_border_side(c, BORDER_TOP | BORDER_RIGHT | BORDER_BOTTOM | BORDER_LEFT, false);
	set_border_position(c, BORDER_POSITION_OUTSIDE);

	return c;
}

/* Rectangle default_rect(void) */
/* { */
/* 	return (Rectangle){ */
/* 		.x      = 0, */
/* 		.y      = 0, */
/* 		.width  = 100, */
/* 		.height = 100 */
/* 	}; */
/* } */

void draw_border(Component *c)
{
	if (c->border.mask  == BORDER_NONE)
		return;
	if (c->border.mask & BORDER_TOP)
		gfx_draw_line(VEC2(c->x, c->y), VEC2(c->x + c->width, c->y), c->border.color);
	if (c->border.mask & BORDER_RIGHT)
		gfx_draw_line(VEC2(c->x + c->width, c->y),  VEC2(c->x + c->width, c->y + c->height), c->border.color);
	if (c->border.mask & BORDER_BOTTOM)
		gfx_draw_line(VEC2(c->x, c->y + c->height), VEC2(c->x + c->width, c->y + c->height), c->border.color);
	if (c->border.mask & BORDER_LEFT)
		gfx_draw_line(VEC2(c->x, c->y + c->height), VEC2(c->x, c->y), c->border.color);
}

void set_border_side(Component *c, U32 mask, bool overwrite)
{
	if (mask == BORDER_NONE) {
		c->border.mask &= ~(BORDER_TOP | BORDER_RIGHT | BORDER_BOTTOM | BORDER_LEFT);
		return;
	}
	if (overwrite)
		c->border.mask &= ~(BORDER_TOP | BORDER_RIGHT | BORDER_BOTTOM | BORDER_LEFT);
	c->border.mask |= mask;
}

// TODO: make sure there is only 1 position passed
void set_border_position(Component *c, U32 mask)
{
	c->border.mask &= ~(BORDER_POSITION_CENTER | BORDER_POSITION_INSIDE | BORDER_POSITION_OUTSIDE);
	if (mask & BORDER_POSITION_CENTER) {
		c->border.mask |= BORDER_POSITION_CENTER;
		return;
	}
	if (mask & BORDER_POSITION_INSIDE) {
		c->border.mask |= BORDER_POSITION_INSIDE;
		return;
	}
	if (mask & BORDER_POSITION_OUTSIDE) {
		c->border.mask |= BORDER_POSITION_OUTSIDE;
		return;
	}
}

bool button(int x, int y, int width, int height)
{
	Vector2 pos = get_mouse_position();
	bool hovered = mouse_in_rect(pos, (Rectangle){ .x = x, .y = y, .width = width, .height = height});
	Component *c = create_component();
	c->x = x;
	c->y = y;
	c->width = width;
	c->height = height;
	c->state.is_hovered = hovered;
	c->state.is_clicked = (c->state.is_hovered && is_mouse_btn_press(SRN_BTN_LEFT));
	if (c->state.is_hovered) 
		c->border.color = COLOR_RED, c->height += 199, c->width += 199;
	draw_border(c);
	return c->state.is_clicked;
}


