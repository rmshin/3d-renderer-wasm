#include <assert.h>
#include "util.h"
#include "array.h"
#include "display.h"
#include "vector.h"
#include "matrix.h"
#include "mesh.h"
#include "lighting.h"

triangle_t *triangles_to_render = NULL;

vec3_t camera_position = {0, 0, 0};
mat4_t projection_matrix;

uint32_t prev_frame_time = 0;
bool is_running = false;

void setup(void)
{
    display_mode = DISPLAY_WIRE;
    cull_method = CULL_BACKFACE;

    colour_buffer = (uint32_t *)malloc(sizeof(uint32_t) * window_width * window_height);
    assert(colour_buffer);
    colour_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, window_width, window_height);
    assert(colour_buffer_texture);

    float fov = M_PI / 2.0;
    float aspect = (float)window_height / (float)window_width;
    projection_matrix = mat4_make_projection(fov, aspect, 1.0, 1000.0);

    // load_cube_mesh_data();
    load_obj_file_data("./assets/crab.obj");
};

void process_input(void)
{
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type)
    {
    case SDL_QUIT:
        is_running = false;
        break;
    case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_ESCAPE)
        {
            is_running = false;
        }
        else if (event.key.keysym.sym == SDLK_1)
        {
            display_mode = DISPLAY_WIRE_VERTEX;
        }
        else if (event.key.keysym.sym == SDLK_2)
        {
            display_mode = DISPLAY_WIRE;
        }
        else if (event.key.keysym.sym == SDLK_3)
        {
            display_mode = DISPLAY_FILL;
        }
        else if (event.key.keysym.sym == SDLK_4)
        {
            display_mode = DISPLAY_FILL_WIRE;
        }
        else if (event.key.keysym.sym == SDLK_c)
        {
            cull_method = CULL_BACKFACE;
        }
        else if (event.key.keysym.sym == SDLK_d)
        {
            cull_method = CULL_NONE;
        };
        break;
    }
};

void update(void)
{
    int time_to_wait = TARGET_FRAME_TIME - (SDL_GetTicks() - prev_frame_time);
    if (time_to_wait > 0 && time_to_wait <= TARGET_FRAME_TIME)
    {
        SDL_Delay(time_to_wait);
    };

    prev_frame_time = SDL_GetTicks();

    // initialise triangles array
    triangles_to_render = NULL;

    mesh.rotation.y += 0.02;
    // mesh.rotation.x += 0.03;
    // mesh.rotation.z += 0.02;
    // mesh.scale.x += 0.002;
    // mesh.scale.y += 0.002;
    // mesh.scale.z += 0.002;
    // mesh.translation.x += 0.05;
    // mesh.translation.y += 0.05;
    mesh.translation.z = 5.0;

    // more interesting transformations
    // t = SDL_GetTicks() * 0.0005;

    // mesh.rotation.x += 0.02;

    // mesh.scale.x = sin(t) + 1;
    // mesh.scale.y = sin(t) + 1;
    // mesh.scale.z = sin(t) + 1;

    // mesh.translation.x = tan(t) * 2;
    // mesh.translation.y = sin(t) * 2.2;

    mat4_t scale_matrix = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
    mat4_t rotx_matrix = mat4_make_rotation_x(mesh.rotation.x);
    mat4_t roty_matrix = mat4_make_rotation_y(mesh.rotation.y);
    mat4_t rotz_matrix = mat4_make_rotation_z(mesh.rotation.z);
    mat4_t translate_matrix = mat4_make_translation(mesh.translation.x, mesh.translation.y, mesh.translation.z);

    for (int i = 0; i < array_length(mesh.faces); i++)
    {
        face_t mesh_face = mesh.faces[i];
        vec3_t face_vertices[3] = {
            mesh.vertices[mesh_face.a - 1],
            mesh.vertices[mesh_face.b - 1],
            mesh.vertices[mesh_face.c - 1],
        };

        vec4_t transformed_vertices[3];
        for (int j = 0; j < 3; j++)
        {
            vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

            mat4_t world_matrix = scale_matrix;
            world_matrix = mat4_mul_mat4(rotx_matrix, world_matrix);
            world_matrix = mat4_mul_mat4(roty_matrix, world_matrix);
            world_matrix = mat4_mul_mat4(rotz_matrix, world_matrix);
            world_matrix = mat4_mul_mat4(translate_matrix, world_matrix);

            transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);
            transformed_vertices[j] = transformed_vertex;
        };

        if (cull_method == CULL_NONE || !cull_face(transformed_vertices, camera_position))
        {
            triangle_t projected_triangle;
            // for sorting faces by depth (painter's algorithm)
            float depth = 0.0;
            for (int j = 0; j < 3; j++)
            {
                // project & translate to center
                vec4_t projected = mat4_mul_vec4_project(projection_matrix, transformed_vertices[j]);
                projected.x *= window_width / 2.0;
                projected.y *= window_height / 2.0;
                projected.x += window_width / 2.0;
                projected.y += window_height / 2.0;
                projected_triangle.points[j] = (vec2_t){projected.x, projected.y};
                // sum all z values of vertices
                depth += transformed_vertices[j].z;
            };

            float shading_factor = calc_shading_factor(transformed_vertices, light_source.direction);
            projected_triangle.colour = light_apply_intensity(mesh_face.colour, shading_factor);

            projected_triangle.avg_depth = depth / 3.0;
            array_push(triangles_to_render, projected_triangle)
        };
    };
    // sort by average depth
    quick_sort_triangles(triangles_to_render, 0, array_length(triangles_to_render) - 1);
};

void draw_triangles(void)
{
    for (int i = 0; i < array_length(triangles_to_render); i++)
    {
        triangle_t triangle = triangles_to_render[i];

        if (display_mode == DISPLAY_FILL_WIRE || display_mode == DISPLAY_FILL)
        {
            draw_filled_triangle(
                triangle.points[0].x,
                triangle.points[0].y,
                triangle.points[1].x,
                triangle.points[1].y,
                triangle.points[2].x,
                triangle.points[2].y,
                triangle.colour);
        }
        if (display_mode == DISPLAY_WIRE || display_mode == DISPLAY_WIRE_VERTEX || display_mode == DISPLAY_FILL_WIRE)
        {
            draw_triangle(triangle, 0xFFFF0000);
        };
        if (display_mode == DISPLAY_WIRE_VERTEX)
        {
            draw_rect(triangle.points[0].x, triangle.points[0].y, 3, 3, 0xFFFAC70D);
            draw_rect(triangle.points[1].x, triangle.points[1].y, 3, 3, 0xFFFAC70D);
            draw_rect(triangle.points[2].x, triangle.points[2].y, 3, 3, 0xFFFAC70D);
        };
    };
};

void render(void)
{
    clear_colour_buffer(0xFF000000);
    draw_grid(0xFF3C3C3C, 20);
    draw_triangles();
    array_free(triangles_to_render);
    render_colour_buffer();

    SDL_RenderPresent(renderer);
};

void free_resources(void)
{
    array_free(mesh.faces);
    array_free(mesh.vertices);
    free(colour_buffer);
};

int main(void)
{
    // create an SDL window
    is_running = initialize_window();

    setup();
    while (is_running)
    {
        process_input();
        update();
        render();
    };

    destroy_window();
    free_resources();

    return 0;
};