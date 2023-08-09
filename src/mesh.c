#include <stdio.h>
#include <assert.h>
#include "array.h"
#include "mesh.h"
#include "texture.h"

#define MAX_NUM_MESHES 10

static mesh_t meshes[MAX_NUM_MESHES]; // mesh values automatically initialised to NULL pointers or zeroes
static int mesh_count = 0;

void load_mesh_obj_data(char *pathname, mesh_t *mesh)
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
            array_push(mesh->vertices, vertex);
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
            // face.colour = 0xFF0DC7FA;
            face.colour = 0xFFFFFFFF;
            array_push(mesh->faces, face);
        };
    };

    array_free(tex_coords);
    fclose(file);
};

void load_mesh_png_data(char *pathname, mesh_t *mesh)
{
    upng_t *png_image = upng_new_from_file(pathname);
    if (png_image != NULL)
    {
        upng_decode(png_image);
        if (upng_get_error(png_image) == UPNG_EOK)
        {
            mesh->texture = png_image;
        }
    }
};

void load_mesh(char *obj_pathname, char *png_pathname, vec3_t scale, vec3_t rotation, vec3_t translation)
{
    load_mesh_obj_data(obj_pathname, &meshes[mesh_count]);
    load_mesh_png_data(png_pathname, &meshes[mesh_count]);
    meshes[mesh_count].scale = scale;
    meshes[mesh_count].rotation = rotation;
    meshes[mesh_count].translation = translation;
    mesh_count++;
};

// wasm-specific functions
#ifdef __EMSCRIPTEN__
#include <string.h>

char *ALL_MESH_OBJ_PATHS[7] = {
    "./assets/cube.obj",
    "./assets/crab.obj",
    "./assets/drone.obj",
    "./assets/efa.obj",
    "./assets/f22.obj",
    "./assets/f117.obj",
    "./assets/sphere.obj",
};

char *ALL_MESH_PNG_PATHS[7] = {
    "./assets/cube.png",
    "./assets/crab.png",
    "./assets/drone.png",
    "./assets/efa.png",
    "./assets/f22.png",
    "./assets/f117.png",
    "./assets/sphere.png",
};

mesh_t init_meshes[MAX_NUM_MESHES];

void load_single_mesh(char *mesh_name)
{
    char *obj_pathname, *png_pathname;
    if (asprintf(&obj_pathname, "./assets/%s.obj", mesh_name) < 0)
    {
        fprintf(stderr, "Error formatting mesh obj path name\n");
        return;
    }
    if (asprintf(&png_pathname, "./assets/%s.png", mesh_name) < 0)
    {
        fprintf(stderr, "Error formatting mesh png path name\n");
        return;
    }

    // clear existing mesh(es) before loading new
    if (mesh_count > 0)
    {
        free_mesh_resources();
        mesh_count = 0;
        memcpy(meshes, init_meshes, sizeof(meshes));
    }

    load_mesh(obj_pathname, png_pathname, (vec3_t){1, 1, 1}, (vec3_t){0, 0, 0}, (vec3_t){0, 0, 4});
};
void load_all_meshes(void)
{
    free_mesh_resources();
    mesh_count = 0;
    memcpy(meshes, init_meshes, sizeof(meshes));

    // handle first mesh separate due to specific centering issues
    load_mesh(ALL_MESH_OBJ_PATHS[0], ALL_MESH_PNG_PATHS[0], (vec3_t){1, 1, 1}, (vec3_t){0, 0, 0}, (vec3_t){1, 0, 2});

    for (int i = 1; i < 7; i++)
    {
        bool even = i % 2 == 0;
        vec3_t translation = {0, 0, 0};
        if (even)
        {
            translation.x = i * 3;
            translation.z = i * 4;
        }
        else
        {
            translation.x = -i * 3;
            translation.z = -i * 4;
        }
        load_mesh(ALL_MESH_OBJ_PATHS[i], ALL_MESH_PNG_PATHS[i], (vec3_t){1, 1, 1}, (vec3_t){0, 0, 0}, translation);
    }
};
bool mesh_has_texture(void)
{
    return meshes[0].texture != NULL;
};

#endif

mesh_t *get_mesh(int index)
{
    return &meshes[index];
};
int get_mesh_count(void)
{
    return mesh_count;
}

void free_mesh_resources(void)
{
    for (int m = 0; m < mesh_count; m++)
    {
        array_free(meshes[m].vertices);
        array_free(meshes[m].faces);
        upng_free(meshes[m].texture);
    }
}