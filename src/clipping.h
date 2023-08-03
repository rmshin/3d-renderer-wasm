#ifndef CLIPPING_H
#define CLIPPING_H

#include <stdbool.h>
#include "vector.h"
#include "triangle.h"
#include "texture.h"

#define MAX_POLY_VERTICES 10
#define MAX_POLY_TRIANGLES 10

enum Plane
{
    TOP_PLANE,
    BOTTOM_PLANE,
    LEFT_PLANE,
    RIGHT_PLANE,
    NEAR_PLANE,
    FAR_PLANE,
};

typedef struct
{
    vec3_t point;
    vec3_t normal;
} plane_t;

typedef struct
{
    vec3_t vertices[MAX_POLY_VERTICES];
    tex2_t tex_coords[MAX_POLY_VERTICES];
    int num_vertices;
} polygon_t;

void init_frustum_planes(float fov_x, float fov_y, float znear, float zfar);
bool point_outside_plane(plane_t plane, vec3_t q);
void clip_polygon(polygon_t *poly);
polygon_t create_poly_from_triangle(vec3_t a, vec3_t b, vec3_t c, tex2_t a_uv, tex2_t b_uv, tex2_t c_uv);
void create_triangles_from_poly(polygon_t *poly, triangle_t triangles[]);

#endif