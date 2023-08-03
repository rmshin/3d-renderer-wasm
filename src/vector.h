#ifndef VECTOR_H
#define VECTOR_H

#include <stdbool.h>

typedef struct
{
    float x;
    float y;
} vec2_t;

typedef struct
{
    float x;
    float y;
    float z;
} vec3_t;

typedef struct
{
    float x;
    float y;
    float z;
    float w;
} vec4_t;

// 3d -> 2d vector projection functions
vec2_t ortho_project(vec3_t point);
vec2_t perspect_project(vec3_t point);

// 2d vector functions
float vec2_length(vec2_t v);
vec2_t vec2_add(vec2_t v1, vec2_t v2);
vec2_t vec2_subtract(vec2_t v1, vec2_t v2);
vec2_t vec2_mul(vec2_t v, float factor);
vec2_t vec2_div(vec2_t v, float factor);
float vec2_dot(vec2_t v1, vec2_t v2);
void vec2_normalise(vec2_t *v);

// 3d vector functions
float vec3_length(vec3_t v);
vec3_t vec3_clone(vec3_t *v);
vec3_t vec3_add(vec3_t v1, vec3_t v2);
vec3_t vec3_subtract(vec3_t v1, vec3_t v2);
vec3_t vec3_mul(vec3_t v, float factor);
vec3_t vec3_div(vec3_t v, float factor);
vec3_t vec3_cross(vec3_t v1, vec3_t v2);
float vec3_dot(vec3_t v1, vec3_t v2);
void vec3_normalise(vec3_t *v);

vec3_t vec3_rotate_x(vec3_t v, float angle);
vec3_t vec3_rotate_y(vec3_t v, float angle);
vec3_t vec3_rotate_z(vec3_t v, float angle);

bool cull_face(vec4_t face_vertices[3]);

// 4d vector functions
float vec4_dot(vec4_t v1, vec4_t v2);

// conversion functions
vec4_t vec4_from_vec3(vec3_t v);
vec3_t vec3_from_vec4(vec4_t v);
vec2_t vec2_from_vec4(vec4_t v);

#endif