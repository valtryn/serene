#ifndef _UI_H
#define _UI_H

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xdbe.h>
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
        XdbeBackBuffer back_buffer;

        struct {
                uint64_t current;
                uint64_t previous;
                uint64_t target;
        } Time ;
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
void ui_begin_draw(SRN_Context *ctx);
void ui_end_draw(SRN_Context *ctx);

void ui_create_renderer(SRN_Context *ctx, SRN_Surface *surface);
void ui_surface_draw(SRN_Context *ctx, SRN_Surface *surface);
void ui_surface_clear(SRN_Surface *surface);
void ui_get_mouse_state(SRN_Context *ctx, int *x, int *y);

double ui_get_frame_time(SRN_Context *ctx);
void ui_set_fps(SRN_Context *ctx, int fps);
#endif /* _UI_H */
