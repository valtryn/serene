#ifndef BOX_H
#define BOX_H

#include <stdbool.h>
#include "core.h"

typedef struct Box Box;
typedef struct State State;

struct State {
	int hovered;
	int clicked;
};

struct Box {
	Vector2 pos;

	int width;
	int height;
	// TODO: add support for this later
	/* int padding; */
	/* int margin; */

	// color
	Color background_color;

	// border
	int   border_width;
	Color border_color_top;
	Color border_color_bottom;
	Color border_color_left;
	Color border_color_right;

	State state;
};

bool box_is_hovered(Box *box);
void draw_button(void);

#endif  /* BOX_H */
