#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/shm.h>
#include <string.h>
#include <time.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xdbe.h>
#include <X11/extensions/XShm.h>
#include <X11/Xatom.h>

#include "base/allocator.h"
#include "base/types.h"
#include "backend.h"
#include "core.h"
#include "gfx.h"

SRN_Context CONTEXT = {0};
BackendContext backend = {0};

// TODO: maybe make an init_window function that uses its own
//	 arena allocator rather than via a parameter
//	 if implemented add an api to set the arena memory size
int init_window(string title, int width, int height, Allocator *alloc)
{
	CONTEXT.allocator = alloc;
	Allocator *frame_arena_alloc = alloc->alloc(sizeof(Allocator), alloc->ctx);
	arena_allocator_init(frame_arena_alloc, megabytes(2));
	CONTEXT.frame_allocator = frame_arena_alloc;

	CONTEXT.Window.title  = title;
	CONTEXT.Window.width  = width;
	CONTEXT.Window.height = height;
	CONTEXT.Window.x      = 0;
	CONTEXT.Window.y      = 0;

	CONTEXT.Surface.format = PIXELFORMAT_RGBA32;
	CONTEXT.Surface.width  = width;
	CONTEXT.Surface.height = height;
	CONTEXT.Surface.pitch  = width * sizeof(U32);
	CONTEXT.Surface.flags  = 0;

	/* memset(CONTEXT.Keyboard.curr_key_state, 0, ARRAY_SIZE(CONTEXT.Keyboard.curr_key_state)); */
	/* memset(CONTEXT.Keyboard.prev_key_state, 0, ARRAY_SIZE(CONTEXT.Keyboard.prev_key_state)); */

	memset(CONTEXT.Keyboard.curr_key_state, 0, ARRAY_SIZE(CONTEXT.Keyboard.curr_key_state) * sizeof(U32));
	memset(CONTEXT.Keyboard.prev_key_state, 0, ARRAY_SIZE(CONTEXT.Keyboard.prev_key_state) * sizeof(U32));
	memset(CONTEXT.Mouse.curr_mouse_state, 0, ARRAY_SIZE(CONTEXT.Mouse.curr_mouse_state) * sizeof(U8));
	memset(CONTEXT.Mouse.prev_mouse_state, 0, ARRAY_SIZE(CONTEXT.Mouse.prev_mouse_state) * sizeof(U8));

	CONTEXT.Mouse.prev_pos = VEC2(0, 0);
	CONTEXT.Mouse.curr_pos = VEC2(0, 0);

	CONTEXT.Time.current     = time_get_ticks();
	CONTEXT.Time.previous    = 0;
	CONTEXT.Time.frame       = (double)NS_PER_SECOND/60;
	CONTEXT.Time.frame_count = 0;

	// NOTE: assume the backend is x11 for now
	// Create Window

	Display *display           = NULL;
	Window window              = 0;
	Window root_window	   = 0;
	XImage *image              = NULL;
	GC graphical_ctx           = 0;
	XdbeBackBuffer back_buffer = 0;

	display = XOpenDisplay(NULL);
	root_window = XDefaultRootWindow(display);
	window = XCreateSimpleWindow(
			display,
			root_window,
			0, 0,
			width, height,
			1,
			0,
			0
	);

	XSelectInput(display, window, ButtonReleaseMask| ButtonPressMask | KeyPressMask | PointerMotionMask);
	XMapWindow(display, window);

	// NOTE: this should not be here
	XSizeHints hints;
	hints.flags = PMinSize | PMaxSize;
	hints.min_width = hints.max_width   = width;
	hints.min_height = hints.max_height = height;
	XSetWMNormalHints(display, window, &hints);

	/* Atom wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False); */
	/* XSetWMProtocols(display, window, &wm_delete_window, 1); */

	// Create Surface
	graphical_ctx = XCreateGC(display, window, 0, NULL);
	back_buffer = XdbeAllocateBackBufferName(display,  window, 0);
	XWindowAttributes wa = {0};
	XGetWindowAttributes(display, window, &wa);
	XShmSegmentInfo *si = alloc->alloc(sizeof(XShmSegmentInfo), alloc->ctx);
	image = XShmCreateImage(
			display, 
			wa.visual,
			wa.depth,
			ZPixmap,
			NULL,
			si,
			CONTEXT.Surface.width,
			CONTEXT.Surface.height
	);
	si->shmid = shmget(IPC_PRIVATE, CONTEXT.Surface.height * CONTEXT.Surface.pitch, IPC_CREAT |  0777);
	CONTEXT.Surface.pixels = si->shmaddr = image->data = shmat(si->shmid, 0, 0);
	si->readOnly = False;
	XShmAttach(display, si);
	printf("wa: %d\n", wa.depth);
	backend.ctx = (X11_Context){
		.display      = display,
		.window       = window,
		.image        = image,
		.graphical_ctx= graphical_ctx,
		.back_buffer  = back_buffer,
	};
	return 0;
}

void deinit_window(void)
{
	allocator_deinit(CONTEXT.frame_allocator); // free the frame_buffer_allocator
	// free x11 objects
	XdbeDeallocateBackBufferName(backend.ctx.display, backend.ctx.back_buffer);
	XFreeGC(backend.ctx.display, backend.ctx.graphical_ctx);
	XDestroyImage(backend.ctx.image);
	XDestroyWindow(backend.ctx.display, backend.ctx.window);
	XCloseDisplay(backend.ctx.display);
}

