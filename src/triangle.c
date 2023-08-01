#include <math.h>
#include "util.h"
#include "triangle.h"
#include "display.h"

void draw_triangle(triangle_t triangle, uint32_t colour)
{
    draw_line(triangle.points[0].x, triangle.points[0].y, triangle.points[1].x, triangle.points[1].y, colour);
    draw_line(triangle.points[1].x, triangle.points[1].y, triangle.points[2].x, triangle.points[2].y, colour);
    draw_line(triangle.points[2].x, triangle.points[2].y, triangle.points[0].x, triangle.points[0].y, colour);
};

void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int mx, int my, uint32_t colour)
{
    float slope_1 = (float)(x1 - x0) / (y1 - y0);
    float slope_2 = (float)(mx - x0) / (my - y0);
    float start_x = x0;
    float end_x = x0;
    for (int i = y0; i <= my; i++)
    {
        draw_line(round(start_x), i, round(end_x), i, colour);
        start_x += slope_1;
        end_x += slope_2;
    };
};

void fill_flat_top_triangle(int x1, int y1, int mx, int my, int x2, int y2, uint32_t colour)
{
    float slope_1 = (float)(x1 - x2) / (y1 - y2);
    float slope_2 = (float)(mx - x2) / (my - y2);
    float start_x = x2;
    float end_x = x2;
    for (int i = y2; i >= y1; i--)
    {
        draw_line(round(start_x), i, round(end_x), i, colour);
        start_x -= slope_1;
        end_x -= slope_2;
    };
};

void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t colour)
{
    // need to sort vertices by ascending y (y0 -> y1 -> y2)
    if (y0 > y1)
    {
        swap(&y0, &y1);
        swap(&x0, &x1);
    };
    if (y1 > y2)
    {
        swap(&y1, &y2);
        swap(&x1, &x2);
    };
    if (y0 > y1)
    {
        swap(&y0, &y1);
        swap(&x0, &x1);
    };

    if (y1 == y2)
    {
        fill_flat_bottom_triangle(x0, y0, x1, y1, x2, y2, colour);
    }
    else if (y0 == y1)
    {
        fill_flat_top_triangle(x1, y1, x0, y0, x2, y2, colour);
    }
    else
    {
        int my = y1;
        int mx = (float)((x2 - x0) * (y1 - y0)) / (float)(y2 - y0) + x0;

        fill_flat_bottom_triangle(x0, y0, x1, y1, mx, my, colour);
        fill_flat_top_triangle(x1, y1, mx, my, x2, y2, colour);
    };
};