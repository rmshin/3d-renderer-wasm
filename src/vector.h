#ifndef VECTOR_H
#define VECTOR_H

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

#define N_POINTS (9 * 9 * 9)
vec3_t cube_points[N_POINTS];

void initialize_cube_points(void);
vec2_t ortho_project(vec3_t point);
vec2_t perspect_project(vec3_t point);
vec3_t vec3_rotate_x(vec3_t v, float angle);
vec3_t vec3_rotate_y(vec3_t v, float angle);
vec3_t vec3_rotate_z(vec3_t v, float angle);

#endif