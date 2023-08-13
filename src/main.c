#include <assert.h>
#include <math.h>
#include "util.h"
#include "array.h"
#include "display.h"
#include "vector.h"
#include "matrix.h"
#include "mesh.h"
#include "triangle.h"
#include "lighting.h"
#include "texture.h"
#include "camera.h"
#include "clipping.h"
#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#include <emscripten/html5.h>
#endif

triangle_t *triangles_to_render = NULL;

mat4_t projection_matrix;
mat4_t world_matrix;
mat4_t view_matrix;

float delta_time;
uint32_t prev_frame_time;
bool is_running;
bool left_mouse_down;

void setup(void)
{
    set_display_mode(DISPLAY_WIRE);
    set_cull_method(CULL_BACKFACE);
    set_rotation_mode(ROTATE_NONE);

    init_light((vec3_t){0, 0, 1}); // direction
    init_camera(
        (vec3_t){0, 0, 0}, // position
        (vec3_t){0, 0, 1}  // direction
    );

    float aspect_x = (float)get_window_width() / (float)get_window_height();
    float aspect_y = (float)get_window_height() / (float)get_window_width();
    float fov_y = M_PI / 2.0;
    float fov_x = 2 * atan(tan(fov_y / 2.0) * aspect_x);
    float znear = 1.0;
    float zfar = 1000.0;
    projection_matrix = mat4_make_projection(fov_y, aspect_y, znear, zfar);

    init_frustum_planes(fov_x, fov_y, znear, zfar);

#ifdef __EMSCRIPTEN__
    load_single_mesh("sphere");
#endif
#ifndef __EMSCRIPTEN__
    load_mesh("./assets/efa.obj", "./assets/efa.png", (vec3_t){1, 1, 1}, (vec3_t){0, 0, 0}, (vec3_t){-3, 0, 3});
    load_mesh("./assets/f22.obj", "./assets/f22.png", (vec3_t){1, 1, 1}, (vec3_t){0, 0, 0}, (vec3_t){3, 0, 7});
    // load_mesh("./assets/f117.obj", "./assets/f117.png", (vec3_t){1, 1, 1}, (vec3_t){0, 0, 0}, (vec3_t){0, -5, 5});
    load_mesh("./assets/cube.obj", "./assets/cube.png", (vec3_t){1, 1, 1}, (vec3_t){0, 0, 0}, (vec3_t){0, -5, 5});
#endif
};

void process_input(void)
{
    SDL_Event event;

    // for smoother camera movement
    uint8_t *keystate = SDL_GetKeyboardState(NULL);
    if (keystate[SDL_SCANCODE_W])
    {
        set_camera_forward_velocity(vec3_mul(get_camera_direction(), 5.0 * delta_time));
        set_camera_position(vec3_add(get_camera_position(), get_camera_forward_velocity()));
    }
    else if (keystate[SDL_SCANCODE_S])
    {
        set_camera_forward_velocity(vec3_mul(get_camera_direction(), 5.0 * delta_time));
        set_camera_position(vec3_subtract(get_camera_position(), get_camera_forward_velocity()));
    }
    else if (keystate[SDL_SCANCODE_A])
    {
        vec3_t up = {0, 1, 0};
        vec3_t left = vec3_cross(get_camera_direction(), up);
        vec3_normalise(&left);
        set_camera_forward_velocity(vec3_mul(left, 5.0 * delta_time));
        set_camera_position(vec3_add(get_camera_position(), get_camera_forward_velocity()));
    }
    else if (keystate[SDL_SCANCODE_D])
    {
        vec3_t down = {0, -1, 0};
        vec3_t right = vec3_cross(get_camera_direction(), down);
        vec3_normalise(&right);
        set_camera_forward_velocity(vec3_mul(right, 5.0 * delta_time));
        set_camera_position(vec3_add(get_camera_position(), get_camera_forward_velocity()));
    }

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            is_running = false;
            break;
        case SDL_KEYDOWN:
#ifndef __EMSCRIPTEN__
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                is_running = false;
                break;
            }
