#include "lighting.h"
#include "vector.h"

static light_t light_source;

void init_light(vec3_t direction)
{
    light_source.direction = direction;
};
vec3_t get_light_direction(void)
{
    return light_source.direction;
};

uint32_t light_apply_intensity(uint32_t colour, float percentage_factor)
{
    if (percentage_factor > 1)
    {
        percentage_factor = 1;
    }
    else if (percentage_factor < 0)
    {
        percentage_factor = 0;
    };

    uint32_t a = (colour & 0xFF000000);
    uint32_t r = (colour & 0x00FF0000) * percentage_factor;
    uint32_t g = (colour & 0x0000FF00) * percentage_factor;
    uint32_t b = (colour & 0x000000FF) * percentage_factor;

    uint32_t new_colour = a | (r & 0x00FF0000) | (g & 0x0000FF00) | (b & 0x000000FF);
    return new_colour;
};

float calc_shading_factor(vec4_t face_vertices[3], vec3_t light)
{
    vec3_t fv0, fv1, fv2;
    fv0 = vec3_from_vec4(face_vertices[0]);
    fv1 = vec3_from_vec4(face_vertices[1]);
    fv2 = vec3_from_vec4(face_vertices[2]);
    // get cross product of vectors
    vec3_t cross = vec3_cross(vec3_subtract(fv1, fv0), vec3_subtract(fv2, fv0));
    vec3_normalise(&cross);
    // vec3_normalise(&light);
    // dot product between light pos vector & cross product vector
    float dot = vec3_dot(cross, light);
    return -dot;
};