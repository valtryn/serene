#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <stdbool.h>
#include "base/types.h"
#include "color.h"
#include "core.h"

#define BORDER_NONE   (U32)0
#define BORDER_TOP    (U32)1 << 1
#define BORDER_RIGHT  (U32)1 << 2
#define BORDER_BOTTOM (U32)1 << 3
#define BORDER_LEFT   (U32)1 << 4
#define BORDER_POSITION_CENTER  (U32)1 << 5
#define BORDER_POSITION_INSIDE  (U32)1 << 6
#define BORDER_POSITION_OUTSIDE (U32)1 << 7

typedef struct Component Component;
struct Component {
	int x, y;
	int width;
	int height;
	int padding;
	int margin;

	struct border {
		U16   width;
		U32   mask;
		Color color;
	} border;

	struct state {
		bool is_hovered;
		bool is_clicked;
		bool is_dragged;
	} state;

	/* Component *parent; */
	/* Component **children; */
};

bool button(int x, int y, int width, int height);

// UTILITY FUNCTIONS
void set_border_side(Component *c, U32 mask, bool overwrite);
void set_border_position(Component *c, U32 mask);
bool mouse_in_rect(Vector2 pos, Rectangle rect);


#endif // COMPONENTS_H
