#include <math.h>
#include "matrix.h"
#include "vector.h"

mat4_t mat4_identity(void)
{
    mat4_t m = {.m = {{0}}};
    m.m[0][0] = 1;
    m.m[1][1] = 1;
    m.m[2][2] = 1;
    m.m[3][3] = 1;
    return m;
};

mat4_t mat4_make_scale(float sx, float sy, float sz)
{
    mat4_t m = mat4_identity();
    m.m[0][0] = sx;
    m.m[1][1] = sy;
    m.m[2][2] = sz;
    return m;
};

mat4_t mat4_make_translation(float tx, float ty, float tz)
{
    mat4_t m = mat4_identity();
    m.m[0][3] = tx;
    m.m[1][3] = ty;
    m.m[2][3] = tz;
    return m;
};

mat4_t mat4_make_rotation_x(float angle)
{
    float c = cos(angle);
    float s = sin(angle);

    mat4_t m = mat4_identity();
    m.m[1][1] = c;
    m.m[1][2] = -s;
    m.m[2][1] = s;
    m.m[2][2] = c;
    return m;
};
mat4_t mat4_make_rotation_y(float angle)
{
    float c = cos(angle);
    float s = sin(angle);

    mat4_t m = mat4_identity();
    m.m[0][0] = c;
    m.m[0][2] = s;
    m.m[2][0] = -s;
    m.m[2][2] = c;
    return m;
};
mat4_t mat4_make_rotation_z(float angle)
{
    float c = cos(angle);
    float s = sin(angle);

    mat4_t m = mat4_identity();
    m.m[0][0] = c;
    m.m[0][1] = -s;
    m.m[1][0] = s;
    m.m[1][1] = c;
    return m;
};

vec4_t mat4_mul_vec4(mat4_t m, vec4_t v)
{
    vec4_t result = {
        .x = (m.m[0][0] * v.x) + (m.m[0][1] * v.y) + (m.m[0][2] * v.z) + (m.m[0][3] * v.w),
        .y = (m.m[1][0] * v.x) + (m.m[1][1] * v.y) + (m.m[1][2] * v.z) + (m.m[1][3] * v.w),
        .z = (m.m[2][0] * v.x) + (m.m[2][1] * v.y) + (m.m[2][2] * v.z) + (m.m[2][3] * v.w),
        .w = (m.m[3][0] * v.x) + (m.m[3][1] * v.y) + (m.m[3][2] * v.z) + (m.m[3][3] * v.w),
    };

    return result;
};

mat4_t mat4_mul_mat4(mat4_t a, mat4_t b)
{
    mat4_t result;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            vec4_t row = {
                .x = a.m[i][0],
                .y = a.m[i][1],
                .z = a.m[i][2],
                .w = a.m[i][3],
            };
            vec4_t col = {
                .x = b.m[0][j],
                .y = b.m[1][j],
                .z = b.m[2][j],
                .w = b.m[3][j],
            };
            result.m[i][j] = vec4_dot(row, col);
        };
    };

    return result;
};

mat4_t mat4_make_projection(float fov, float aspect, float znear, float zfar)
{
    mat4_t m = {.m = {{0}}};
    m.m[0][0] = aspect * 1 / tan(fov / 2);
    m.m[1][1] = -1 / tan(fov / 2);
    m.m[2][2] = zfar / (zfar - znear);
    m.m[2][3] = -(zfar * znear / zfar - znear);
    m.m[3][2] = 1.0;
    return m;
};

vec4_t mat4_mul_vec4_project(mat4_t perspective_matrix, vec4_t v)
{
    vec4_t projected = mat4_mul_vec4(perspective_matrix, v);

    if (projected.w != 0.0)
    {
        projected.x /= projected.w;
        projected.y /= projected.w;
        projected.z /= projected.w;
    };

    return projected;
};