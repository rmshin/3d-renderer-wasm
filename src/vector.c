#include <math.h>
#include <stdbool.h>
#include "vector.h"

float fov_factor = 640;

vec2_t ortho_project(vec3_t point)
{
    vec2_t projected = {.x = fov_factor * point.x, .y = fov_factor * point.y};
    return projected;
};
vec2_t perspect_project(vec3_t point)
{
    vec2_t projected = {.x = fov_factor * point.x / point.z, .y = fov_factor * point.y / point.z};
    return projected;
};

// 2d vector functions
float vec2_length(vec2_t v)
{
    return sqrt(v.x * v.x + v.y * v.y);
};
vec2_t vec2_add(vec2_t v1, vec2_t v2)
{
    return (vec2_t){
        .x = v1.x + v2.x,
        .y = v1.y + v2.y,
    };
};
vec2_t vec2_subtract(vec2_t v1, vec2_t v2)
{
    return (vec2_t){
        .x = v1.x - v2.x,
        .y = v1.y - v2.y,
    };
};
vec2_t vec2_mul(vec2_t v, float factor)
{
    return (vec2_t){v.x * factor, v.y * factor};
};
vec2_t vec2_div(vec2_t v, float factor)
{
    return (vec2_t){v.x / factor, v.y / factor};
};
float vec2_dot(vec2_t v1, vec2_t v2)
{
    return v1.x * v2.x + v1.y * v2.y;
};
void vec2_normalise(vec2_t *v)
{
    float vl = vec2_length(*v);
    v->x /= vl;
    v->y /= vl;
};

// 3d vector functions
float vec3_length(vec3_t v)
{
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
};
vec3_t vec3_add(vec3_t v1, vec3_t v2)
{
    return (vec3_t){
        .x = v1.x + v2.x,
        .y = v1.y + v2.y,
        .z = v1.z + v2.z,
    };
};
vec3_t vec3_subtract(vec3_t v1, vec3_t v2)
{
    return (vec3_t){
        .x = v1.x - v2.x,
        .y = v1.y - v2.y,
        .z = v1.z - v2.z,
    };
};
vec3_t vec3_mul(vec3_t v, float factor)
{
    return (vec3_t){v.x * factor, v.y * factor, v.z * factor};
};
vec3_t vec3_div(vec3_t v, float factor)
{
    return (vec3_t){v.x / factor, v.y / factor, v.z / factor};
};
vec3_t vec3_cross(vec3_t v1, vec3_t v2)
{
    return (vec3_t){
        .x = v1.y * v2.z - v1.z * v2.y,
        .y = v1.z * v2.x - v1.x * v2.z,
        .z = v1.x * v2.y - v1.y * v2.x,
    };
};
float vec3_dot(vec3_t v1, vec3_t v2)
{
    return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
};
void vec3_normalise(vec3_t *v)
{
    float vl = vec3_length(*v);
    v->x /= vl;
    v->y /= vl;
    v->z /= vl;
};

// rotation functions
vec3_t vec3_rotate_x(vec3_t v, float angle)
{
    float rotatedY = v.y * cos(angle) - v.z * sin(angle);
    float rotatedZ = v.y * sin(angle) + v.z * cos(angle);

    return (vec3_t){v.x, rotatedY, rotatedZ};
};
vec3_t vec3_rotate_y(vec3_t v, float angle)
{
    float rotatedX = v.x * cos(angle) - v.z * sin(angle);
    float rotatedZ = v.x * sin(angle) + v.z * cos(angle);

    return (vec3_t){rotatedX, v.y, rotatedZ};
};
vec3_t vec3_rotate_z(vec3_t v, float angle)
{
    float rotatedX = v.x * cos(angle) - v.y * sin(angle);
    float rotatedY = v.x * sin(angle) + v.y * cos(angle);

    return (vec3_t){rotatedX, rotatedY, v.z};
};

bool cull_face(vec3_t face_vertices[3], vec3_t camera_pos)
{
    // get cross product of vectors
    vec3_t cross = vec3_cross(vec3_subtract(face_vertices[1], face_vertices[0]), vec3_subtract(face_vertices[2], face_vertices[0]));
    vec3_normalise(&cross);
    // get camera pos vector
    vec3_t camera = vec3_subtract(camera_pos, face_vertices[0]);
    // dot product between cam pos vector & cross product vector
    float dot = vec3_dot(camera, cross);
    // return true if dot < 0, otherwise false
    return dot < 0;
};