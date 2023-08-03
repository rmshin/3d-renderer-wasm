#ifndef MESH_H
#define MESH_H

#include "vector.h"
#include "triangle.h"
#include "upng.h"

typedef struct
{
    vec3_t *vertices; // mesh dynamic vertices array
    face_t *faces;    // mesh dynamic faces array
    upng_t *texture;  // mesh PNG texture pointer
    vec3_t scale;
    vec3_t rotation;
    vec3_t translation;
} mesh_t;

void load_mesh(char *mesh_path, char *texture_path, vec3_t scale, vec3_t rotation, vec3_t translation);

mesh_t *get_mesh(int index);
int get_mesh_count(void);

void free_mesh_resources(void);

#endif