#ifndef CORE_H
#define CORE_H

#include "base/allocator.h"
#include "base/str.h"
#include "color.h"
#include <stdbool.h>

#define NS_PER_SEC 1000000000LL

typedef struct Context Context;
typedef struct SRN_Window SRN_Window;
typedef struct Surface Surface;

typedef struct Event Event;

typedef struct Vector2 Vector2;
typedef struct Vector3 Vector3;
typedef struct Rectangle Rectangle;
typedef struct Image Image;

typedef enum {
	PF_RGBA32 = 0,
} PixelFormat;

typedef enum EventType {
	EVENT_QUIT	         = 0,
	EVENT_MOUSE_MOTION,
    	EVENT_MOUSE_BUTTON_DOWN,
    	EVENT_MOUSE_BUTTON_UP,         
    	EVENT_MOUSE_WHEEL,
} EventType;

struct Vector2 {
        float x;
        float y;
};

struct Vector3 {
        float x;
        float y;
        float z;
};

extern Context global_ctx;

struct SRN_Window {
	string title;
	int x, y;
	int width, height;
	int min_w, min_h;
	int max_w, max_h;
};

struct Surface {
	void        *data;
	int          width;
	int          height;
	int          pitch;
	PixelFormat  fmt;
};

struct Event {
	EventType type;
};

struct Context {
	Allocator *alloc;
	SRN_Window window;
	Surface    surface;

	struct {
		Vector2 curr_pos;
		Vector2 prev_pos;
		struct {
			int press;
			int scroll;
			int scroll_direction;
		} State;
	} Mouse;


	struct {
		long init_time;
		long current;
		long previous;

		int target_fps;
		int current_fps;
		int frame_count;
	} Time;

	
};

struct Image {
	void *data;
	int  width;
	int  height;
	int  pitch;
	int  format;
	/* int  mipmap; */
};


// window
void init_window(string title, int width, int height, Allocator *alloc);
int  poll_event(Event *event);
void surface_draw(void);
Vector2 get_mouse_position(void);

void begin(void);
void end(void);
void clear_background(Color color);

// util
Vector2 vec2(float x, float y);
void    set_fps(unsigned int fps);
unsigned int get_fps(void);

// time
unsigned long get_tick_ns(void);
void     delay_ns(long ns);
#endif /* CORE_H */
