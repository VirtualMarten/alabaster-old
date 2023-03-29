#ifndef __SHADER_H__
#define __SHADER_H__

#include "vertex.h"

#include <GL/glew.h>
#include <stdbool.h>
#include <stdint.h>

#ifndef POLYGON_VERTEX_FORMAT
#define POLYGON_VERTEX_FORMAT "position:2f uv:2f"
#endif
#ifndef MESH_VERTEX_FORMAT
#define MESH_VERTEX_FORMAT "position:3f uv:2f"
#endif

typedef struct {
    uint32_t program_id;
    uint32_t vao_id;
    vertex_buffer_t *polygon_vertex_buffer;
    vertex_buffer_t *mesh_vertex_buffer;
    int16_t uniform_projection;
    int16_t uniform_texrect;
    int16_t uniform_texmask;
    int16_t uniform_size;
    int16_t uniform_color;
    int16_t uniform_view;
    int16_t uniform_model;
} shader_t;

vertex_buffer_t* get_default_polygon_vertex_buffer();
vertex_buffer_t* get_default_mesh_vertex_buffer();

shader_t* new_shader();
shader_t* new_shader_ext(vertex_buffer_t *polygon_vertex_buffer, vertex_buffer_t *mesh_vertex_buffer);
void delete_shader(shader_t **shader);
bool update_shader(shader_t *shader, const char *source);
bool load_shader(shader_t *shader, const char *path);

#endif