#endif
            if (event.key.keysym.sym == SDLK_1)
            {
                set_display_mode(DISPLAY_VERTEX);
            }
            else if (event.key.keysym.sym == SDLK_2)
            {
                set_display_mode(DISPLAY_WIRE);
            }
            else if (event.key.keysym.sym == SDLK_3)
            {
                set_display_mode(DISPLAY_FILL_WIRE);
            }
            else if (event.key.keysym.sym == SDLK_4)
            {
                set_display_mode(DISPLAY_FILL);
            }
            else if (event.key.keysym.sym == SDLK_5)
            {
                set_display_mode(DISPLAY_TEXTURE);
            }
            else if (event.key.keysym.sym == SDLK_c)
            {
                get_cull_method() == CULL_BACKFACE ? set_cull_method(CULL_NONE) : set_cull_method(CULL_BACKFACE);
            }
            else if (event.key.keysym.sym == SDLK_r)
            {
                get_rotation_mode() == ROTATE_NONE ? set_rotation_mode(ROTATE_AUTO) : set_rotation_mode(ROTATE_NONE);
            }
            else if (event.key.keysym.sym == SDLK_q)
            {
                reset_camera();
            }
            break;
        case SDL_MOUSEMOTION:
            if (left_mouse_down)
            {
                rotate_camera_yaw(event.motion.xrel * delta_time * MOUSE_SENSITIVITY);
                rotate_camera_pitch(event.motion.yrel * delta_time * MOUSE_SENSITIVITY);
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if (left_mouse_down && event.button.button == SDL_BUTTON_LEFT)
            {
                left_mouse_down = false;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (!left_mouse_down && event.button.button == SDL_BUTTON_LEFT)
            {
                left_mouse_down = true;
            }
            break;
        case SDL_MULTIGESTURE:
            if (event.mgesture.numFingers == 2)
            {
                if (event.mgesture.dDist > 0)
                {
                    // Zoom in (pinch)
                    set_camera_forward_velocity(vec3_mul(get_camera_direction(), 5.0 * delta_time));
                    set_camera_position(vec3_add(get_camera_position(), get_camera_forward_velocity()));
                }
                else if (event.mgesture.dDist < 0)
                {
                    // Zoom out (spread)
                    set_camera_forward_velocity(vec3_mul(get_camera_direction(), 5.0 * delta_time));
                    set_camera_position(vec3_subtract(get_camera_position(), get_camera_forward_velocity()));
                }
            }
        }
    }
};

void process_graphics_pipeline_stages(mesh_t *mesh)
{
    // apply auto rotation if enabled
    if (get_rotation_mode() == ROTATE_AUTO)
    {
        mesh->rotation.x += 0.5 * delta_time;
        mesh->rotation.y += 0.5 * delta_time;
    }

    // create view matrix based off camera look at target
    vec3_t up_direction = {0, 1, 0};
    vec3_t target = get_camera_lookat_target();
    view_matrix = mat4_make_view(target, get_camera_position(), up_direction);

    mat4_t scale_matrix = mat4_make_scale(mesh->scale.x, mesh->scale.y, mesh->scale.z);
    mat4_t rotx_matrix = mat4_make_rotation_x(mesh->rotation.x);
    mat4_t roty_matrix = mat4_make_rotation_y(mesh->rotation.y);
    mat4_t rotz_matrix = mat4_make_rotation_z(mesh->rotation.z);
    mat4_t translate_matrix = mat4_make_translation(mesh->translation.x, mesh->translation.y, mesh->translation.z);

    // calculate combined world transformation matrix
    world_matrix = scale_matrix;
    world_matrix = mat4_mul_mat4(rotx_matrix, world_matrix);
    world_matrix = mat4_mul_mat4(roty_matrix, world_matrix);
    world_matrix = mat4_mul_mat4(rotz_matrix, world_matrix);
    world_matrix = mat4_mul_mat4(translate_matrix, world_matrix);

    for (int i = 0; i < array_length(mesh->faces); i++)
    {
        face_t mesh_face = mesh->faces[i];
        vec3_t face_vertices[3] = {
            mesh->vertices[mesh_face.a],
            mesh->vertices[mesh_face.b],
            mesh->vertices[mesh_face.c],
        };

        vec4_t transformed_vertices[3];
        for (int j = 0; j < 3; j++)
        {
            vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

            // world space transformation
            transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);

            // camera space transformation
            transformed_vertex = mat4_mul_vec4(view_matrix, transformed_vertex);

            transformed_vertices[j] = transformed_vertex;
        }

        if (!is_cull_backface() || !cull_face(transformed_vertices))
        {
            // perform clipping
            polygon_t poly = create_poly_from_triangle(
                vec3_from_vec4(transformed_vertices[0]),
                vec3_from_vec4(transformed_vertices[1]),
                vec3_from_vec4(transformed_vertices[2]),
                mesh_face.a_uv,
                mesh_face.b_uv,
                mesh_face.c_uv);

            clip_polygon(&poly);

            triangle_t triangles_after_clipping[MAX_POLY_TRIANGLES];
            int num_triangles_after_clipping = poly.num_vertices - 2;
            create_triangles_from_poly(&poly, triangles_after_clipping);

            for (int t = 0; t < num_triangles_after_clipping; t++)
            {
                triangle_t triangle_to_render = triangles_after_clipping[t];
                for (int j = 0; j < 3; j++)
                {
                    // project to screen space
                    vec4_t projected = mat4_mul_vec4_project(projection_matrix, triangle_to_render.points[j]);
                    // translate to center & scale
                    projected.x *= get_window_width() / 2.0;
                    projected.y *= get_window_height() / 2.0;
                    projected.x += get_window_width() / 2.0;
                    projected.y += get_window_height() / 2.0;
                    triangle_to_render.points[j] = (vec4_t){projected.x, projected.y, projected.z, projected.w};
                }

                float shading_factor = calc_shading_factor(transformed_vertices, get_light_direction());
                triangle_to_render.colour = light_apply_intensity(mesh_face.colour, shading_factor);
                triangle_to_render.shading_factor = shading_factor;
                triangle_to_render.texture = mesh->texture;

                array_push(triangles_to_render, triangle_to_render)
            }
        }
    }
};

