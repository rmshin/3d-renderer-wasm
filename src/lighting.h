#ifndef LIGHTING_H
#define LIGHTING_H

#include <stdint.h>
#include "vector.h"

typedef struct
{
    vec3_t direction;
} light_t;

void init_light(vec3_t direction);
vec3_t get_light_direction(void);
uint32_t light_apply_intensity(uint32_t colour, float percentage_factor);
float calc_shading_factor(vec4_t face_vertices[3], vec3_t light_pos);

#endif