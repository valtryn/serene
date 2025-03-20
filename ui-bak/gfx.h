#ifndef _GFX_H
#define _GFX_H

#include <stddef.h>
#include <ds.h>

typedef struct Color Color; // R8G8B8A8 (32bit)
typedef struct Vector2 Vector2;
typedef struct Vector3 Vector3;
typedef struct Rectangle Rectangle;

typedef struct Canvas Canvas;
struct Canvas {
	int        width;
	int        height;
	size_t     pitch;
        uint32_t  *buffer;
};

// DRAW BASIC SHAPES
/* void gfx_canvas_init(Canvas *canvas, SRN_Surface *surface); */
void gfx_draw_pixel(Canvas *canvas, int x, int y, Color color);
void gfx_draw_rectangle(Canvas *canvas, int x, int y, int width, int height, Color color);
void gfx_draw_rectangle_outline(Canvas *canvas, int x, int y, int width, int height, Color color);
void gfx_draw_line(Canvas *canvas, int sx, int sy, int ex, int ey, Color border_color);
void gfx_clear(Canvas *canvas);
void gfx_draw_circle(Canvas *canvas, int cx, int cy, int radius, Color color);
void gfx_draw_circle_outline(Canvas *canvas, int cx, int cy, int radius, Color color);

/* struct Color { */
/* 	unsigned char r; */
/* 	unsigned char g; */
/* 	unsigned char b; */
/* 	unsigned char a; */
/* }; */

/* struct Vector2 { */
/*         float x; */
/*         float y; */
/* }; */

/* struct Vector3 { */
/*         float x; */
/*         float y; */
/*         float z; */
/* }; */

/* struct Rectangle { */
/*         float x; */
/*         float y; */
/*         float width; */
/*         float height; */
/* }; */

/* // Utils */
/* Vector2 Vec2(float x, float y); */
/* Vector3 Vec3(float x, float y, float z); */

/* // NOTE: use catppuccin colorscheme for now */
/* #define SERENE_ROSEWATER (Color){245, 224, 220, 255} /1* #f5e0dc *1/ */
/* #define SERENE_FLAMINGO  (Color){242, 205, 205, 255} /1* #f2cdcd *1/ */
/* #define SERENE_PINK 	 (Color){245, 194, 231, 255} /1* #f5c2e7 *1/ */
/* #define SERENE_MAUVE 	 (Color){203, 166, 247, 255} /1* #cba6f7 *1/ */
/* #define SERENE_RED 	 (Color){243, 139, 168, 255} /1* #f38ba8 *1/ */
/* #define SERENE_MAROON 	 (Color){235, 160, 172, 255} /1* #eba0ac *1/ */
/* #define SERENE_PEACH 	 (Color){250, 179, 135, 255} /1* #fab387 *1/ */
/* #define SERENE_YELLOW 	 (Color){249, 226, 175, 255} /1* #f9e2af *1/ */
/* #define SERENE_GREEN 	 (Color){166, 227, 161, 255} /1* #a6e3a1 *1/ */
/* #define SERENE_TEAL 	 (Color){148, 226, 213, 255} /1* #94e2d5 *1/ */
/* #define SERENE_SKY 	 (Color){137, 220, 235, 255} /1* #89dceb *1/ */
/* #define SERENE_SAPPHIRE  (Color){116, 199, 236, 255} /1* #74c7ec *1/ */
/* #define SERENE_BLUE 	 (Color){137, 180, 250, 255} /1* #89b4fa *1/ */
/* #define SERENE_LAVENDER  (Color){180, 190, 254, 255} /1* #b4befe *1/ */
/* #define SERENE_TEXT 	 (Color){205, 214, 244, 255} /1* #cdd6f4 *1/ */
/* #define SERENE_SUBTEXT1  (Color){186, 194, 222, 255} /1* #bac2de *1/ */
/* #define SERENE_SUBTEXT0  (Color){166, 173, 200, 255} /1* #a6adc8 *1/ */
/* #define SERENE_OVERLAY2  (Color){147, 153, 178, 255} /1* #9399b2 *1/ */
/* #define SERENE_OVERLAY1  (Color){127, 132, 156, 255} /1* #7f849c *1/ */
/* #define SERENE_OVERLAY0  (Color){108, 112, 134, 255} /1* #6c7086 *1/ */
/* #define SERENE_SURFACE2  (Color){88,  91,  112, 255} /1* #585b70 *1/ */
/* #define SERENE_SURFACE1  (Color){69,  71,  90,  255} /1* #45475a *1/ */
/* #define SERENE_SURFACE0  (Color){49,  50,  68,  255} /1* #313244 *1/ */
/* #define SERENE_BASE 	 (Color){30,  30,  46,  255} /1* #1e1e2e *1/ */
/* #define SERENE_MANTLE 	 (Color){24,  24,  37,  255} /1* #181825 *1/ */
/* #define SERENE_CRUST 	 (Color){17,  17,  27,  255} /1* #11111b *1/ */


#endif /* _GFX_H */
