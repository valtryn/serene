#ifndef _UI_H
#define _UI_H

#include <X11/Xlib.h>
#include <stdint.h>

#include "str.h"

typedef struct SRN_Context SRN_Context;
typedef struct SRN_Window  SRN_Window;
typedef struct SRN_Surface SRN_Surface;

struct SRN_Context {
        Allocator *allocator;
        Display   *display;
        Window     window;
        XImage    *image;
        GC         gc;

};

struct SRN_Window {
        int    width;
        int    height;
        string name;
};

struct SRN_Surface {
        /* int flags; */
        /* int pixel_format; */
        int  width;
        int  height;
        int  pitch;
        void *buffer;
};


SRN_Context *ui_create_ctx(Allocator *alloc);
SRN_Window  *ui_create_window(SRN_Context *ctx, string name, int width, int height);
SRN_Surface *ui_create_surface(SRN_Context *ctx, int width, int height);
void ui_create_renderer(SRN_Context *ctx, SRN_Surface *surface);
void ui_surface_draw(SRN_Context *ctx, SRN_Surface *surface);
void ui_surface_clear(SRN_Surface *surface);
long ui_get_ticks_ns(void);
#endif /* _UI_H */
