#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <stdbool.h>
#include "base/types.h"
#include "color.h"
#include "core.h"

typedef enum BorderPosition {
	CENTER = 0,
	INSIDE,
	OUTSIDE
} BorderPosition;

static const U32 BORDER_TOP      = 1 << 0;
static const U32 BORDER_RIGHT    = 1 << 1;
static const U32 BORDER_BOTTOM   = 1 << 2;
static const U32 BORDER_LEFT     = 1 << 3;

 // NOTE: which direction the border expands when width > 1
static const U32 BORDER_POSITION_CENTER  = 1 << 4;
static const U32 BORDER_POSITION_INSIDE  = 1 << 5;
static const U32 BORDER_POSITION_OUTSIDE = 1 << 6;

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
void set_border_active(Component *c, U32 mask);
void set_border_position(Component *c, U32 mask);
bool mouse_in_rect(Vector2 pos, Component *rect);


#endif // COMPONENTS_H
