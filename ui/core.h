#ifndef CORE_H
#define CORE_H

#include <stdbool.h>
#include "base/allocator.h"
#include "base/types.h"
#include "color.h"
#include "keycode.h"

#define FPS_UNCAPPED -1

#define MAX_KEY_STATE   512
#define MAX_MOUSE_STATE 16
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define MS_PER_SECOND 1000
#define US_PER_SECOND 1000000
#define NS_PER_SECOND 1000000000LL
#define NS_PER_MS     1000000
#define NS_PER_US     1000

/* #define SECONDS_TO_NS(S)    (((uint64_t)(S)) * NS_PER_SECOND) */
/* #define NS_TO_SECONDS(NS)   ((NS) / NS_PER_SECOND) */
/* #define MS_TO_NS(MS)        (((uint64_t)(MS)) * NS_PER_MS) */
/* #define NS_TO_MS(NS)        ((NS) / NS_PER_MS) */
/* #define US_TO_NS(US)        (((uint64_t)(US)) * NS_PER_US) */
/* #define NS_TO_US(NS)        ((NS) / NS_PER_US) */
/* #define NS_TO_SEC(NS)        ((NS) / NS_PER_US) */

typedef struct Vector2 Vector2;
typedef struct Vector3 Vector3;
typedef struct Rectangle Rectangle;
typedef struct Image Image;

#define VEC2(x, y) ((Vector2){(x), (y)})

struct Vector2 {
        float x;
        float y;
};

struct Vector3 {
        float x;
        float y;
        float z;
};

struct Rectangle {
	float x;
	float y;
	float width; 
	float height;
};

struct Image {
	void *pixels;
	U32   width;
	U32   height;
};

//
// WINDOW
//
typedef struct SRN_Context  SRN_Context;
typedef struct SRN_Window   SRN_Window;
typedef struct SRN_Surface  SRN_Surface;
typedef struct SRN_Time     SRN_Time;
typedef struct SRN_Keyboard SRN_Keyboard;
typedef struct SRN_Mouse    SRN_Mouse;

typedef struct InputState InputState;
#define PRESS   (bool)1
#define RELEASE (bool)0

typedef enum {
	PIXELFORMAT_RGBA32 = 0,
} PixelFormat;

//
// TODO: support/add the missing properties
struct SRN_Window {
	string title;
	U32    x, y;
	U32    width;
	U32    height;
};

struct SRN_Surface {
	void *pixels;
	U32   width;
	U32   height;
	U32   pitch;
	U64   flags;
	PixelFormat format;
};

struct SRN_Keyboard {
	U32 curr_state[MAX_KEY_STATE];
	U32 prev_state[MAX_KEY_STATE];
};

struct SRN_Mouse {
	Vector2 curr_pos;
	Vector2 prev_pos;
	U8 curr_state[MAX_MOUSE_STATE];
	U8 prev_state[MAX_MOUSE_STATE];
};

struct SRN_Time {
	double current;
	double previous;
	double update;   // time to update the frame buffer (ns)
	double draw;     // time to draw to the frame buffer to the screen/window (ns)
	double frame;    // delta (ns)
	double target;   // target time per frame (ns)
	U32    frame_count;
};

struct SRN_Context {
	Allocator    *allocator;
	Allocator    *frame_allocator; // NOTE: replace when arena temporary allocator is implemented
	SRN_Window   Window;
	SRN_Surface  Surface;
	SRN_Time     Time;
	SRN_Mouse    Mouse;
	SRN_Keyboard Keyboard;
};

extern SRN_Context CONTEXT;

// WINDOW
int  init_window(string title, int width, int height, Allocator *alloc);
void deinit_window(void);
void BEGIN(void);
void END(void);
int  should_close(void);

// UTIL FUNCTIONS
void    clear_background(Color color);
void    set_fps(U32 fps);
U32     get_fps(void);
float   get_frame_time(void);
Vector2 get_mouse_position(void);
bool    is_mouse_btn_press(SRN_MOUSE key);
bool    is_mouse_btn_released(SRN_MOUSE key);
bool    is_mouse_btn_down(SRN_MOUSE key);
bool    is_mouse_btn_up(SRN_MOUSE key);
float   lerp(float v0, float v1, float t);
bool    mouse_in_rect(Vector2 pos, Rectangle rect);

// TIMING FUNCTIONS
void time_delay(uint64_t ns);
U64  time_get_ticks(void);

#endif /* CORE_H */
