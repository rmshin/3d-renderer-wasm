#include <assert.h>
#include "array.h"
#include "display.h"
#include "vector.h"
#include "mesh.h"

triangle_t *triangles_to_render = NULL;

vec3_t camera_position = {0, 0, -5};

uint32_t prev_frame_time = 0;
bool is_running = false;

void setup(void)
{
    colour_buffer = (uint32_t *)malloc(sizeof(uint32_t) * window_width * window_height);
    assert(colour_buffer);
    colour_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, window_width, window_height);
    assert(colour_buffer_texture);

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

    mesh.rotation.y += 0.01;
    mesh.rotation.x += 0.01;
    mesh.rotation.z += 0.01;

    for (int i = 0; i < array_length(mesh.faces); i++)
    {
        face_t mesh_face = mesh.faces[i];
        vec3_t face_vertices[3] = {
            mesh.vertices[mesh_face.a - 1],
            mesh.vertices[mesh_face.b - 1],
            mesh.vertices[mesh_face.c - 1],
        };

        triangle_t projected_triangle;

        for (int j = 0; j < 3; j++)
        {
            vec3_t transformed_vertex = vec3_rotate_x(face_vertices[j], mesh.rotation.x);
            transformed_vertex = vec3_rotate_y(transformed_vertex, mesh.rotation.y);
            transformed_vertex = vec3_rotate_z(transformed_vertex, mesh.rotation.z);
            transformed_vertex.z -= camera_position.z;

            // project & translate to center
            vec2_t projected = perspect_project(transformed_vertex);
            projected.x += window_width / 2;
            projected.y += window_height / 2;
            projected_triangle.points[j] = projected;
        };

        array_push(triangles_to_render, projected_triangle)
    };
};

void draw_triangles(void)
{
    for (int i = 0; i < array_length(triangles_to_render); i++)
    {
        triangle_t triangle = triangles_to_render[i];
        draw_rect(triangle.points[0].x, triangle.points[0].y, 3, 3, 0xFFFAC70D);
        draw_rect(triangle.points[1].x, triangle.points[1].y, 3, 3, 0xFFFAC70D);
        draw_rect(triangle.points[2].x, triangle.points[2].y, 3, 3, 0xFFFAC70D);
        draw_line(triangle.points[0].x, triangle.points[0].y, triangle.points[1].x, triangle.points[1].y, 0xFFFAC70D);
        draw_line(triangle.points[1].x, triangle.points[1].y, triangle.points[2].x, triangle.points[2].y, 0xFFFAC70D);
        draw_line(triangle.points[2].x, triangle.points[2].y, triangle.points[0].x, triangle.points[0].y, 0xFFFAC70D);
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