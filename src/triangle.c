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

void draw_texel(
    int x, int y, uint32_t *texture,
    vec4_t point_a, vec4_t point_b, vec4_t point_c,
    tex2_t a_uv, tex2_t b_uv, tex2_t c_uv)
{
    vec2_t p = {x, y};
    vec2_t a = vec2_from_vec4(point_a);
    vec2_t b = vec2_from_vec4(point_b);
    vec2_t c = vec2_from_vec4(point_c);

    vec3_t bary_weights = barycentric_weights(a, b, c, p);

    float alpha = bary_weights.x;
    float beta = bary_weights.y;
    float gamma = bary_weights.z;

    // perspective correct interpolation
    // read Kok-Lim Low. Section 3
    float A = alpha * point_b.w * point_c.w;
    float B = beta * point_a.w * point_c.w;
    float C = gamma * point_a.w * point_b.w;

    float interpolated_u = (a_uv.u * A + b_uv.u * B + c_uv.u * C) / (A + B + C);
    float interpolated_v = (a_uv.v * A + b_uv.v * B + c_uv.v * C) / (A + B + C);

    int tex_x = abs((int)(interpolated_u * (texture_width - 1)));
    int tex_y = abs((int)(interpolated_v * (texture_height - 1)));

    draw_pixel(x, y, texture[tex_y * texture_width + tex_x]);
};

void draw_textured_triangle(
    int x0, int y0, float z0, float w0, float u0, float v0,
    int x1, int y1, float z1, float w1, float u1, float v1,
    int x2, int y2, float z2, float w2, float u2, float v2,
    uint32_t *texture)
{
    // need to sort vertices by ascending y (y0 -> y1 -> y2)
    if (y0 > y1)
    {
        swap(&y0, &y1);
        swap(&x0, &x1);
        float_swap(&z0, &z1);
        float_swap(&w0, &w1);
        float_swap(&u0, &u1);
        float_swap(&v0, &v1);
    };
    if (y1 > y2)
    {
        swap(&y1, &y2);
        swap(&x1, &x2);
        float_swap(&z1, &z2);
        float_swap(&w1, &w2);
        float_swap(&u1, &u2);
        float_swap(&v1, &v2);
    };
    if (y0 > y1)
    {
        swap(&y0, &y1);
        swap(&x0, &x1);
        float_swap(&z0, &z1);
        float_swap(&w0, &w1);
        float_swap(&u0, &u1);
        float_swap(&v0, &v1);
    };

    vec4_t point_a = {x0, y0, z0, w0};
    vec4_t point_b = {x1, y1, z1, w1};
    vec4_t point_c = {x2, y2, z2, w2};
    tex2_t a_uv = {u0, v0};
    tex2_t b_uv = {u1, v1};
    tex2_t c_uv = {u2, v2};

    // don't fully understand this part yet...
    float slope_1 = 0;
    float slope_2 = 0;
    if (y1 - y0 != 0)
        slope_1 = (float)(x1 - x0) / (y1 - y0);
    if (y2 - y0 != 0)
        slope_2 = (float)(x2 - x0) / (y2 - y0);

    if (y1 - y0 != 0) // check this line as well...
    {
        for (int y = y0; y <= y1; y++) // has to be y1 -> why???
        {
            float x_start = x1 + (y - y1) * slope_1;
            float x_end = x0 + (y - y0) * slope_2;

            if (x_end < x_start)
            {
                float_swap(&x_start, &x_end); // swap if x_start is to the right of x_end
            }

            for (int x = round(x_start); x < round(x_end); x++)
            {
                draw_texel(x, y, texture, point_a, point_b, point_c, a_uv, b_uv, c_uv);
            }
        }
    }

    slope_1 = 0;
    slope_2 = 0;
    if (y2 - y1 != 0)
        slope_1 = (float)(x2 - x1) / (y2 - y1);
    if (y2 - y0 != 0)
        slope_2 = (float)(x2 - x0) / (y2 - y0);

    if (y2 - y1 != 0)
    {
        for (int y = y1; y <= y2; y++)
        {
            float x_start = x1 + (y - y1) * slope_1;
            float x_end = x0 + (y - y0) * slope_2;

            if (x_end < x_start)
            {
                float_swap(&x_start, &x_end); // swap if x_start is to the right of x_end
            }

            for (int x = round(x_start); x < round(x_end); x++)
            {
                draw_texel(x, y, texture, point_a, point_b, point_c, a_uv, b_uv, c_uv);
            }
        }
    }
};

vec3_t barycentric_weights(vec2_t a, vec2_t b, vec2_t c, vec2_t p)
{
    vec2_t ac = vec2_subtract(c, a);
    vec2_t ab = vec2_subtract(b, a);
    vec2_t pc = vec2_subtract(c, p);
    vec2_t pb = vec2_subtract(b, p);
    vec2_t ap = vec2_subtract(p, a);

    float area_parallelogram_abc = ac.x * ab.y - ac.y * ab.x;
    float area_parallelogram_pbc = pc.x * pb.y - pc.y * pb.x;
    float area_parallelogram_apc = ac.x * ap.y - ac.y * ap.x;
    float alpha = area_parallelogram_pbc / area_parallelogram_abc;
    float beta = area_parallelogram_apc / area_parallelogram_abc;
    float gamma = 1 - alpha - beta;

    return (vec3_t){alpha, beta, gamma};
};