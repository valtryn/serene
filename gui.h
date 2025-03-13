#ifndef _GUI_C
#define _GUI_C

#include <stddef.h>

#include "allocator.h"

typedef struct Color Color; // R8G8B8A8 (32bit)
typedef struct Vector2 Vector2;
typedef struct Vector3 Vector3;
typedef struct Rectangle Rectangle;

typedef struct Canvas Canvas;
struct Canvas {
	int      width;
	int      height;
	size_t   pitch;
        uint32_t *buffer;
};

// DRAW BASIC SHAPES
int gui_canvas_init(Canvas *canvas, int width, int height, Allocator *alloc);
void gui_draw_pixel(Canvas *canvas, int x, int y, Color color);
void gui_draw_rectangle(Canvas *canvas, int x, int y, int width, int height, Color color);
void gui_draw_rectangle_outline(Canvas *canvas, int x, int y, int width, int height, Color color);
void gui_draw_line(Canvas *canvas, int sx, int sy, int ex, int ey, Color border_color);
void gui_reset(Canvas *canvas);
void gui_draw_circle(Canvas *canvas, int cx, int cy, int radius, Color color);

struct Color {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
};

struct Vector2 {
        float x;
        float y;
};

struct Vector3 {
        float x;
        float y;
        float z;
};

struct Rectangle {
        float x;
        float y;
        float width;
        float height;
};

// NOTE: use catppuccin colorscheme for now
#define SERENE_ROSEWATER (Color){245, 224, 220, 255} /* #f5e0dc */
#define SERENE_FLAMINGO  (Color){242, 205, 205, 255} /* #f2cdcd */
#define SERENE_PINK 	 (Color){245, 194, 231, 255} /* #f5c2e7 */
#define SERENE_MAUVE 	 (Color){203, 166, 247, 255} /* #cba6f7 */
#define SERENE_RED 	 (Color){243, 139, 168, 255} /* #f38ba8 */
#define SERENE_MAROON 	 (Color){235, 160, 172, 255} /* #eba0ac */
#define SERENE_PEACH 	 (Color){250, 179, 135, 255} /* #fab387 */
#define SERENE_YELLOW 	 (Color){249, 226, 175, 255} /* #f9e2af */
#define SERENE_GREEN 	 (Color){166, 227, 161, 255} /* #a6e3a1 */
#define SERENE_TEAL 	 (Color){148, 226, 213, 255} /* #94e2d5 */
#define SERENE_SKY 	 (Color){137, 220, 235, 255} /* #89dceb */
#define SERENE_SAPPHIRE  (Color){116, 199, 236, 255} /* #74c7ec */
#define SERENE_BLUE 	 (Color){137, 180, 250, 255} /* #89b4fa */
#define SERENE_LAVENDER  (Color){180, 190, 254, 255} /* #b4befe */
#define SERENE_TEXT 	 (Color){205, 214, 244, 255} /* #cdd6f4 */
#define SERENE_SUBTEXT1  (Color){186, 194, 222, 255} /* #bac2de */
#define SERENE_SUBTEXT0  (Color){166, 173, 200, 255} /* #a6adc8 */
#define SERENE_OVERLAY2  (Color){147, 153, 178, 255} /* #9399b2 */
#define SERENE_OVERLAY1  (Color){127, 132, 156, 255} /* #7f849c */
#define SERENE_OVERLAY0  (Color){108, 112, 134, 255} /* #6c7086 */
#define SERENE_SURFACE2  (Color){88,  91,  112, 255} /* #585b70 */
#define SERENE_SURFACE1  (Color){69,  71,  90,  255} /* #45475a */
#define SERENE_SURFACE0  (Color){49,  50,  68,  255} /* #313244 */
#define SERENE_BASE 	 (Color){30,  30,  46,  255} /* #1e1e2e */
#define SERENE_MANTLE 	 (Color){24,  24,  37,  255} /* #181825 */
#define SERENE_CRUST 	 (Color){17,  17,  27,  255} /* #11111b */


#endif /* _GUI_C */
