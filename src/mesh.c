#include <stdio.h>
#include <assert.h>
#include "array.h"
#include "mesh.h"

mesh_t mesh = {
    .vertices = NULL,
    .faces = NULL,
    .rotation = {0, 0, 0},
    .translation = {0, 0, 0},
    .scale = {1.0, 1.0, 1.0},
};

vec3_t cube_vertices[N_CUBE_VERTICES] = {
    {.x = -1, .y = -1, .z = -1}, // 1
    {.x = -1, .y = 1, .z = -1},  // 2
    {.x = 1, .y = 1, .z = -1},   // 3
    {.x = 1, .y = -1, .z = -1},  // 4
    {.x = 1, .y = 1, .z = 1},    // 5
    {.x = 1, .y = -1, .z = 1},   // 6
    {.x = -1, .y = 1, .z = 1},   // 7
    {.x = -1, .y = -1, .z = 1},  // 8
};

face_t cube_faces[N_CUBE_FACES] = {
    // front
    {.a = 1, .b = 2, .c = 3, .colour = 0xFFFF0000},
    {.a = 1, .b = 3, .c = 4, .colour = 0xFF00FF00},
    // right
    {.a = 4, .b = 3, .c = 5, .colour = 0xFFFFFF00},
    {.a = 4, .b = 5, .c = 6, .colour = 0xFFFF00FF},
    // back
    {.a = 6, .b = 5, .c = 7, .colour = 0xFF0000FF},
    {.a = 6, .b = 7, .c = 8, .colour = 0xFF00FFFF},
    // left
    {.a = 8, .b = 7, .c = 2, .colour = 0xFFFFFFFF},
    {.a = 8, .b = 2, .c = 1, .colour = 0xFF00FF00},
    // top
    {.a = 2, .b = 7, .c = 5, .colour = 0xFF00FFFF},
    {.a = 2, .b = 5, .c = 3, .colour = 0xFFFF00FF},
    // bottom
    {.a = 6, .b = 8, .c = 1, .colour = 0xFF00FF00},
    {.a = 6, .b = 1, .c = 4, .colour = 0xFFF000FF},
};

void load_cube_mesh_data(void)
{
    for (int i = 0; i < N_CUBE_VERTICES; i++)
    {
        array_push(mesh.vertices, cube_vertices[i]);
    };
    for (int i = 0; i < N_CUBE_FACES; i++)
    {
        array_push(mesh.faces, cube_faces[i]);
    };
    mesh.rotation = (vec3_t){0, 0, 0};
};

void load_obj_file_data(char *pathname)
{
    FILE *file;
    file = fopen(pathname, "r");
    assert(file);

    char line[255];

    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (line[0] == 'v' && line[1] == ' ')
        {

            vec3_t vertex;
            sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
            array_push(mesh.vertices, vertex);
        }
        else if (line[0] == 'f' && line[1] == ' ')
        {

            face_t face;
            sscanf(line, "f %d/%*d/%*d %d/%*d/%*d %d/%*d/%*d", &face.a, &face.b, &face.c);
            face.colour = 0xFFFAC70D;
            array_push(mesh.faces, face);
        };
    };
    mesh.rotation = (vec3_t){0, 0, 0};

    fclose(file);
};