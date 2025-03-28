#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "base/types.h"
#include "color.h"
#include "core.h"
#include "gfx.h"

// TODO: dont know if this name makes sense so maybe change it later.
#define NORMALIZE_ALPHA(x) ((float)(x) / 255.0f)

Color gfx_blend_color(Color src, Color dest)
{
    // Normalize alpha channels to [0, 1]
    float sna = NORMALIZE_ALPHA(src.a); 
    float dna = NORMALIZE_ALPHA(dest.a);

    float blended_alpha = sna + dna * (1.0f - sna);

    if (blended_alpha == 0.0f)
        return (Color){0, 0, 0, 0};

    Color result;
    result.r = (unsigned char)((src.r * sna + dest.r * dna * (1.0f - sna)) * 255.0f / blended_alpha);
    result.g = (unsigned char)((src.g * sna + dest.g * dna * (1.0f - sna)) * 255.0f / blended_alpha);
    result.b = (unsigned char)((src.b * sna + dest.b * dna * (1.0f - sna)) * 255.0f / blended_alpha);
    result.a = (unsigned char)(blended_alpha * 255.0f);

    return result;
}

Color gfx_blend_color_fast(Color src, Color dest)
{
	Color result;
	unsigned int alpha = src.a;

	unsigned int rb = src.r & 0xff00ff;
	unsigned int dest_rb = dest.r & 0xff00ff;
	rb = ((rb * alpha) + (dest_rb * (255 - alpha)) + 0x800080);
	rb = (rb + ((rb >> 8) & 0xff00ff)) >> 8;
	result.r = rb & 0xff;

	unsigned int gb = src.g & 0xff00ff;
	unsigned int dest_gb = dest.g & 0xff00ff;
	gb = ((gb * alpha) + (dest_gb * (255 - alpha)) + 0x800080);
	gb = (gb + ((gb >> 8) & 0xff00ff)) >> 8;
	result.g = gb & 0xff;

	unsigned int bb = src.b & 0xff00ff;
	unsigned int dest_bb = dest.b & 0xff00ff;
	bb = ((bb * alpha) + (dest_bb * (255 - alpha)) + 0x800080);
	bb = (bb + ((bb >> 8) & 0xff00ff)) >> 8;
	result.b = bb & 0xff;

	result.a = src.a + dest.a * (255 - src.a) / 255;

	return result;
}

void gfx_draw_pixel(int x, int y, Color color)
{
	if ((U32)x >= CONTEXT.Surface.width || (U32)y >= CONTEXT.Surface.height || x < 0 || y < 0)
		return;
	uint32_t *pixel = (uint32_t*)CONTEXT.Surface.pixels + (y * CONTEXT.Surface.width + x);

	if (color.a == 255) {
		*pixel = color_ARGB(color);
	} else {
		// NOTE: since x11 uses ARGB32 format we have to do this for now
		Color dest_color = {
			.r = ((*pixel) >> 16) & 0xFF,
			.g = ((*pixel) >> 8)  & 0xFF,
			.b = (*pixel)         & 0xFF,
			.a = ((*pixel) >> 24) & 0xFF,
		};
		Color blended_color = gfx_blend_color(color, dest_color);
		*pixel = color_ARGB(blended_color);
	}

}

// TODO: refactor
void gfx_draw_pixel_buf(size_t index, Color color)
{
	size_t buf_size = CONTEXT.Surface.height * CONTEXT.Surface.width;
	if (index > buf_size) {
		return;
	}
	uint32_t *pixel_buf = (uint32_t*)CONTEXT.Surface.pixels + index;
	*pixel_buf = color_ARGB(color);
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
	uint32_t value = (color.a << 24) | (color.r << 16) | (color.g << 8) | color.b;
	uint32_t *buffer = (uint32_t*)CONTEXT.Surface.pixels;

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

void gfx_draw_rectangle_outline(int x, int y, int width, int height, Color color)
{
	// horizontal upper | lower
	gfx_draw_line(VEC2(x, y), VEC2(x + width, y), color);
	gfx_draw_line(VEC2(x, y + height), VEC2(x + width, y + height), color);

	// verical right - left
	gfx_draw_line(VEC2(x, y + height), VEC2(x, y), color);
	gfx_draw_line(VEC2(x + width, y),  VEC2(x + width, y + height), color);
}

void gfx_draw_image(unsigned char *buf, int width, int height, int channels)
{
	int accum = 0;
	for (int i = 0; i < width * height; i++) {
		Color color = {
			.a = 255,
			.r = buf[accum],
			.g = buf[accum + 1],
			.b = buf[accum + 2]
		};
		gfx_draw_pixel_buf(i, color);
		accum+=channels;
	}
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

