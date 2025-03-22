#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>
#include <stdio.h>
#include <stdlib.h>

#include "ui.h"
#include "util.h"

// TODO:
/* static void srn_platform(void) */
/* { */
/* 	return; */
/* } */

SRN_Context context  = {0};
SRN_X11	    backend  = {0};
/* PLATFORM    platform = 0; */

void srn_init_window(char *title, int width, int height, Allocator *allocator)
{
	context.allocator     = allocator;
	context.Window.width  = width;
	context.Window.height = height;

	// NOTE: separate this in the future
	// X11 backend 
	{
		// NOTE: separate this in the future
		Image *image   = allocator->alloc(sizeof(Image), allocator->ctx);
		image->width   = width;
		image->height  = height;
		image->data    = allocator->alloc(width * height * sizeof(uint32_t), allocator->ctx);
		backend.buffer = image;


		backend.display = XOpenDisplay(NULL);
		backend.window = XCreateSimpleWindow(backend.display,
				XDefaultRootWindow(backend.display),
				0, 0,
				width, height,
				1,
				0,
				0);
		backend.gc = XCreateGC(backend.display, backend.window, 0, NULL);
		XSelectInput(backend.display, backend.window, KeyPressMask | PointerMotionMask);

		XMapWindow(backend.display, backend.window);
		XSizeHints hints;
		hints.flags = PMinSize | PMaxSize;
		hints.min_width = hints.max_width = width;
		hints.min_height = hints.max_height = height;
		XSetWMNormalHints(backend.display, backend.window, &hints);

		XdbeBackBuffer back_buffer = XdbeAllocateBackBufferName(backend.display, backend.window, 0);
		backend.back_buffer = back_buffer;
		XWindowAttributes wa = {0};
		XGetWindowAttributes(backend.display, backend.window, &wa);
		XShmSegmentInfo *si = allocator->alloc(sizeof(XShmSegmentInfo), allocator->ctx);
		backend.image = XShmCreateImage(backend.display, 
				wa.visual,
				wa.depth,
				ZPixmap,
				NULL,
				si,
				width,
				height);
		si->shmid = shmget(IPC_PRIVATE, height * backend.image->bytes_per_line, IPC_CREAT | 0777);
		image->data = si->shmaddr = backend.image->data = shmat(si->shmid, 0, 0);
		si->readOnly = False;
		XShmAttach(backend.display, si);

	}

	context.Time.current     = util_get_ns();
	context.Time.previous    = context.Time.current;
	context.Time.target      = 60;
	context.Time.start       = util_get_ns();
	context.Time.frame_count = 0;

	return;
}

void srn_begin(void)
{
	while (XPending(backend.display) > 0) {
		XEvent event = {0};
		XNextEvent(backend.display, &event);
		switch (event.type) {
			case MotionNotify:
				{
					Window root_window, child_window;
					int window_x, window_y;
					unsigned int mask;
					context.Mouse.previous_position = context.Mouse.current_position;
					int x = 0;
					int y = 0;
					XQueryPointer(backend.display,
							backend.window,
							&root_window, &child_window,
							&window_x, &window_y,
							&x, &y,
							&mask);
					context.Mouse.current_position  = (Vector2){ .x = x, .y = y };
				}
				break;
			case ShmCompletion:
				printf("asdfdfsdf\n");
				break;
		}
	}
	context.Time.previous = context.Time.current;
	context.Time.current  = util_get_ns();
}

void srn_end(void)
{
	// NOTE: assume platform is x11 for now
	/* XPutImage(backend.display, */
	/* 		backend.back_buffer, */
	/* 		backend.gc, */
	/* 		backend.image, */
	/* 		0, 0, 0, 0, */
	/* 		backend.image->width, backend.image->height); */

	XShmPutImage(backend.display,
			backend.back_buffer,
			backend.gc,
			backend.image,
			0, 0,
			0, 0,
			context.Window.width, context.Window.height,
			True);

	XdbeSwapInfo swap_info;
	swap_info.swap_window = backend.window;
	swap_info.swap_action = 0;
	XdbeSwapBuffers(backend.display, &swap_info, 1);

	XSync(backend.display, 0);

	// calculate frame rate
	context.Time.frame_count++;
	double elapsed = (util_get_ns() - (double)context.Time.start)/NS_PER_SEC;
	if (elapsed >= 1.0f) {
		context.Time.current_fps = context.Time.frame_count;
		context.Time.start = util_get_ns();
		context.Time.frame_count = 0;

	}

	double frame_delay = (double)NS_PER_SEC/context.Time.target;
	long elapsed_time = util_get_ns() - context.Time.current;
	if ((double)elapsed_time < frame_delay && context.Time.current > 0)
		util_delay_ns(frame_delay - elapsed_time);
}

void srn_set_fps(int fps)
{
	context.Time.target = fps * 1.05;
}

int srn_get_fps(void)
{
	return context.Time.current_fps;
}

// TODO: optimize this
void srn_clear_background(Color color)
{
	int pixel_count = context.Window.width * context.Window.height;
	uint32_t value = (color.a << 24) | (color.r << 16) | (color.g << 8) | color.b;
	uint32_t *buffer = (uint32_t*)backend.buffer->data;

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

Vector2 srn_get_mouse_position(void)
{
	return context.Mouse.current_position;
}

void srn_draw_pixel(int x, int y, Color color)
{
	if (x >= backend.buffer->width || y >= backend.buffer->height || x < 0 || y < 0)
		return;
	uint32_t *pixel = (uint32_t*)backend.buffer->data + (y * backend.buffer->width + x);
	/* *pixel = color.r | (color.g << 8) | (color.b << 16) | (color.a << 24); */
	*pixel = (color.a << 24) | (color.r << 16) | (color.g << 8) | color.b;
}

void srn_draw_rectangle(int x, int y, int width, int height, Color color)
{
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			srn_draw_pixel(x+i, y+j, color);
		}
	}
}

void srn_draw_line(int start_x, int start_y, int end_x, int end_y, Color color)
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
		srn_draw_pixel(x0, y0, color);
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

void srn_draw_line_ex(int x1, int y1, int x2, int y2, int thickness, int alignment, Color color)
{
	// NOTE: this assumes horizontal for now
	switch (alignment) {
		case 0: // bottom/right expand
			for (int i = 0; i < thickness; i++)
				srn_draw_line(x1, y1 + i, x2, y2 + i, color);
			break;
		case 1: // upper/left expand
			for (int i = 0; i < thickness; i++)
				srn_draw_line(x1, y1 - i, x2, y2 - i, color);
			break;
	}
}

void srn_draw_rectangle_outline(int x, int y, int width, int height, Color color)
{
	// upper
	srn_draw_line(x, y, x + width, y, color);
	// left
	srn_draw_line(x + width, y, x + width, y + height, color);
	// lower
	srn_draw_line(x, y + height, x + width, y + height, color);
	// right
	srn_draw_line(x, y + height, x, y, color);
}