void update(void)
{
    delta_time = (SDL_GetTicks() - prev_frame_time) / 1000.0;
    prev_frame_time = SDL_GetTicks();

    array_reset(triangles_to_render);

    int mesh_count = get_mesh_count();

    for (int m = 0; m < mesh_count; m++)
    {
        mesh_t *mesh = get_mesh(m);

        process_graphics_pipeline_stages(mesh);
    }
};

void draw_triangles(void)
{
    for (int i = 0; i < array_length(triangles_to_render); i++)
    {
        triangle_t triangle = triangles_to_render[i];

        if (should_render_filled_triangle())
        {
            draw_filled_triangle(
                triangle.points[0].x, triangle.points[0].y, triangle.points[0].w,
                triangle.points[1].x, triangle.points[1].y, triangle.points[1].w,
                triangle.points[2].x, triangle.points[2].y, triangle.points[2].w,
                triangle.colour);
        }
        if (should_render_texture())
        {
            // only render textured if exists, otherwise render fill
            if (triangle.texture != NULL)
            {
                draw_textured_triangle(
                    triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, triangle.tex_coords[0].u, triangle.tex_coords[0].v,
                    triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, triangle.tex_coords[1].u, triangle.tex_coords[1].v,
                    triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, triangle.tex_coords[2].u, triangle.tex_coords[2].v,
                    triangle.texture, triangle.shading_factor);
            }
            else
            {
                draw_filled_triangle(
                    triangle.points[0].x, triangle.points[0].y, triangle.points[0].w,
                    triangle.points[1].x, triangle.points[1].y, triangle.points[1].w,
                    triangle.points[2].x, triangle.points[2].y, triangle.points[2].w,
                    triangle.colour);
            }
        }
        if (should_render_wireframe())
        {
            draw_triangle(triangle, 0xFFC3C3C3);
        }
        if (should_render_vertices())
        {
            draw_rect(triangle.points[0].x, triangle.points[0].y, 3, 3, 0xFF0000FF);
            draw_rect(triangle.points[1].x, triangle.points[1].y, 3, 3, 0xFF0000FF);
            draw_rect(triangle.points[2].x, triangle.points[2].y, 3, 3, 0xFF0000FF);
        }
    };
};

void render(void)
{
    clear_colour_buffer(0xFF000000);
    clear_w_buffer();

    draw_grid(0xFF3C3C3C, 20);
    draw_triangles();

    render_colour_buffer();
};

void free_resources(void)
{
    array_free(triangles_to_render);
    free_mesh_resources();
};

void mainLoop(void)
{
    process_input();
    update();
    render();

#ifdef __EMSCRIPTEN__
    if (!is_running)
    {
        emscripten_cancel_main_loop();
        destroy_window();
        free_resources();
    }
#endif
};

int main(void)
{
    prev_frame_time = 0;
    is_running = false;
    left_mouse_down = false;

#ifdef __EMSCRIPTEN__
    // webgl context
    EmscriptenWebGLContextAttributes gl_ctx;
    emscripten_webgl_init_context_attributes(&gl_ctx);
    int ctx_handle = emscripten_webgl_create_context("#canvas", &gl_ctx);
    emscripten_webgl_make_context_current(ctx_handle);
#endif

    // create an SDL window
    is_running = initialize_window();

    setup();
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(mainLoop, -1, 1);
#endif
#ifndef __EMSCRIPTEN__
    while (is_running)
    {
        mainLoop();
    }
    destroy_window();
    free_resources();
#endif

    return 0;
};