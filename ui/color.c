#include "color.h"
#include "base/types.h"
#include <stdint.h>

U32 color_to_int_rgba(Color color)
{
	/* return (color.r << 24) | (color.g << 16) | (color.b << 8) | color.a; */
	return (color.r << 24) | (color.g << 16) | (color.b << 8) | color.a;
}

U32 color_to_int_argb(Color color)
{
	return (color.a << 24) | (color.r << 16) | (color.g << 8) | color.b;
}
