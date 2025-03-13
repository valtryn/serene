#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_properties.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <stddef.h>
#include <stdint.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "allocator.h"
#include "gui.h"

static const double TARGET_FPS = 6000;
static const double FRAME_DELAY = 1000000000.0f / TARGET_FPS;

int quit_application(SDL_Event *event)
{
	return event->type == SDL_EVENT_QUIT || (event->type == SDL_EVENT_KEY_DOWN && event->key.key == SDLK_ESCAPE) ? 1 : 0;
}

int main(void)
{
	Allocator arena = {0};
	arena_allocator_init(&arena, megabytes(10));
	Canvas canvas = {0};
	gui_canvas_init(&canvas, 1280, 720, &arena);

	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window *window = NULL;
	SDL_Surface *surface = NULL;
	SDL_Renderer *renderer = NULL;
	SDL_Texture *texture = NULL;
	window = SDL_CreateWindow("serene", canvas.width, canvas.height, 0);
	surface = SDL_CreateSurfaceFrom(canvas.width, canvas.height, SDL_PIXELFORMAT_RGBA32, canvas.buffer, canvas.pitch);
	renderer = SDL_CreateRenderer(window, NULL);
	texture = SDL_CreateTextureFromSurface(renderer, surface);


	uint64_t current_time = SDL_GetTicksNS();
	uint64_t previous_time = current_time;
	double delta_time = 0.0f;

	SDL_Event event = {0};
	Vector2 pos = { canvas.width/2, canvas.height/2 };
	int quit = 0;
	int pressed = 0;
	int shape = 0;
	while (!quit) {
		previous_time = current_time;
		current_time = SDL_GetTicksNS();
		delta_time = (double)(current_time - previous_time) / 1000000000.0;

		/* gui_reset(&canvas); */
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_EVENT_MOUSE_MOTION:
					SDL_GetMouseState(&pos.x, &pos.y);
					break;
				case SDL_EVENT_MOUSE_BUTTON_DOWN:
					pressed = 1;
					break;
				case SDL_EVENT_MOUSE_BUTTON_UP:
					pressed = 0;
					break;
				case SDL_EVENT_KEY_DOWN:
					switch (event.key.key) {
						case SDLK_R:
							gui_reset(&canvas);
							break;
						case SDLK_A:
							shape = 0;
							break;
						case SDLK_S:
							shape = 1;
							break;
						case SDLK_D:
							shape = 2;
							break;

					} 
			}
			quit = quit_application(&event);
		}


		/* gui_draw_line(&canvas, 0, 0, canvas.width, canvas.height, SERENE_RED); */
		if (pressed == 1) {
			if (shape == 0) {
				gui_draw_rectangle_outline(&canvas, pos.x - 25, pos.y - 25, 50, 50, SERENE_RED);
			}
			if (shape == 1) {
				gui_draw_circle(&canvas, pos.x, pos.y, 50, SERENE_BLUE);
			}
			if (shape == 2) {
				gui_draw_rectangle(&canvas, pos.x - 25, pos.y - 25, 50, 50, SERENE_GREEN);
			}
		}

		SDL_UpdateTexture(texture, NULL, canvas.buffer, canvas.pitch);
		SDL_RenderTexture(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);

		SDL_RenderClear(renderer);
		uint64_t elapsed_time = SDL_GetTicksNS() - current_time;
		if ((double)elapsed_time < FRAME_DELAY) SDL_DelayPrecise((uint64_t)FRAME_DELAY - elapsed_time);
	}

	allocator_deinit(&arena);
	return 0;
}

