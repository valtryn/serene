#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>

#include "core.h"
#include "color.h"
#include "backend.h"
#include "base/str.h"
#include "base/allocator.h"
#include "gfx.h"

Context global_ctx = {0};

void init_window(string title, int width, int height, Allocator *alloc)
{
	global_ctx.alloc = alloc;

	global_ctx.window.title  = title;
	global_ctx.window.width  = width;
	global_ctx.window.height = height;
	
	global_ctx.Mouse.curr_pos = vec2(0, 0);
	global_ctx.Mouse.prev_pos = vec2(0, 0);

	global_ctx.Time.init_time = get_tick_ns();
	global_ctx.Time.current   = 0;
	global_ctx.Time.previous  = 0;

	global_ctx.Time.target_fps  = 60;
	global_ctx.Time.current_fps = 0;

	global_ctx.surface.width  = width;
	global_ctx.surface.height = width;
	global_ctx.surface.data   = alloc->alloc(sizeof(uint32_t) * width * height, alloc->ctx);

	x11_init_window(&global_ctx.window);
	x11_init_surface(&global_ctx.surface, alloc);
}

void begin(void)
{
	Event event;
	int quit = 0;
	while (poll_event(&event) > 0) {
		switch (event.type) {
			case EVENT_MOUSE_MOTION: 
				/* printf("pos: %f - %f\n", get_mouse_position().x, get_mouse_position().y); */
				break;
			case EVENT_MOUSE_BUTTON_DOWN:
				global_ctx.Mouse.State.press = 1;
				break;
			case EVENT_MOUSE_BUTTON_UP:
				global_ctx.Mouse.State.press = 0;
				break;
			default:
				break;
		}
	}
	global_ctx.Time.previous = global_ctx.Time.current;
	global_ctx.Time.current  = get_tick_ns();
}

void end(void)
{
	surface_draw();

	global_ctx.Time.frame_count++;
	double elapsed = (double)(get_tick_ns() - global_ctx.Time.init_time) / NS_PER_SEC;
	// NOTE: not sure if this correct
	static double next_fps_update = 1.0;
	if (elapsed >= next_fps_update) {
		global_ctx.Time.current_fps = global_ctx.Time.frame_count;
		next_fps_update = elapsed + 1.0f;
		global_ctx.Time.frame_count = 0;
	}

	double frame_delay = (double)NS_PER_SEC/global_ctx.Time.target_fps;
	long elapsed_frame_time = get_tick_ns() - global_ctx.Time.current;
	if ((double)elapsed_frame_time < frame_delay && global_ctx.Time.current > 0)
		delay_ns(frame_delay - elapsed_frame_time);
}

void surface_draw(void)
{
	x11_surface_draw();
}

void clear_background(Color color)
{
	gfx_clear_background(color);
}


void delay_ns(long ns)
{
	struct timespec ts;
	ts.tv_sec  = (ns / NS_PER_SEC);
	ts.tv_nsec = (ns % NS_PER_SEC);
	nanosleep(&ts, NULL);
}

int poll_event(Event *event)
{
	return x11_poll_event(event);
}

Vector2 get_mouse_position(void)
{
	return global_ctx.Mouse.curr_pos;
}

unsigned int get_fps(void)
{
	return global_ctx.Time.current_fps;
}

void set_fps(unsigned int fps)
{
	global_ctx.Time.target_fps = fps;
}

Vector2 vec2(float x, float y)
{
	return (Vector2){x,y};
}

unsigned long get_tick_ns(void)
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (ts.tv_sec * NS_PER_SEC) + ts.tv_nsec;
}
