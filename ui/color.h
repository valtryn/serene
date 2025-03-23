#ifndef COLOR_H
#define COLOR_H

#include <stdint.h>

typedef struct Color Color;

struct Color {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
};

uint32_t color_ARGB(Color color);
uint32_t color_to_int(Color color);

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

#define SERENE_CLEAR_WHITE (Color){255,  255,  255,  255}
#define SERENE_CLEAR_BLACK (Color){0,  0,  0,  0}


#endif /* COLOR_H */
