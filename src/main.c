#include <assert.h>
#include "display.h"
#include "vector.h"

vec2_t projected_points[N_POINTS];
vec3_t camera_position = {0, 0, -5};
vec3_t cube_rotation = {0, 0, 0};

uint32_t prev_frame_time = 0;
bool is_running = false;

void setup(void)
{
    colour_buffer = (uint32_t *)malloc(sizeof(uint32_t) * window_width * window_height);
    assert(colour_buffer);
    colour_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, window_width, window_height);
    assert(colour_buffer_texture);

    initialize_cube_points();
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

    cube_rotation.y += 0.01;
    cube_rotation.x += 0.01;
    cube_rotation.z += 0.01;
    for (int i = 0; i < N_POINTS; i++)
    {
        vec3_t point = cube_points[i];
        vec3_t rotatedY = vec3_rotate_y(point, cube_rotation.y);
        vec3_t rotatedXY = vec3_rotate_x(rotatedY, cube_rotation.x);
        vec3_t rotatedXYZ = vec3_rotate_z(rotatedXY, cube_rotation.z);
        rotatedXYZ.z -= camera_position.z;
        vec2_t projected = perspect_project(rotatedXYZ);
        projected_points[i] = projected;
    };
};

void transform_points(void) {}

void draw_points(vec2_t points[], uint32_t colour)
{
    for (int i = 0; i < N_POINTS; i++)
    {
        vec2_t point = points[i];
        draw_rect(point.x + window_width / 2, point.y + window_height / 2, 4, 4, colour);
    };
};

void render(void)
{
    clear_colour_buffer(0xFF000000);
    draw_grid(0xFF3C3C3C, 40);
    draw_points(projected_points, 0xFFFA736D);
    render_colour_buffer();

    SDL_RenderPresent(renderer);
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

    return 0;
};