#include <stdio.h>
#include <math.h>
#include "clipping.h"
#include "vector.h"
#include "texture.h"

#define NUM_PLANES 6

plane_t frustum_planes[NUM_PLANES];

void init_frustum_planes(float fov_x, float fov_y, float znear, float zfar)
{
    // top = { 0, -cos (fov / 2) , sin(fov / 2)}
    // bottom = { 0, cos (fov / 2), sin(fov / 2)}
    // left = { cos (fov / 2), 0 , sin(fov / 2)}
    // right = { -cos (fov / 2), 0 , sin(fov / 2)}
    // near = {0, 0, znear}, { 0, 0, 1}
    // far = { 0, 0, zfar}, {0, 0, -1}

    vec3_t p = {0, 0, 0};
    vec3_t zn = {0, 0, znear};
    vec3_t zf = {0, 0, zfar};
    float cy = cos(fov_y / 2.0);
    float sy = sin(fov_y / 2.0);
    float cx = cos(fov_x / 2.0);
    float sx = sin(fov_x / 2.0);

    frustum_planes[TOP_PLANE] = (plane_t){p, (vec3_t){0, -cy, sy}};
    frustum_planes[BOTTOM_PLANE] = (plane_t){p, (vec3_t){0, cy, sy}};
    frustum_planes[LEFT_PLANE] = (plane_t){p, (vec3_t){cx, 0, sx}};
    frustum_planes[RIGHT_PLANE] = (plane_t){p, (vec3_t){-cx, 0, sx}};
    frustum_planes[NEAR_PLANE] = (plane_t){zn, (vec3_t){0, 0, 1}};
    frustum_planes[FAR_PLANE] = (plane_t){zf, (vec3_t){0, 0, -1}};
};

polygon_t create_poly_from_triangle(vec3_t a, vec3_t b, vec3_t c, tex2_t a_uv, tex2_t b_uv, tex2_t c_uv)
{
    polygon_t poly = {
        .vertices = {a, b, c},
        .tex_coords = {a_uv, b_uv, c_uv},
        .num_vertices = 3,
    };
    return poly;
};

// linear interpolation
float float_lerp(float a, float b, float factor)
{
    return a + factor * (b - a);
};

void clip_poly_against_plane(polygon_t *poly, int plane)
{
    vec3_t plane_point = frustum_planes[plane].point;
    vec3_t plane_normal = frustum_planes[plane].normal;

    vec3_t inside_vertices[MAX_POLY_VERTICES];
    tex2_t inside_tex_coords[MAX_POLY_VERTICES];
    int num_inside_vertices = 0;

    // check if poly vertices intersect
    vec3_t *current_vertex = &poly->vertices[0];
    vec3_t *previous_vertex = &poly->vertices[poly->num_vertices - 1];
    // handle textures too
    tex2_t *current_tex_coord = &poly->tex_coords[0];
    tex2_t *previous_tex_coord = &poly->tex_coords[poly->num_vertices - 1];

    float current_dot = vec3_dot(vec3_subtract(*current_vertex, plane_point), plane_normal);
    float previous_dot = vec3_dot(vec3_subtract(*previous_vertex, plane_point), plane_normal);

    // full circle completed when current vertex reaches just one past end of array
    while (current_vertex != &poly->vertices[poly->num_vertices])
    {
        current_dot = vec3_dot(vec3_subtract(*current_vertex, plane_point), plane_normal);
        if (current_dot * previous_dot < 0)
        {
            // I = Q1 + t(Q2 - Q1), where t = dotQ1 / (dotQ1 - dotQ2)
            float t = previous_dot / (previous_dot - current_dot);
            vec3_t intersection_point = {
                .x = float_lerp(previous_vertex->x, current_vertex->x, t),
                .y = float_lerp(previous_vertex->y, current_vertex->y, t),
                .z = float_lerp(previous_vertex->z, current_vertex->z, t),
            };

            tex2_t interpolated_tex_coord = {
                .u = float_lerp(previous_tex_coord->u, current_tex_coord->u, t),
                .v = float_lerp(previous_tex_coord->v, current_tex_coord->v, t),
            };

            inside_vertices[num_inside_vertices] = intersection_point; // TO CHECK: has to be cloned?
            inside_tex_coords[num_inside_vertices] = interpolated_tex_coord;
            num_inside_vertices++;
        }
        if (current_dot > 0)
        {
            inside_vertices[num_inside_vertices] = vec3_clone(current_vertex);
            inside_tex_coords[num_inside_vertices] = tex2_clone(current_tex_coord);
            num_inside_vertices++;
        }

        previous_dot = current_dot;
        previous_tex_coord = current_tex_coord;
        current_tex_coord++;
        previous_vertex = current_vertex;
        current_vertex++;
    }
    for (int i = 0; i < num_inside_vertices; i++)
    {
        poly->vertices[i] = vec3_clone(&inside_vertices[i]);
        poly->tex_coords[i] = tex2_clone(&inside_tex_coords[i]);
    }
    poly->num_vertices = num_inside_vertices;
};

void clip_polygon(polygon_t *poly)
{
    clip_poly_against_plane(poly, TOP_PLANE);
    clip_poly_against_plane(poly, BOTTOM_PLANE);
    clip_poly_against_plane(poly, LEFT_PLANE);
    clip_poly_against_plane(poly, RIGHT_PLANE);
    clip_poly_against_plane(poly, NEAR_PLANE);
    clip_poly_against_plane(poly, FAR_PLANE);
};

void create_triangles_from_poly(polygon_t *poly, triangle_t triangles[])
{
    for (int i = 0; i < poly->num_vertices - 2; i++)
    {
        int idx0 = 0;
        int idx1 = i + 1;
        int idx2 = i + 2;

        triangles[i].points[0] = vec4_from_vec3(poly->vertices[idx0]);
        triangles[i].points[1] = vec4_from_vec3(poly->vertices[idx1]);
        triangles[i].points[2] = vec4_from_vec3(poly->vertices[idx2]);

        triangles[i].tex_coords[0] = poly->tex_coords[idx0];
        triangles[i].tex_coords[1] = poly->tex_coords[idx1];
        triangles[i].tex_coords[2] = poly->tex_coords[idx2];
    }
};