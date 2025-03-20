#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "gfx.h"

/* void gfx_canvas_init(Canvas *canvas, SRN_Surface *surface) */
/* { */
/* 	canvas->width  = surface->width; */
/* 	canvas->height = surface->height; */
/* 	canvas->pitch  = surface->pitch; */
/* 	canvas->buffer = surface->buffer; */
/* } */

void gfx_draw_pixel(Canvas *canvas, int x, int y, Color color)
{

	if (x >= canvas->width || y >= canvas->height || x < 0 || y < 0)
		return;
	uint32_t *pixel = canvas->buffer + (y * canvas->width + x);
	/* *pixel = color.r | (color.g << 8) | (color.b << 16) | (color.a << 24); */
	*pixel = (color.a << 24) | (color.r << 16) | (color.g << 8) | color.b;
}

void gfx_draw_rectangle(Canvas *canvas, int x, int y, int width, int height, Color color) {
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			gfx_draw_pixel(canvas, x+i, y+j, color);
		}
	}
}

void gfx_clear(Canvas *canvas) {
	memset(canvas->buffer, 0, canvas->width * canvas->height * sizeof(uint32_t));
	/* canvas->allocator->free_all(canvas->allocator->ctx); */
}

void gfx_draw_line(Canvas *canvas, int start_x, int start_y, int end_x, int end_y, Color color)
{
	// Copied from https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
	int x0 = start_x;
	int y0 = start_y;
	int x1 = end_x;
	int y1 = end_y;

	int dx = abs(x1- x0);
	int sx = x0 < x1 ? 1 : -1;
	int dy = -abs(y1 - y0);
	int sy = y0 < y1 ? 1 : -1;
	int error = dx + dy;

	while (1) {
		gfx_draw_pixel(canvas, x0, y0, color);
		int e2 = 2 * error;

		if (e2 >= dy) {
			if (x0 == x1) break;
			error = error + dy;
			x0 = x0 + sx;
		}
		if (e2 <= dx) {
			if (y0 == y1) break;
			error = error + dx;
			y0 = y0 + sy;
		}
	}
}

void gfx_draw_rectangle_outline(Canvas *canvas, int x, int y, int width, int height, Color color)
{
	// upper
	gfx_draw_line(canvas, x, y, x + width, y, color);
	// left
	gfx_draw_line(canvas, x + width, y, x + width, y + height, color);
	// lower
	gfx_draw_line(canvas, x, y + height, x + width, y + height, color);
	// right
	gfx_draw_line(canvas, x, y + height, x, y, color);
}

void gfx_draw_circle_outline(Canvas *canvas, int cx, int cy, int radius, Color color) {
	int x = radius;
	int y = 0;
	int t1 = radius / 16;
	int t2;

	while (x >= y) {
		gfx_draw_pixel(canvas, cx + x, cy + y, color);  // Octant 1
		gfx_draw_pixel(canvas, cx - x, cy + y, color);  // Octant 2
		gfx_draw_pixel(canvas, cx + x, cy - y, color);  // Octant 3
		gfx_draw_pixel(canvas, cx - x, cy - y, color);  // Octant 4
		gfx_draw_pixel(canvas, cx + y, cy + x, color);  // Octant 5
		gfx_draw_pixel(canvas, cx - y, cy + x, color);  // Octant 6
		gfx_draw_pixel(canvas, cx + y, cy - x, color);  // Octant 7
		gfx_draw_pixel(canvas, cx - y, cy - x, color);  // Octant 8
		y++;
		t1 += y;
		t2 = t1 - x;
		if (t2 >= 0) {
			t1 = t2;
			x--;
		}
	}
}

void gfx_draw_circle(Canvas *canvas, int cx, int cy, int radius, Color color) {
	int x = radius;
	int y = 0;
	int t1 = radius / 16;
	int t2;

	while (x >= y) {
		for (int i = cx - x; i <= cx + x; i++) {
			gfx_draw_pixel(canvas, i, cy + y, color);  // Bottom octant
			gfx_draw_pixel(canvas, i, cy - y, color);  // Top octant
		}
		for (int i = cx - y; i <= cx + y; i++) {
			gfx_draw_pixel(canvas, i, cy + x, color);  // Bottom octant
			gfx_draw_pixel(canvas, i, cy - x, color);  // Top octant
		}
		y++;
		t1 += y;
		t2 = t1 - x;
		if (t2 >= 0) {
			t1 = t2;
			x--;
		}
	}
}

Vector2 Vec2(float x, float y)
{
	return (Vector2){ x, y};
}

Vector3 Vec3(float x, float y, float z)
{
	return (Vector3){ x, y, z};
}
