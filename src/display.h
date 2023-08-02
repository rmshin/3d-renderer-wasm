#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define FPS 30
#define TARGET_FRAME_TIME (1000 / FPS)

extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern SDL_Texture *colour_buffer_texture;
extern uint32_t *colour_buffer;
extern float *w_buffer;
extern int window_width;
extern int window_height;

bool initialize_window(void);
void destroy_window(void);
void render_colour_buffer(void);
void clear_colour_buffer(uint32_t colour);
void clear_w_buffer(void);
void draw_grid(uint32_t line_colour, int grid_spacing);
void draw_pixel(int x, int y, float inverse_w, uint32_t colour);
void draw_rect(int x, int y, int width, int height, uint32_t colour);
void draw_line(int x0, int y0, int x1, int y1, uint32_t colour);

enum Display_Mode
{
    DISPLAY_WIRE,
    DISPLAY_WIRE_VERTEX,
    DISPLAY_FILL,
    DISPLAY_FILL_WIRE,
    DISPLAY_TEXTURE,
    DISPLAY_TEXTURE_WIRE,
} display_mode;

enum Cull_Method
{
    CULL_NONE,
    CULL_BACKFACE,
} cull_method;

#endif