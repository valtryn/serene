#define _POSIX_C_SOURCE 199309L
#define _XOPEN_SOURCE 600
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "ui.h"
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
	SRN_Context *ui_ctx = allocator->alloc(sizeof(SRN_Context), allocator->ctx);
	ui_ctx->allocator = allocator;
	ui_ctx->display   = XOpenDisplay(NULL);
	ui_ctx->window    = 0;
	return ui_ctx;
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
	XSync(ctx->display, 0);

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
	XPutImage(ctx->display, ctx->window, ctx->gc, ctx->image, 0, 0, 0, 0, surface->width, surface->height);
}
