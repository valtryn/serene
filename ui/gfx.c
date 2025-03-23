#include <string.h>

#include "gfx.h"
#include "color.h"
#include "core.h"

void gfx_draw_pixel(int x, int y, Color color)
{
	if (x >= global_ctx.surface.width || y >= global_ctx.surface.height || x < 0 || y < 0)
		return;
	uint32_t *pixel = (uint32_t*)global_ctx.surface.data + (y * global_ctx.surface.width + x);
	/* *pixel = color.r | (color.g << 8) | (color.b << 16) | (color.a << 24); */
	// we are using 
	*pixel = color_ARGB(color);
}

// NOTE: maybe this should not be here
void gfx_clear_background(Color color)
{
	int val = color.r;
	if (val == color.g && val == color.b && val == color.a) {
		memset(global_ctx.surface.data, color.r, sizeof(uint32_t) * global_ctx.window.width * global_ctx.window.height);
		return;
	}
	int pixel_count = global_ctx.window.width * global_ctx.surface.height;
	uint32_t value = (color.a << 24) | (color.r << 16) | (color.g << 8) | color.b;
	uint32_t *buffer = (uint32_t*)global_ctx.surface.data;

	int remaining = pixel_count % 32;
	int offset = pixel_count - remaining;
	
	for (int i = 0; i < offset; i+=32) {
		buffer[i+0] = value;
		buffer[i+1] = value;
		buffer[i+2] = value;
		buffer[i+3] = value;
		buffer[i+4] = value;
		buffer[i+5] = value;
		buffer[i+6] = value;
		buffer[i+7] = value;
		buffer[i+8] = value;
		buffer[i+9] = value;
		buffer[i+10] = value;
		buffer[i+11] = value;
		buffer[i+12] = value;
		buffer[i+13] = value;
		buffer[i+14] = value;
		buffer[i+15] = value;
		buffer[i+16] = value;
		buffer[i+17] = value;
		buffer[i+18] = value;
		buffer[i+19] = value;
		buffer[i+20] = value;
		buffer[i+21] = value;
		buffer[i+22] = value;
		buffer[i+23] = value;
		buffer[i+24] = value;
		buffer[i+25] = value;
		buffer[i+26] = value;
		buffer[i+27] = value;
		buffer[i+28] = value;
		buffer[i+29] = value;
		buffer[i+30] = value;
		buffer[i+31] = value;
	}

	for (int i = offset; i < pixel_count; i++) {
		buffer[i] = value;
	}
}

void gfx_draw_rectangle(int x, int y, int width, int height, Color color)
{
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			gfx_draw_pixel(x+i, y+j, color);
		}
	}
}

void gfx_draw_line(int start_x, int start_y, int end_x, int end_y, Color color)
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
		gfx_draw_pixel(x0, y0, color);
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

void gfx_draw_rectangle_outline(int x, int y, int width, int height, Color color)
{
	// upper
	gfx_draw_line(x, y, x + width, y, color);
	// left
	gfx_draw_line(x + width, y, x + width, y + height, color);
	// lower
	gfx_draw_line(x, y + height, x + width, y + height, color);
	// right
	gfx_draw_line(x, y + height, x, y, color);
}

