#include <math.h>
#include "vector.h"

float fov_factor = 640;

void initialize_cube_points(void)
{
    int point_count = 0;
    // -1 to 1 across x, y, z
    // move 0.25 per point (9 points each direction)
    for (float z = 1.0; z >= -1.0; z -= 0.25)
    {
        for (float y = 1.0; y >= -1.0; y -= 0.25)
        {
            for (float x = 1.0; x >= -1.0; x -= 0.25)
            {
                vec3_t new_point = {x, y, z};
                cube_points[point_count++] = new_point;
            };
        };
    };
};

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
