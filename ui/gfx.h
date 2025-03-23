#ifndef GFX_H
#define GFX_H

#include "color.h"

void gfx_draw_pixel(int x, int y, Color color);
void gfx_clear_background(Color color);
void gfx_draw_rectangle(int x, int y, int width, int height, Color color);
void gfx_draw_rectangle_outline(int x, int y, int width, int height, Color color);
void gfx_draw_line(int start_x, int start_y, int end_x, int end_y, Color color);

#endif  /* GFX_H */
