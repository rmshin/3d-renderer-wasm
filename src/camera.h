#ifndef CAMERA_H
#define CAMERA_H

#include "vector.h"
#include "matrix.h"

#define MOUSE_SENSITIVITY 0.1f

typedef struct
{
    vec3_t position;
    vec3_t direction;
    vec3_t forward_velocity;
    float yaw;
    float pitch;
} camera_t;

void init_camera(vec3_t pos, vec3_t direction);
vec3_t get_camera_lookat_target(void);

vec3_t get_camera_position(void);
vec3_t get_camera_direction(void);
vec3_t get_camera_forward_velocity(void);
float get_camera_yaw(void);
float get_camera_pitch(void);
void set_camera_position(vec3_t p);
void set_camera_direction(vec3_t d);
void set_camera_forward_velocity(vec3_t v);
void rotate_camera_yaw(float y);
void rotate_camera_pitch(float p);
void reset_camera(void);

#endif