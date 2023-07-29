#include <stdio.h>
#include "display.h"

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
    free(colour_buffer);
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
    for (int y = 1; y < window_height; y++)
    {
        for (int x = 1; x < window_width; x++)
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