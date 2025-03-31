#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "base/types.h"
#include "color.h"
#include "core.h"
#include "gfx.h"

// TODO: dont know if this name makes sense so maybe change it later.
#define NORMALIZE_ALPHA(x) ((float)(x) / 255.0f)

void gfx_draw_pixel(int x, int y, Color color)
{
	if ((U32)x >= CONTEXT.Surface.width || (U32)y >= CONTEXT.Surface.height || x < 0 || y < 0)
		return;
	uint32_t *pixel = (uint32_t*)CONTEXT.Surface.pixels + (y * CONTEXT.Surface.width + x);
	// NOTE: use argb since we assume the cirrent backend is x11
	*pixel = color_to_int_argb(color);
}

void gfx_draw_line(Vector2 start, Vector2 end, Color color)
{
	if (start.x == end.x) {
		int sy = (start.y < end.y) ? start.y : end.y;
		int ey = (start.y > end.y) ? start.y : end.y;
		for (int y = sy; y <= ey; ++y) {
			gfx_draw_pixel(start.x, y, color);
		}
		return;
	}

	if (start.y == end.y) {
		int sx = (start.x < end.x) ? start.x : end.x;
		int ex = (start.x > end.x) ? start.x : end.x;
		for (int x = sx; x <= ex; ++x) {
			gfx_draw_pixel(x, start.y, color);
		}
		return;
	}

	int x0 = start.x;
	int y0 = start.y;
	int x1 = end.x;
	int y1 = end.y;

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

// NOTE: maybe this should not be here
void gfx_clear_background(Color color)
{
	int val = color.r;
	if (val == color.g && val == color.b && val == color.a) {
		memset(CONTEXT.Surface.pixels, color.r, sizeof(uint32_t) * CONTEXT.Window.width * CONTEXT.Window.height);
		return;
	}
	int pixel_count = CONTEXT.Window.width * CONTEXT.Surface.height;
	uint32_t value = color_to_int_argb(color);
	uint32_t *buffer = (uint32_t*)CONTEXT.Surface.pixels;

	int remaining = pixel_count % 32;
	int offset = pixel_count - remaining;
	
	// TODO: refactor this to SIMD or miltithread
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

void gfx_draw_rectangle_outline(int x, int y, int width, int height, Color color)
{
	gfx_draw_line(VEC2(x, y), VEC2(x + width, y), color);
	gfx_draw_line(VEC2(x, y + height), VEC2(x + width, y + height), color);
	gfx_draw_line(VEC2(x, y + height), VEC2(x, y), color);
	gfx_draw_line(VEC2(x + width, y),  VEC2(x + width, y + height), color);
}

void gfx_draw_rectangle(int x, int y, int width, int height, Color color)
{
	Rectangle rect = {
		.x = x,
		.y = y,
		.width  = width,
		.height = height,
	};
	gfx_draw_rectangle_core(rect, VEC2(0, 0), 0, color);
}

void gfx_draw_rectangle_core(Rectangle rectangle, Vector2 origin, float rotation, Color color)
{
	// TODO: support rotation
	if (rotation != 0.0f) {
		printf("rotation not supported\n");
		return;
	}

	int x = rectangle.x;
	int y = rectangle.y;
	int width = rectangle.width;
	int height = rectangle.height;

	for (int i = 0; i < height; i++) {
		gfx_draw_line((Vector2){x, y + i},     (Vector2){x + width, y + i}, color);  // Drawing each scanline
	}
}

