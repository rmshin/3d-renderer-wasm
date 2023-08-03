#include "camera.h"
#include "vector.h"

camera_t camera;

void init_camera(vec3_t pos, vec3_t direction)
{
    camera.position = pos;
    camera.direction = direction;
    camera.forward_velocity = (vec3_t){0, 0, 0};
    camera.yaw = 0.0;
    camera.pitch = 0.0;
};

vec3_t get_camera_lookat_target(void)
{
    vec3_t target = {0, 0, 1.0};

    mat4_t camera_yaw_rotation = mat4_make_rotation_y(camera.yaw);
    mat4_t camera_pitch_rotation = mat4_make_rotation_x(camera.pitch);
    mat4_t camera_rotation = mat4_mul_mat4(camera_yaw_rotation, camera_pitch_rotation);

    // update camera direction based on rotation
    vec4_t camera_direction = mat4_mul_vec4(camera_rotation, vec4_from_vec3(target));
    camera.direction = vec3_from_vec4(camera_direction);

    target = vec3_add(camera.position, camera.direction);

    return target;
};

vec3_t get_camera_position(void)
{
    return camera.position;
};
vec3_t get_camera_direction(void)
{
    return camera.direction;
};
vec3_t get_camera_forward_velocity(void)
{
    return camera.forward_velocity;
};
float get_camera_yaw(void)
{
    return camera.yaw;
};
float get_camera_pitch(void)
{
    return camera.pitch;
};
void set_camera_position(vec3_t p)
{
    camera.position = p;
};
void set_camera_direction(vec3_t d)
{
    camera.direction = d;
};
void set_camera_forward_velocity(vec3_t v)
{
    camera.forward_velocity = v;
};
void rotate_camera_yaw(float y)
{
    camera.yaw += y;
};
void rotate_camera_pitch(float p)
{
    camera.pitch += p;
};