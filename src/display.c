#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "display.h"
#include "texture.h"
#ifdef __EMSCRIPTEN__
#include <emscripten/html5.h>
#endif

static DisplayMode_t display_mode = DISPLAY_WIRE;
static CullMethod_t cull_method = CULL_BACKFACE;
static RotationMode_t rotation_mode = ROTATE_NONE;
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture *colour_buffer_texture = NULL;
static uint32_t *colour_buffer = NULL;
static float *w_buffer = NULL;
static int window_width = 640;  // simulates lower resolution display
static int window_height = 360; // simulates lower resolution display

int get_window_height(void)
{
    return window_height;
};
int get_window_width(void)
{
    return window_width;
};

DisplayMode_t get_display_mode(void)
{
    return display_mode;
};
void set_display_mode(DisplayMode_t mode)
{
    display_mode = mode;
};
bool should_render_filled_triangle(void)
{
    return (
        display_mode == DISPLAY_FILL_WIRE ||
        display_mode == DISPLAY_FILL);
};
bool should_render_texture(void)
{
    return (display_mode == DISPLAY_TEXTURE);
};
bool should_render_wireframe(void)
{
    return (
        display_mode == DISPLAY_WIRE ||
        display_mode == DISPLAY_FILL_WIRE);
};
bool should_render_vertices(void)
{
    return (display_mode == DISPLAY_VERTEX);
};

CullMethod_t get_cull_method(void)
{
    return cull_method;
};
void set_cull_method(CullMethod_t method)
{
    cull_method = method;
};
bool is_cull_backface(void)
{
    return cull_method == CULL_BACKFACE;
};

RotationMode_t get_rotation_mode(void)
{
    return rotation_mode;
};
void set_rotation_mode(RotationMode_t mode)
{
    rotation_mode = mode;
};

bool initialize_window(void)
{
    SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");
    SDL_SetHint(SDL_HINT_FRAMEBUFFER_ACCELERATION, "1");
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        fprintf(stderr, "Error initializing\n");
        return false;
    };

    SDL_DisplayMode sdl_display_mode;
    SDL_GetCurrentDisplayMode(0, &sdl_display_mode);
    int fullscreen_window_width = sdl_display_mode.w;
    int fullscreen_window_height = sdl_display_mode.h;

    window_width = fullscreen_window_width;
    window_height = fullscreen_window_height;

#ifdef __EMSCRIPTEN__
    double pixel_ratio = emscripten_get_device_pixel_ratio();
    window_width = window_width * pixel_ratio;
    window_height = window_height * pixel_ratio;

    // scale to proper resolution for high dpi displays
    EmscriptenFullscreenStrategy strategy;
    strategy.scaleMode = EMSCRIPTEN_FULLSCREEN_CANVAS_SCALE_HIDEF;
    strategy.canvasResolutionScaleMode = EMSCRIPTEN_FULLSCREEN_CANVAS_SCALE_HIDEF;
    strategy.filteringMode = EMSCRIPTEN_FULLSCREEN_FILTERING_DEFAULT;
    strategy.canvasResizedCallbackUserData = NULL;
    emscripten_enter_soft_fullscreen("canvas", &strategy);
    emscripten_set_element_css_size("canvas", (double)window_width, (double)window_height);

#endif

    // Create SDL window
    window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, SDL_WINDOW_BORDERLESS);
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

    colour_buffer = (uint32_t *)malloc(sizeof(uint32_t) * window_width * window_height);
    assert(colour_buffer);
    colour_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, window_width, window_height);
    assert(colour_buffer_texture);
    w_buffer = (float *)malloc(sizeof(float) * window_width * window_height);
    assert(w_buffer);

    return true;
};

void destroy_window(void)
{
    free(colour_buffer);
    free(w_buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
};

void render_colour_buffer(void)
{
    SDL_UpdateTexture(colour_buffer_texture, NULL, colour_buffer, (int)(window_width * sizeof(uint32_t)));
    SDL_RenderCopy(renderer, colour_buffer_texture, NULL, NULL); // automatically handles buffer stretching/shrinking to match display resolution
    SDL_RenderPresent(renderer);
};
void clear_colour_buffer(uint32_t colour)
{
    for (int i = 0; i < window_width * window_height; i++)
    {
        colour_buffer[i] = colour;
    };
};

float get_w_buffer_at(int x, int y)
{
    if (x < 0 || x >= window_width || y < 0 || y >= window_height)
    {
        return 0.0;
    }
    return w_buffer[(y * window_width) + x];
};
void update_w_buffer_at(int x, int y, float val)
{
    if (x < 0 || x >= window_width || y < 0 || y >= window_height)
    {
        return;
    }
    w_buffer[(y * window_width) + x] = val;
};
void clear_w_buffer(void)
{
    for (int i = 0; i < window_width * window_height; i++)
    {
        w_buffer[i] = 0.0;
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
    if (x < 0 || x >= window_width || y < 0 || y >= window_height)
    {
        return;
    }
    colour_buffer[(y * window_width) + x] = colour;
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