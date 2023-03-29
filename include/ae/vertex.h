#ifndef __VERTEX_H__
#define __VERTEX_H__

#include <stdint.h>
#include <stdlib.h>

typedef struct {
    char *name;
    uint32_t type;
    uint32_t length, size;
} vertex_attribute_t;

typedef struct {
    uint32_t shader_program_id;
    uint32_t vao_id;
} vertex_shader_binding_t;

typedef struct {
    uint32_t buffer_id;
    uint32_t vertex_size;
    uint32_t attribute_count;
    vertex_shader_binding_t *shader_bindings;
    vertex_attribute_t attributes[];
} vertex_buffer_t;

vertex_buffer_t* new_vertex_buffer(const char *src);

static inline void delete_vertex(vertex_buffer_t **v) {
    free(*v);
    *v = NULL;
}

#endif