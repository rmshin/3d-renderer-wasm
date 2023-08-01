#include <stdio.h>
#include <math.h>
#include "display.h"
#include "texture.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *colour_buffer_texture = NULL;
uint32_t *colour_buffer = NULL;
int window_width = 800;
int window_height = 600;

bool initialize_window(void)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        fprintf(stderr, "Error initializing\n");
        return false;
    };

    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);
    window_width = display_mode.w;
    window_height = display_mode.h;

    // Create SDL window
    window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, SDL_WINDOW_BORDERLESS | SDL_WINDOW_FULLSCREEN);
    if (!window)
    {
        fprintf(stderr, "Error creating SDL window\n");
        return false;
    };

    // Create SDL renderer
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer)
    {
        fprintf(stderr, "Error creating SDL renderer\n");
        return false;
    };

    return true;
};

void destroy_window(void)
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
};

void render_colour_buffer(void)
{
    SDL_UpdateTexture(colour_buffer_texture, NULL, colour_buffer, (int)(window_width * sizeof(uint32_t)));
    SDL_RenderCopy(renderer, colour_buffer_texture, NULL, NULL);
};

void clear_colour_buffer(uint32_t colour)
{
    for (int i = 0; i < window_width * window_height; i++)
    {
        colour_buffer[i] = colour;
    };
};

void draw_grid(uint32_t line_colour, int grid_spacing)
{
    for (int y = grid_spacing; y < window_height; y += grid_spacing)
    {
        for (int x = grid_spacing; x < window_width; x += grid_spacing)
        {
            if (x % grid_spacing == 0 || y % grid_spacing == 0)
            {
                colour_buffer[(y * window_width) + x] = line_colour;
            };
        };
    };
};

void draw_pixel(int x, int y, uint32_t colour)
{
    if (x >= 0 && x < window_width && y >= 0 && y < window_height)
    {
        colour_buffer[(y * window_width) + x] = colour;
    };
};

void draw_rect(int x, int y, int width, int height, uint32_t colour)
{
    for (int by = y; by < y + height; by++)
    {
        for (int bx = x; bx < x + width; bx++)
        {
            draw_pixel(bx, by, colour);
        };
    };
};

void draw_line(int x0, int y0, int x1, int y1, uint32_t colour)
{
    int delta_x = x1 - x0;
    int delta_y = y1 - y0;

    int side_length = abs(delta_x) > abs(delta_y) ? abs(delta_x) : abs(delta_y);

    float x_inc = delta_x / (float)side_length;
    float y_inc = delta_y / (float)side_length;

    float current_x = x0;
    float current_y = y0;

    for (int i = 0; i <= side_length; i++)
    {
        draw_pixel(round(current_x), round(current_y), colour);
        current_x += x_inc;
        current_y += y_inc;
    };
};

// Bresenham's Algo implementation, to explore further another time
// void draw_line(int x0, int y0, int x1, int y1, uint32_t color) {
//     int index = x0 + y0 * window_width;
//     if (x0 == x1 && y0 == y1) { // Draw single pixel:
//         DRAW_PIXEL(index, color);
//         return;
//     }

//     int dx = 1;
//     int dy = 1;
//     int run  = x1 - x0;
//     int rise = y1 - y0;
//     if (x0 > x1) {
//         dx = -1;
//         run  = x0 - x1;
//     }

//     int index_inc_per_line = window_width;
//     if (y0 > y1) {
//         dy = -1;
//         rise = y0 - y1;
//         index_inc_per_line = -window_width;
//     }

//     // Configure for a trivial line (horizontal, vertical or diagonal, default to a shallow line):
//     int inc = dx;
//     int start = x0;
//     int end = x1 + inc;
//     int index_inc = dx;
//     if (rise > run) { // Reconfigure for a steep line:
//         inc = dy;
//         start = y0;
//         end = y1 + inc;
//         index_inc = index_inc_per_line;
//     }

//     if (rise == run || !rise || !run) { // Draw a trivial line:
//         if (rise && run) // Reconfigure for a diagonal line:
//             index_inc = index_inc_per_line + dx;

//         for (int i = start; i != end; i += inc, index += index_inc)
//             DRAW_PIXEL(index, color);

//         return;
//     }

//     // Configure for a non-trivial line (default to a shallow line):
//     int rise_twice = rise + rise;
//     int run_twice  = run + run;
//     int threshold = run;
//     int error_dec = run_twice;
//     int error_inc = rise_twice;
//     int secondary_inc = index_inc_per_line;
//     if (rise > run) { // Reconfigure for a steep line:
//         secondary_inc = dx;
//         threshold = rise;
//         error_dec = rise_twice;
//         error_inc = run_twice;
//     }

//     int error = 0;
//     for (int i = start; i != end; i += inc) {
//         DRAW_PIXEL(index, color);
//         index += index_inc;
//         error += error_inc;
//         if (error > threshold) {
//             error -= error_dec;
//             index += secondary_inc;
//         }
//     }
// }