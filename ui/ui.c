#include <SDL3/SDL_stdinc.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "ui.h"
#include "util.h"
#include "allocator.h"

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
		Image *image  = allocator->alloc(sizeof(Image), allocator->ctx);
		image->width  = width;
		image->height = height;
		image->data   = allocator->alloc(width * height * sizeof(uint32_t), allocator->ctx);
		backend.buffer = image;


		backend.display = XOpenDisplay(NULL);
		backend.window = XCreateSimpleWindow(backend.display,
				XDefaultRootWindow(backend.display),
				0, 0,
				width, height,
				1,
				0,
				0);

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
		backend.gc = XCreateGC(backend.display, backend.window, 0, NULL);
/* 		backend.image = XCreateImage(backend.display, */
/* 				wa.visual, */
/* 				wa.depth, */
/* 				ZPixmap, */
/* 				0, */
/* 				image->data, */
/* 				image->width, */
/* 				image->height, */
/* 				32, */
/* 				image->width * sizeof(uint32_t)); */

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
		si->shmaddr = backend.image->data = shmat(si->shmid, 0, 0);
		si->readOnly = False;
		image->data = si->shmaddr;

		XShmAttach(backend.display, si);

	}

	/* XSync(backend.display, False); */
	context.Time.current     = util_get_ns();
	context.Time.previous    = context.Time.current;
	context.Time.target      = 60;
	context.Time.start       = util_get_ns();
	context.Time.frame_count = 0;

	return;
}

void srn_begin(void)
{
	XSelectInput(backend.display, backend.window, KeyPressMask | PointerMotionMask);
	while (XPending(backend.display) > 0) {
		XEvent event = {0};
		XNextEvent(backend.display, &event);
		switch (event.type) {
			case MotionNotify:
				printf("moving mouse\n");
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
			0);

	XdbeSwapInfo swap_info;
	swap_info.swap_window = backend.window;
	swap_info.swap_action = 0;
	XdbeSwapBuffers(backend.display, &swap_info, 1);
	/* XSync(backend.display, 0); */
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
	if ((double)elapsed_time < frame_delay)
		util_delay_ns(frame_delay - elapsed_time);
}

void srn_set_fps(unsigned int fps)
{
	context.Time.target = fps;
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

	int remaining = pixel_count % 20;
	int offset = pixel_count - remaining;
	
	for (int i = 0; i < offset; i+=20) {
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
	}

	for (int i = offset; i < pixel_count; i++) {
		buffer[i] = value;
	}
}



void ui_get_mouse_state(int *x, int *y)
{
	Window root_window, child_window;
	int window_x, window_y;
	unsigned int mask;
	XQueryPointer(backend.display, backend.window, &root_window, &child_window, &window_x, &window_y, x, y, &mask);
	return;
}

void ui_draw_pixel(int x, int y, Color color)
{
	if (x >= backend.buffer->width || y >= backend.buffer->height || x < 0 || y < 0)
		return;
	uint32_t *pixel = (uint32_t*)backend.buffer->data + (y * backend.buffer->width + x);
	/* *pixel = color.r | (color.g << 8) | (color.b << 16) | (color.a << 24); */
	*pixel = (color.a << 24) | (color.r << 16) | (color.g << 8) | color.b;
}

void ui_draw_rectangle(int x, int y, int width, int height, Color color)
{
	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			ui_draw_pixel(x+i, y+j, color);
		}
	}
}

