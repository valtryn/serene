#ifndef BACKEND_H
#define BACKEND_H

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xdbe.h>
#include <X11/extensions/XShm.h>

typedef struct {
	Display       *display;
	Window         window;
	XImage        *image;
	GC             graphical_ctx;
	XdbeBackBuffer back_buffer;
} X11_Context;

typedef struct {
	/* union { */
	X11_Context ctx;
	/* }; */
} BackendContext;

#endif /* BACKEND_H */
