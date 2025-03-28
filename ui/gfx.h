#ifndef GFX_H
#define GFX_H

#include "color.h"
#include "core.h"

Color gfx_blend_color(Color src, Color dest);
Color gfx_blend_color_fast(Color src, Color dest);
void gfx_draw_pixel(int x, int y, Color color);
void gfx_clear_background(Color color);
void gfx_draw_rectangle(int x, int y, int width, int height, Color color);
void gfx_draw_rectangle_outline(int x, int y, int width, int height, Color color);
void gfx_draw_rectangle_core(Rectangle rectangle, Vector2 origin, float rotation, Color color);
void gfx_draw_line(Vector2 start, Vector2 end, Color color);

void gfx_draw_image(unsigned char *buf, int width, int height, int channels);

#endif  /* GFX_H */
