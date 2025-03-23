#include "color.h"
#include <stdint.h>

uint32_t color_to_int(Color color)
{
	return (color.r << 24) | (color.g << 16) | (color.b << 8) | color.a;
}

uint32_t color_ARGB(Color color)
{
	return (color.a << 24) | (color.r << 16) | (color.g << 8) | (color.b);
}