int draw_surface(void)
{
	GC gc                      = backend.ctx.graphical_ctx;
	XImage *image              = backend.ctx.image;
	Window window              = backend.ctx.window;
	Display *display           = backend.ctx.display;
	XdbeBackBuffer back_buffer = backend.ctx.back_buffer;

	XShmPutImage(display,
			back_buffer,
			gc,
			image,
			0, 0,
			0, 0,
			CONTEXT.Window.width, CONTEXT.Window.height,
			True);

	XdbeSwapInfo swap_info;
	swap_info.swap_window = window;
	swap_info.swap_action = 0;
	XdbeSwapBuffers(display, &swap_info, 1);
	XSync(display, 0);
	// TODO: handle errors
	return 0;
}

void BEGIN(void)
{
	CONTEXT.Time.previous = CONTEXT.Time.current;
	CONTEXT.Time.current  = time_get_ticks();
}

void END(void)
{
	U64 update_end = time_get_ticks();
	CONTEXT.Time.update = update_end - CONTEXT.Time.current;

	draw_surface();

	CONTEXT.Time.draw = time_get_ticks() - update_end;
	double elapsed    = time_get_ticks() - CONTEXT.Time.current;

	CONTEXT.Time.frame = CONTEXT.Time.current - CONTEXT.Time.previous;
	if (CONTEXT.Time.target > 0 && elapsed < CONTEXT.Time.target)
		time_delay((CONTEXT.Time.target - elapsed));

	CONTEXT.Mouse.prev_pos = CONTEXT.Mouse.curr_pos;
	memcpy(CONTEXT.Mouse.prev_mouse_state, CONTEXT.Mouse.curr_mouse_state, ARRAY_SIZE(CONTEXT.Mouse.prev_mouse_state) * sizeof(U8));
	CONTEXT.frame_allocator->free_all(CONTEXT.frame_allocator->ctx);
}

int should_close(void)
{
	Display *display = backend.ctx.display;
	Window window    = backend.ctx.window;
	Window root_window, child_window;
	int window_x, window_y;
	unsigned int mask;
	int x = 0;
	int y = 0;

	int ret = 0;

	while (XPending(display) > 0) {
		XEvent x11_event = {0};
		XNextEvent(display, &x11_event);

		// NOTE: (SELF) IF THE EVENT IS NOT HANDLED YOU SHOULD PROBABLY ADD IT TO XSelectInput.
		switch (x11_event.type) {
			// TODO: implement
			/* case ClientMessage: */
			/* 	if ((Atom)x11_event.xclient.data.l[0] == wm_delete_window) */
			/* 		ret = 1;  // Exit the loop */
			/* 	break; */
			case MotionNotify:
				XQueryPointer(display, window, &root_window, &child_window, &window_x, &window_y, &x, &y, &mask);
				CONTEXT.Mouse.curr_pos = VEC2(x, y);
				break;
			case ButtonPress:
				CONTEXT.Mouse.curr_mouse_state[x11_event.xbutton.button] = PRESS;
				break;
			case ButtonRelease: {
				CONTEXT.Mouse.curr_mouse_state[x11_event.xbutton.button] = RELEASE;
				break;
			}
			case KeyPress: {
				// TODO: handle modifiers
				KeySym keysym = XLookupKeysym(&x11_event.xkey, 0);
				if (keysym <= MAX_KEY_STATE)
					CONTEXT.Keyboard.curr_key_state[keysym] = keysym;
				if (keysym == XK_q) {
					ret = 1;
				}
				break;
			}
			case KeyRelease:
				break;
			default:
				break;
		}
	}
	// NOTE: strange place to deinit frame_allocator because order of 
	// operation matters between deinit_window() and allocator_deinit(&arena);
	return ret;
}

//
// UTIL FUNCTIONS
//
void clear_background(Color color)
{
	gfx_clear_background(color);
}

void set_fps(U32 fps)
{
	CONTEXT.Time.target = (double)NS_PER_SECOND/fps;
}

// TODO: implement a more accurate fps counter
U32 get_fps(void)
{
	return ceil(NS_PER_SECOND/CONTEXT.Time.frame);
}

Vector2 get_mouse_position(void)
{
	return CONTEXT.Mouse.curr_pos;
}

float get_frame_time(void)
{
	return CONTEXT.Time.frame/NS_PER_SECOND;
}

bool is_mouse_btn_press(SRN_MOUSE key)
{
	return (CONTEXT.Mouse.curr_mouse_state[key] == PRESS) && (CONTEXT.Mouse.prev_mouse_state[key] == RELEASE);
}

bool is_mouse_btn_down(SRN_MOUSE key)
{
	return (CONTEXT.Mouse.curr_mouse_state[key] == PRESS);
}

bool is_mouse_btn_released(SRN_MOUSE key)
{
	return (CONTEXT.Mouse.curr_mouse_state[key] == RELEASE) && (CONTEXT.Mouse.prev_mouse_state[key] == PRESS);
}

bool is_mouse_btn_up(SRN_MOUSE key)
{
	return (CONTEXT.Mouse.curr_mouse_state[key] == RELEASE);
}

bool mouse_in_rect(Vector2 pos, Rectangle rect)
{
	return pos.x >= rect.x && 
	       pos.x <= rect.x + rect.width && 
	       pos.y >= rect.y && 
	       pos.y <= rect.y + rect.height;
}


float lerp(float v0, float v1, float t)
{
	return (1 - t) * v0 + t * v1;
}

//
// TIMING FUNCTIONS
//
void time_delay(uint64_t ns)
{
	struct timespec ts;
	ts.tv_sec  = (ns / NS_PER_SECOND);
	ts.tv_nsec = (ns % NS_PER_SECOND);
	nanosleep(&ts, NULL);
}

U64 time_get_ticks(void)
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (ts.tv_sec * NS_PER_SECOND) + ts.tv_nsec;
}

// 
// INPUT FUNCTIONS
//
/* int key_down(unsigned long key); */
