#define _POSIX_C_SOURCE 199309L
#include <time.h>
#include <stdio.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdint.h>
#include <unistd.h>
#include <SDL3/SDL.h>

#include "allocator.h"
#include "gfx.h"
#include "str.h"
#include "ui.h"
#include "util.h"

static const int width  = 1280;
static const int height = 720;

static const int TARGET_FPS = 60;
static const double FRAME_DELAY = NSEC/TARGET_FPS;

int main(void)
{
	Allocator arena = {0};
	arena_allocator_init(&arena, megabytes(10));

	SDL_Init(SDL_INIT_VIDEO);
	SRN_Context *ui_ctx  = ui_create_ctx(&arena);
	SRN_Window  *window  = ui_create_window(ui_ctx, str_make("serene", &arena), width, height);
	SRN_Surface *surface = ui_create_surface(ui_ctx, width, height);

	XSelectInput(ui_ctx->display, ui_ctx->window, KeyPressMask);

	Canvas canvas = {0};
	gfx_canvas_init(&canvas, surface);
	int quit = 0;

	uint64_t current_time = SDL_GetTicksNS();
	uint64_t previous_time = current_time;
	double delta_time = 0.0f;

	int accum = 0;
	while (!quit) {
		previous_time = current_time;
		current_time = SDL_GetTicksNS();
		delta_time = (double)(current_time - previous_time) / 1000000000.0;

		XEvent event = {0};
		while (XPending(ui_ctx->display) > 0) {
			XNextEvent(ui_ctx->display, &event);
			switch (event.type) {
				case KeyPress:
					if (XLookupKeysym(&event.xkey, 0) == XK_Escape) {
						quit = 1;
						break;
					}
			}
		};
		gfx_draw_rectangle(&canvas, accum * delta_time * 10, (height/2), 100, 100, SERENE_YELLOW);
		ui_surface_draw(ui_ctx, surface);
		ui_surface_clear(surface);
		accum++;

		uint64_t elapsed_time = SDL_GetTicksNS() - current_time;
		if ((double)elapsed_time < FRAME_DELAY) SDL_DelayPrecise((uint64_t)FRAME_DELAY - elapsed_time);

	}
}
