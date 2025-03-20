#define _POSIX_C_SOURCE 199309L
#define _XOPEN_SOURCE 600
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xdbe.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "ui.h"
#include "util.h"
#include "allocator.h"

/* void ui_init(Allocator *alloc) */
/* { */
/* 	ui_ctx.arena   = alloc; */
/* 	ui_ctx.display = XOpenDisplay(NULL); */

/* 	if (!ui_ctx.display) { */
/* 		printf("Failed to open X11 display\n"); */
/* 		return; */
/* 	} */

/* 	ui_ctx.initialized = 1; */
/* 	return; */
/* } */

SRN_Context *ui_create_ctx(Allocator *allocator)
{
	SRN_Context *ctx = allocator->alloc(sizeof(SRN_Context), allocator->ctx);
	ctx->allocator   = allocator;
	ctx->display     = XOpenDisplay(NULL);
	ctx->window      = 0;

	ctx->Time.current  = util_get_ns();
	ctx->Time.previous = ctx->Time.current;
	ctx->Time.target   = 60;
	
	return ctx;
}

void ui_begin_draw(SRN_Context *ctx)
{
	ctx->Time.previous = ctx->Time.current;
	ctx->Time.current  = util_get_ns();
}

double ui_get_frame_time(SRN_Context *ctx)
{
	return (double)(ctx->Time.current  - ctx->Time.previous) / NS_PER_SEC;
}

void ui_end_draw(SRN_Context *ctx)
{
	double frame_delay = (double)NS_PER_SEC/ctx->Time.target;
	uint64_t elapsed_time = util_get_ns() - ctx->Time.current;
	if ((double)elapsed_time < frame_delay)
		util_delay_ns((uint64_t)frame_delay - elapsed_time);
}

void ui_set_fps(SRN_Context *ctx, int fps)
{
	ctx->Time.target = fps;
}

SRN_Window *ui_create_window(SRN_Context *ctx, string name, int width, int height)
{
	Allocator *arena = ctx->allocator;
	SRN_Window *window = arena->alloc(sizeof(SRN_Window), arena->ctx);
	window->name   = name;
	window->width  = width;
	window->height = height;

	ctx->window = XCreateSimpleWindow(
			ctx->display,
			XDefaultRootWindow(ctx->display),
			0, 0,
			width,
			height,
			1,
			0,
			0);
	XMapWindow(ctx->display, ctx->window);
	
	{
		// TODO: remove
		XSizeHints hints;
		hints.flags = PMinSize | PMaxSize;
		hints.min_width = hints.max_width = width;
		hints.min_height = hints.max_height = height;
		XSetWMNormalHints(ctx->display, ctx->window, &hints);
	}

	XdbeBackBuffer back_buffer = XdbeAllocateBackBufferName(ctx->display, ctx->window, 0);
	ctx->back_buffer = back_buffer;

	return window;
}

SRN_Surface *ui_create_surface(SRN_Context *ctx, int width, int height)
{
	Allocator *arena = ctx->allocator;
	SRN_Surface *surface = arena->alloc(sizeof(SRN_Surface), arena->ctx);
	surface->width  = width;
	surface->height = height;
	surface->pitch  = width * sizeof(uint32_t);
	surface->buffer = arena->alloc(sizeof(uint32_t) * width * height, arena->ctx);

	XWindowAttributes wa = {0};
	XGetWindowAttributes(ctx->display, ctx->window, &wa);
	ctx->image = XCreateImage(ctx->display,
			wa.visual,
			wa.depth,
			ZPixmap,
			0,
			surface->buffer,
			surface->width,
			surface->height,
			32,
			surface->width * sizeof(uint32_t));

	ctx->gc = XCreateGC(ctx->display, ctx->window, 0, NULL);

	return surface;
}

void ui_surface_clear(SRN_Surface *surface)
{
	memset(surface->buffer, 0, surface->width * surface->height * sizeof(uint32_t));
}

void ui_surface_draw(SRN_Context *ctx, SRN_Surface *surface)
{
	/* XPutImage(ctx->display, ctx->window, ctx->gc, ctx->image, 0, 0, 0, 0, surface->width, surface->height); */

	XPutImage(ctx->display, ctx->back_buffer, ctx->gc, ctx->image, 0, 0, 0, 0, surface->width, surface->height);
	XdbeSwapInfo swap_info;
	swap_info.swap_window = ctx->window;
	swap_info.swap_action = 0;
	XdbeSwapBuffers(ctx->display, &swap_info, 1);
}

void ui_get_mouse_state(SRN_Context *ctx, int *x, int *y)
{
	Window root_window, child_window;
	int window_x, window_y;
	unsigned int mask;
	XQueryPointer(ctx->display, ctx->window, &root_window, &child_window, &window_x, &window_y, x, y, &mask);
	return;
}
