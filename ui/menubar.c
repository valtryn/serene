#include "ui.h"
#include "util.h"

void menu_bar(void)
{
	Widget widget = {0};

	widget.width  = context.Window.width;
	widget.height = percentage_of(2.8f, context.Window.width);
	widget.border_width = 1;
	widget.BorderColor.bottom = SERENE_BLUE;

	// NOTE: (HACK) draw a line as a bottom border for now
	for (int i = 0; i < widget.border_width; i++)
		srn_draw_line(0, widget.height + i, widget.width, widget.height + i, widget.BorderColor.bottom);

	
	// Draw the menu icon
	/* { */
	/* 	int x = 10; */
	/* 	int menu_width = 16 + x; */

	/* 	// TODO: use these */
	/* 	/1* int gap = 5; *1/ */
	/* 	/1* int menu_thickness = 3; *1/ */

	/* 	for (int i = 0; i < 2; i++) { */
	/* 		srn_draw_line(x, (widget.height/2) - 5 + i, menu_width, (widget.height/2) - 5 + i, SERENE_BLUE); */
	/* 		srn_draw_line(x, widget.height/2 + i, menu_width, widget.height/2 + i, SERENE_BLUE); */
	/* 		srn_draw_line(x, (widget.height/2) + 5 + i, menu_width, (widget.height/2) + 5 + i, SERENE_BLUE); */
	/* 	} */
	/* } */
	{
		// NOTE: stopped here
		int x = 10;
		int border = 3;
		int width = 12;
		// top
		srn_draw_line_ex(x, widget.height/2, x + 10 + width, widget.height/2, 2, 1, SERENE_BLUE);
		// middle
		srn_draw_line_ex(x, widget.height/2, x + 10 + width, widget.height/2, 2, 1, SERENE_BLUE);

/* 		srn_draw_line_ex(x, int y1, int x2, int y2, int thickness, int alignment, Color color) */
	}
}

