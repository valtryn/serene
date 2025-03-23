#ifndef BACKEND_H
#define BACKEND_H

#include "core.h"

/* typedef struct SRN_Window SRN_Window; */
/* struct SRN_Window { */
/* }; */

void x11_init_window(SRN_Window *window);
void x11_init_surface(Surface *surface, Allocator *alloc);
int  x11_poll_event(Event *event);
void x11_surface_draw(void);

#endif /* BACKEND_H */
