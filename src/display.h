#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

typedef enum
{
    DISPLAY_WIRE,
    DISPLAY_WIRE_VERTEX,
    DISPLAY_FILL,
    DISPLAY_FILL_WIRE,
    DISPLAY_TEXTURE,
    DISPLAY_TEXTURE_WIRE,
} DisplayMode_t;

typedef enum
{
    CULL_NONE,
    CULL_BACKFACE,
} CullMethod_t;

// window
bool initialize_window(void);
void destroy_window(void);
int get_window_height(void);
int get_window_width(void);

// display mode
DisplayMode_t get_display_mode(void);
void set_display_mode(DisplayMode_t mode);
bool should_render_filled_triangle(void);
bool should_render_texture(void);
bool should_render_wireframe(void);
bool should_render_vertices(void);

// cull method
CullMethod_t get_cull_method(void);
void set_cull_method(CullMethod_t method);
bool is_cull_backface(void);

// colour buffer
void render_colour_buffer(void);
void clear_colour_buffer(uint32_t colour);

float get_w_buffer_at(int x, int y);
void update_w_buffer_at(int x, int y, float val);
void clear_w_buffer(void);
void draw_grid(uint32_t line_colour, int grid_spacing);
void draw_pixel(int x, int y, uint32_t colour);
void draw_rect(int x, int y, int width, int height, uint32_t colour);
void draw_line(int x0, int y0, int x1, int y1, uint32_t colour);

#endif