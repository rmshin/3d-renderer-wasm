#ifndef MESH_H
#define MESH_H

#include <stdbool.h>
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
// wasm-specific functions
#ifdef __EMSCRIPTEN__
bool mesh_has_texture(void);
void load_single_mesh(char *mesh_name);
void load_all_meshes(void);
#endif

mesh_t *get_mesh(int index);
int get_mesh_count(void);

void free_mesh_resources(void);

#endif