#ifndef CAMERA_H
#define CAMERA_H

#include "vector.h"
#include "matrix.h"

typedef struct
{
    vec3_t position;
    vec3_t direction;
    vec3_t forward_velocity;
    float yaw_angle;
} camera_t;

mat4_t look_at(vec3_t target_pos, vec3_t camera_pos, vec3_t up_direction);

extern camera_t camera;

#endif