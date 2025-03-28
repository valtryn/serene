#define _POSIX_C_SOURCE 199309L
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

#include "core.h"
#include "backend.h"
#include "base/types.h"
#include "gfx.h"

SRN_Context CONTEXT = {0};
BackendContext backend = {0};

int init_window(string title, int width, int height, Allocator *alloc)
{
	CONTEXT.allocator = alloc;

	CONTEXT.Window.title  = title;
	CONTEXT.Window.width  = width;
	CONTEXT.Window.height = height;
	CONTEXT.Window.x      = 0;
	CONTEXT.Window.y      = 0;

	CONTEXT.Surface.format = PIXELFORMAT_RGBA32;
	// NOTE: assume the pixel format is RGBA32 for now
	if (CONTEXT.Surface.format == PIXELFORMAT_RGBA32) {
		CONTEXT.Surface.pixels = alloc->alloc(sizeof(U32) * width * height, alloc->ctx);
	}
	CONTEXT.Surface.width  = width;
	CONTEXT.Surface.height = height;
	CONTEXT.Surface.pitch  = width * sizeof(U32);
	CONTEXT.Surface.flags  = 0;

	memset(CONTEXT.Keyboard.curr_key_state, 0, ARRAY_SIZE(CONTEXT.Keyboard.curr_key_state));
	memset(CONTEXT.Keyboard.prev_key_state, 0, ARRAY_SIZE(CONTEXT.Keyboard.prev_key_state));

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
	image->bytes_per_line = CONTEXT.Surface.pitch;
	si->shmid = shmget(IPC_PRIVATE, CONTEXT.Surface.height * image->bytes_per_line, IPC_CREAT | 0777);
	// NOTE: not sure why assigning this works by copying into surface.pixels does not
	CONTEXT.Surface.pixels = si->shmaddr = image->data = shmat(si->shmid, 0, 0);
	si->readOnly = False;
	XShmAttach(display, si);
	backend.ctx = (X11_Context){
		.display      = display,
		.window       = window,
		.image        = image,
		.graphical_ctx= graphical_ctx,
		.back_buffer  = back_buffer,
	};
	// TODO: handle errors
	return 0;
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

void begin(void)
{
	CONTEXT.Time.previous = CONTEXT.Time.current;
	CONTEXT.Time.current  = time_get_ticks();
}

void end(void)
{
	memcpy(CONTEXT.Keyboard.prev_key_state, CONTEXT.Keyboard.curr_key_state, MAX_STATE);
	memset(CONTEXT.Keyboard.curr_key_state, 0, MAX_STATE);
	uint64_t update_end = time_get_ticks();
	CONTEXT.Time.update = update_end - CONTEXT.Time.current;

	draw_surface();

	CONTEXT.Time.draw   = time_get_ticks() - update_end;
	double elapsed = time_get_ticks() - CONTEXT.Time.current;

	CONTEXT.Time.frame = CONTEXT.Time.current - CONTEXT.Time.previous;
	if (CONTEXT.Time.target > 0 && elapsed < CONTEXT.Time.target)
		time_delay((CONTEXT.Time.target - elapsed));
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
			// TODO: implement this
			/* case ClientMessage: */
			/* 	if ((Atom)x11_event.xclient.data.l[0] == wm_delete_window) */
			/* 		ret = 1;  // Exit the loop */
			/* 	break; */
			case MotionNotify:
				break;
			case ButtonPress:
				break;
			case ButtonRelease:
				break;
			case KeyPress: {
				// TODO: handle modifiers
				KeySym keysym = XLookupKeysym(&x11_event.xkey, 0);
				if (keysym <= 512)
					CONTEXT.Keyboard.curr_key_state[keysym] = keysym;
				printf("key: %zu\n", keysym);
				break;
			}
			case KeyRelease:
				break;
			default:
				XQueryPointer(display, window, &root_window, &child_window, &window_x, &window_y, &x, &y, &mask);
				CONTEXT.Mouse.prev_pos = CONTEXT.Mouse.curr_pos;
				CONTEXT.Mouse.curr_pos = VEC2(x, y);
				break;
		}
	}
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

U32 get_fps(void)
{
	return ceil(NS_PER_SECOND/CONTEXT.Time.frame);
}

float get_frame_time(void)
{
	return CONTEXT.Time.frame/NS_PER_SECOND;
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
