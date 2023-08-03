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

triangle_t *triangles_to_render = NULL;

mat4_t projection_matrix;
mat4_t world_matrix;
mat4_t view_matrix;

float delta_time;
uint32_t prev_frame_time = 0;
bool is_running = false;

void setup(void)
{
    set_display_mode(DISPLAY_WIRE);
    set_cull_method(CULL_BACKFACE);

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

    load_mesh("./assets/efa.obj", "./assets/efa.png", (vec3_t){1, 1, 1}, (vec3_t){0, 0, 0}, (vec3_t){-3, 0, 2});
    load_mesh("./assets/crab.obj", "./assets/crab.png", (vec3_t){1, 1, 1}, (vec3_t){0, 0, 0}, (vec3_t){3, 0, 2});
};

void process_input(void)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
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
                set_display_mode(DISPLAY_WIRE_VERTEX);
            }
            else if (event.key.keysym.sym == SDLK_2)
            {
                set_display_mode(DISPLAY_WIRE);
            }
            else if (event.key.keysym.sym == SDLK_3)
            {
                set_display_mode(DISPLAY_FILL);
            }
            else if (event.key.keysym.sym == SDLK_4)
            {
                set_display_mode(DISPLAY_FILL_WIRE);
            }
            else if (event.key.keysym.sym == SDLK_5)
            {
                set_display_mode(DISPLAY_TEXTURE);
            }
            else if (event.key.keysym.sym == SDLK_6)
            {
                set_display_mode(DISPLAY_TEXTURE_WIRE);
            }
            else if (event.key.keysym.sym == SDLK_c)
            {
                get_cull_method() == CULL_BACKFACE ? set_cull_method(CULL_NONE) : set_cull_method(CULL_BACKFACE);
            }
            else if (event.key.keysym.sym == SDLK_w)
            {
                set_camera_forward_velocity(vec3_mul(get_camera_direction(), 10.0 * delta_time));
                set_camera_position(vec3_add(get_camera_position(), get_camera_forward_velocity()));
            }
            else if (event.key.keysym.sym == SDLK_s)
            {
                set_camera_forward_velocity(vec3_mul(get_camera_direction(), 10.0 * delta_time));
                set_camera_position(vec3_subtract(get_camera_position(), get_camera_forward_velocity()));
            }
            else if (event.key.keysym.sym == SDLK_a)
            {
                rotate_camera_yaw(-3.0 * delta_time);
            }
            else if (event.key.keysym.sym == SDLK_d)
            {
                rotate_camera_yaw(3.0 * delta_time);
            }
            else if (event.key.keysym.sym == SDLK_UP)
            {
                rotate_camera_pitch(-6.0 * delta_time);
            }
            else if (event.key.keysym.sym == SDLK_DOWN)
            {
                rotate_camera_pitch(6.0 * delta_time);
            }
            else if (event.key.keysym.sym == SDLK_p)
            {
                vec3_t camera_position = get_camera_position();
                vec3_t camera_direction = get_camera_direction();
                printf("camera position x: %f -- y: %f -- z: %f\n", camera_position.x, camera_position.y, camera_position.z);
                printf("camera direction x: %f -- y: %f -- z: %f\n", camera_direction.x, camera_direction.y, camera_direction.z);
                printf("pitch value: %f\n", get_camera_pitch());
            }
            else if (event.key.keysym.sym == SDLK_RIGHT)
            {
            }
            break;
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

        // mesh.rotation.y += 0.6 * delta_time;
        // mesh.rotation.x += 0.6 * delta_time;
        // mesh.rotation.z += 0.6 * delta_time;
        // mesh.scale.x += 0.002;
        // mesh.scale.y += 0.002;
        // mesh.scale.z += 0.002;
        // mesh.translation.x += 0.05;
        // mesh.translation.y += 0.05;
        mesh->translation.z = 5.0;

        // more interesting transformations
        // t = SDL_GetTicks() * 0.0005;

        // mesh.rotation.x += 0.02;

        // mesh.scale.x = sin(t) + 1;
        // mesh.scale.y = sin(t) + 1;
        // mesh.scale.z = sin(t) + 1;

        // mesh.translation.x = tan(t) * 2;
        // mesh.translation.y = sin(t) * 2.2;

        // camera movement
        // camera.position.x += 0.1 * delta_time;
        // camera.position.y += 0.3 * delta_time;
        // camera.position.z = 5.0;

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

                    triangle_to_render.colour = 0xFFFFFFF;

                    float shading_factor = calc_shading_factor(transformed_vertices, get_light_direction());
                    triangle_to_render.colour = light_apply_intensity(mesh_face.colour, shading_factor);
                    triangle_to_render.texture = mesh->texture;

                    array_push(triangles_to_render, triangle_to_render)
                }
            }
        }
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
            draw_textured_triangle(
                triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, triangle.tex_coords[0].u, triangle.tex_coords[0].v,
                triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, triangle.tex_coords[1].u, triangle.tex_coords[1].v,
                triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, triangle.tex_coords[2].u, triangle.tex_coords[2].v,
                triangle.texture);
        }
        if (should_render_wireframe())
        {
            draw_triangle(triangle, 0xFFFFFFFF);
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
    }

    destroy_window();
    free_resources();

    return 0;
};