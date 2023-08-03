#include "camera.h"
#include "vector.h"

camera_t camera = {
    .position = {0, 0, 0},
    .direction = {0, 0, 1},
    .forward_velocity = {0, 0, 0},
    .yaw_angle = 0.0,
};

mat4_t look_at(vec3_t target_pos, vec3_t camera_pos, vec3_t up_direction)
{
    vec3_t z = vec3_subtract(target_pos, camera_pos);
    vec3_normalise(&z);
    vec3_t x = vec3_cross(up_direction, z);
    vec3_normalise(&x);
    vec3_t y = vec3_cross(z, x);

    mat4_t view_mat = {
        .m = {
            {x.x, x.y, x.z, -vec3_dot(x, camera_pos)},
            {y.x, y.y, y.z, -vec3_dot(y, camera_pos)},
            {z.x, z.y, z.z, -vec3_dot(z, camera_pos)},
            {0, 0, 0, 1.0},
        },
    };
    return view_mat;
};