#include "gui.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

int gui_canvas_init(Canvas *canvas, int width, int height, Allocator *alloc)
{
	canvas->width = width;
	canvas->height = height;
	canvas->pitch = width * sizeof(uint32_t);
	canvas->buffer = alloc->alloc(width * height * sizeof(uint32_t), alloc->ctx);
	return (!canvas->buffer) ? -1 : 0;
}

void gui_draw_pixel(Canvas *canvas, int x, int y, Color color)
{

	if (x >= canvas->width || y >= canvas->height || x < 0 || y < 0)
		return;
	uint32_t *pixel = canvas->buffer + (y * canvas->width + x);
	*pixel = color.r | (color.g << 8) | (color.b << 16) | (color.a << 24);
}

void gui_draw_rectangle(Canvas *canvas, int x, int y, int width, int height, Color color) {
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			gui_draw_pixel(canvas, x+i, y+j, color);
		}
	}
}

void gui_reset(Canvas *canvas) {
	memset(canvas->buffer, 0, canvas->width * canvas->height * sizeof(uint32_t));
}

void gui_draw_line(Canvas *canvas, int start_x, int start_y, int end_x, int end_y, Color color)
{
    int dx = abs(end_x - start_x);
    int dy = abs(end_y - start_y);
    
    // Calculate the direction of the step (1 or -1)
    int sx = (start_x < end_x) ? 1 : -1;  // Step direction for x
    int sy = (start_y < end_y) ? 1 : -1;  // Step direction for y
    
    // Initial error term
    int err = dx - dy;  // Error term

    int x = start_x;
    int y = start_y;

    while (1) {
        // Plot the current pixel (x, y)

        // If we've reached the end point, break out of the loop
        if (x == end_x && y == end_y)
            break;

        // Calculate the error term adjustments
        int e2 = err * 2;

        // If error is greater than 0, move in the x-direction
        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }

        // If error is less than 0, move in the y-direction
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
	gui_draw_pixel(canvas, x, y, color);
    }
}

void gui_draw_rectangle_outline(Canvas *canvas, int x, int y, int width, int height, Color color)
{
	gui_draw_line(canvas, x, y, x + width, y, color);
	gui_draw_line(canvas, x, y + height, x, y, color);
	gui_draw_line(canvas, x, y + height, x + width, y + height, color);
	gui_draw_line(canvas, x + width, y, x + width, y + height, color);
}

// Function to draw a non-anti-aliased circle using the Midpoint Circle Algorithm
void gui_draw_circle(Canvas *canvas, int cx, int cy, int radius, Color color) {
    // Initial point at (r, 0)
    int x = radius;
    int y = 0;
    
    // Initial decision parameter
    int p = 1 - radius;

    // Plot points using circle symmetry (8 symmetrical points)
    while (x >= y) {
        // Plot the 8 points
        gui_draw_pixel(canvas, cx + x, cy + y, color); // (x, y)
        gui_draw_pixel(canvas, cx - x, cy + y, color); // (-x, y)
        gui_draw_pixel(canvas, cx + x, cy - y, color); // (x, -y)
        gui_draw_pixel(canvas, cx - x, cy - y, color); // (-x, -y)
        gui_draw_pixel(canvas, cx + y, cy + x, color); // (y, x)
        gui_draw_pixel(canvas, cx - y, cy + x, color); // (-y, x)
        gui_draw_pixel(canvas, cx + y, cy - x, color); // (y, -x)
        gui_draw_pixel(canvas, cx - y, cy - x, color); // (-y, -x)

        // Increment y
        y++;

        // Update decision parameter
        if (p <= 0) {
            p = p + 2 * y + 1;  // Midpoint is inside or on the circle
        } else {
            x--;
            p = p + 2 * y - 2 * x + 1;  // Midpoint is outside the circle
        }
    }
}
