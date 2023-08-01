#include <stdio.h>
#include <assert.h>
#include "array.h"
#include "mesh.h"
#include "texture.h"

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
    {.a = 1, .b = 2, .c = 3, .a_uv = {0, 1}, .b_uv = {0, 0}, .c_uv = {1, 0}, .colour = 0xFFFFFFFF},
    {.a = 1, .b = 3, .c = 4, .a_uv = {0, 1}, .b_uv = {1, 0}, .c_uv = {1, 1}, .colour = 0xFFFFFFFF},
    // right
    {.a = 4, .b = 3, .c = 5, .a_uv = {0, 1}, .b_uv = {0, 0}, .c_uv = {1, 0}, .colour = 0xFFFFFFFF},
    {.a = 4, .b = 5, .c = 6, .a_uv = {0, 1}, .b_uv = {1, 0}, .c_uv = {1, 1}, .colour = 0xFFFFFFFF},
    // back
    {.a = 6, .b = 5, .c = 7, .a_uv = {0, 1}, .b_uv = {0, 0}, .c_uv = {1, 0}, .colour = 0xFFFFFFFF},
    {.a = 6, .b = 7, .c = 8, .a_uv = {0, 1}, .b_uv = {1, 0}, .c_uv = {1, 1}, .colour = 0xFFFFFFFF},
    // left
    {.a = 8, .b = 7, .c = 2, .a_uv = {0, 1}, .b_uv = {0, 0}, .c_uv = {1, 0}, .colour = 0xFFFFFFFF},
    {.a = 8, .b = 2, .c = 1, .a_uv = {0, 1}, .b_uv = {1, 0}, .c_uv = {1, 1}, .colour = 0xFFFFFFFF},
    // top
    {.a = 2, .b = 7, .c = 5, .a_uv = {0, 1}, .b_uv = {0, 0}, .c_uv = {1, 0}, .colour = 0xFFFFFFFF},
    {.a = 2, .b = 5, .c = 3, .a_uv = {0, 1}, .b_uv = {1, 0}, .c_uv = {1, 1}, .colour = 0xFFFFFFFF},
    // bottom
    {.a = 6, .b = 8, .c = 1, .a_uv = {0, 1}, .b_uv = {0, 0}, .c_uv = {1, 0}, .colour = 0xFFFFFFFF},
    {.a = 6, .b = 1, .c = 4, .a_uv = {0, 1}, .b_uv = {1, 0}, .c_uv = {1, 1}, .colour = 0xFFFFFFFF}};

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

    char line[512];

    tex2_t *tex_coords = NULL;
    while (fgets(line, sizeof(line), file) != NULL)
    {
        if (line[0] == 'v' && line[1] == ' ')
        {
            vec3_t vertex;
            sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
            array_push(mesh.vertices, vertex);
        }
        else if (line[0] == 'v' && line[1] == 't')
        {
            tex2_t tex_coord;
            sscanf(line, "vt %f %f%*s", &tex_coord.u, &tex_coord.v);
            array_push(tex_coords, tex_coord);
        }
        else if (line[0] == 'f' && line[1] == ' ')
        {
            int texture_indices[3];
            face_t face;
            sscanf(line, "f %d/%d/%*d %d/%d/%*d %d/%d/%*d", &face.a, &texture_indices[0], &face.b, &texture_indices[1], &face.c, &texture_indices[2]);
            face.a -= 1;
            face.b -= 1;
            face.c -= 1;
            face.a_uv = tex_coords[texture_indices[0] - 1];
            face.b_uv = tex_coords[texture_indices[1] - 1];
            face.c_uv = tex_coords[texture_indices[2] - 1];
            face.colour = 0xFF0DC7FA;
            array_push(mesh.faces, face);
        };
    };
    mesh.rotation = (vec3_t){0, 0, 0};

    array_free(tex_coords);
    fclose(file);
};