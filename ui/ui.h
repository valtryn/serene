#ifndef _UI_H
#define _UI_H

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/extensions/Xdbe.h>
#include <stdint.h>

#include "allocator.h"

typedef struct SRN_Context SRN_Context;
typedef struct SRN_X11 SRN_X11;

typedef struct Color Color;
typedef struct Vector2 Vector2;
typedef struct Vector3 Vector3;
typedef struct Rectangle Rectangle;
typedef struct Image Image;
typedef struct Widget Widget;

struct Color {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
};

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
	void *data;
	int  width;
	int  height;
	int  pitch;
	int  format;
	/* int  mipmap; */
};

typedef enum {
	X11     = 0,
	Wayland 
} PLATFORM;

extern SRN_Context context;

struct SRN_X11 {
        Display        *display;
        Window	        window;
        XImage         *image;
        GC              gc;
        XdbeBackBuffer  back_buffer;
	Image		*buffer;
};

struct SRN_Context {
	int	  hardware_accel;
	PLATFORM  platform;
	Allocator *allocator;

	struct {
		int width;
		int height;
		int min_width;
		int min_height;

		int full_screen;
	} Window;

	struct {
		Vector2 current_position;
		Vector2 previous_position;
	} Mouse;

	struct {
		// NOTE: values are in nanosec
		long current;
		long previous;
		int  target;

		long start;
		int current_fps;
		int  frame_count;
	} Time;
};

struct Widget {

	int x;
	int y;

	int width;
	int height;
	int min_width;
	int min_height;
	int padding;
	int margin;

	int border_width;

	struct {
		Color top;
		Color bottom;
		Color left;
		Color right;
	} BorderColor;

	// TODO: todo
	/* int hover; */
	/* int click; */
};
void srn_init_window(char *title, int width, int height, Allocator *allocator);
void srn_begin(void);
void srn_end(void);
void srn_set_fps(int fps);
int  srn_get_fps(void);
void srn_clear_background(Color color);
Vector2 srn_get_mouse_position(void);
void srn_draw_pixel(int x, int y, Color color);
void srn_draw_rectangle(int x, int y, int width, int height, Color color);
void srn_draw_rectangle_outline(int x, int y, int width, int height, Color color);
void srn_draw_line(int start_x, int start_y, int end_x, int end_y, Color color);
void srn_draw_line_ex(int x1, int y1, int x2, int y2, int thickness, int alignment, Color color);

// NOTE: use catppuccin colorscheme for now
#define SERENE_ROSEWATER (Color){245, 224, 220, 255} /* #f5e0dc */
#define SERENE_FLAMINGO  (Color){242, 205, 205, 255} /* #f2cdcd */
#define SERENE_PINK 	 (Color){245, 194, 231, 255} /* #f5c2e7 */
#define SERENE_MAUVE 	 (Color){203, 166, 247, 255} /* #cba6f7 */
#define SERENE_RED 	 (Color){243, 139, 168, 255} /* #f38ba8 */
#define SERENE_MAROON 	 (Color){235, 160, 172, 255} /* #eba0ac */
#define SERENE_PEACH 	 (Color){250, 179, 135, 255} /* #fab387 */
#define SERENE_YELLOW 	 (Color){249, 226, 175, 255} /* #f9e2af */
#define SERENE_GREEN 	 (Color){166, 227, 161, 255} /* #a6e3a1 */
#define SERENE_TEAL 	 (Color){148, 226, 213, 255} /* #94e2d5 */
#define SERENE_SKY 	 (Color){137, 220, 235, 255} /* #89dceb */
#define SERENE_SAPPHIRE  (Color){116, 199, 236, 255} /* #74c7ec */
#define SERENE_BLUE 	 (Color){137, 180, 250, 255} /* #89b4fa */
#define SERENE_LAVENDER  (Color){180, 190, 254, 255} /* #b4befe */
#define SERENE_TEXT 	 (Color){205, 214, 244, 255} /* #cdd6f4 */
#define SERENE_SUBTEXT1  (Color){186, 194, 222, 255} /* #bac2de */
#define SERENE_SUBTEXT0  (Color){166, 173, 200, 255} /* #a6adc8 */
#define SERENE_OVERLAY2  (Color){147, 153, 178, 255} /* #9399b2 */
#define SERENE_OVERLAY1  (Color){127, 132, 156, 255} /* #7f849c */
#define SERENE_OVERLAY0  (Color){108, 112, 134, 255} /* #6c7086 */
#define SERENE_SURFACE2  (Color){88,  91,  112, 255} /* #585b70 */
#define SERENE_SURFACE1  (Color){69,  71,  90,  255} /* #45475a */
#define SERENE_SURFACE0  (Color){49,  50,  68,  255} /* #313244 */
#define SERENE_BASE 	 (Color){30,  30,  46,  255} /* #1e1e2e */
#define SERENE_MANTLE 	 (Color){24,  24,  37,  255} /* #181825 */
#define SERENE_CRUST 	 (Color){17,  17,  27,  255} /* #11111b */

#endif /* _UI_H */
