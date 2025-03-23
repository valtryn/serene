#include "backend.h"
#include "base/allocator.h"
#include "core.h"

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xdbe.h>
#include <X11/extensions/XShm.h>
#include <stdio.h>
#include <sys/shm.h>

Display *display           = NULL;
Window	 window            = 0;
XImage  *ximage            = NULL;
GC       gc                = 0;
XdbeBackBuffer back_buffer = 0;

void x11_init_window(SRN_Window *w)
{
	display = XOpenDisplay(NULL);
	window  = XCreateSimpleWindow( display, XDefaultRootWindow(display), 0, 0, w->width, w->height, 1, 0, 0);
	XMapWindow(display, window);
	XSelectInput(display, window, ButtonReleaseMask| ButtonPressMask | KeyPressMask | PointerMotionMask);

	// NOTE: this should not be here
	XSizeHints hints;
	hints.flags = PMinSize | PMaxSize;
	hints.min_width = hints.max_width   = w->width;
	hints.min_height = hints.max_height = w->height;
	XSetWMNormalHints(display, window, &hints);
}

void x11_init_surface(Surface *surface, Allocator *alloc)
{
	gc = XCreateGC(display, window, 0, NULL);
	back_buffer = XdbeAllocateBackBufferName(display, window, 0);
	XWindowAttributes wa = {0};
	XGetWindowAttributes(display, window, &wa);

	XShmSegmentInfo *si = alloc->alloc(sizeof(XShmSegmentInfo), alloc->ctx);
	ximage = XShmCreateImage(display, 
			wa.visual,
			wa.depth,
			ZPixmap,
			NULL,
			si,
			surface->width,
			surface->height);
	si->shmid = shmget(IPC_PRIVATE, surface->height * ximage->bytes_per_line, IPC_CREAT | 0777);
	surface->data = si->shmaddr = ximage->data = shmat(si->shmid, 0, 0);
	si->readOnly = False;
	XShmAttach(display, si);
}

int x11_poll_event(Event *event)
{
	Window root_window, child_window;
	int window_x, window_y;
	unsigned int mask;
	int x = 0;
	int y = 0;
	int events_processed = 0;

	while (XPending(display) > 0) {
		XEvent x11_event = {0};
		XNextEvent(display, &x11_event);
		events_processed++;

		switch (x11_event.type) {
			case MotionNotify:
				event->type = EVENT_MOUSE_MOTION;
				break;
			case ButtonPress:
				event->type = EVENT_MOUSE_BUTTON_DOWN;
				break;
			case ButtonRelease:
				event->type = EVENT_MOUSE_BUTTON_UP;
				break;
			default:
				XQueryPointer(display, window, &root_window, &child_window, &window_x, &window_y, &x, &y, &mask);
				global_ctx.Mouse.prev_pos = global_ctx.Mouse.curr_pos;
				global_ctx.Mouse.curr_pos = vec2(x, y);
				break;
		}
	}
	return events_processed;
}

void x11_surface_draw(void)
{
	XShmPutImage(display,
			back_buffer,
			gc,
			ximage,
			0, 0,
			0, 0,
			global_ctx.window.width, global_ctx.window.height,
			True);

	XdbeSwapInfo swap_info;
	swap_info.swap_window = window;
	swap_info.swap_action = 0;
	XdbeSwapBuffers(display, &swap_info, 1);
	XSync(display, 0);
}
