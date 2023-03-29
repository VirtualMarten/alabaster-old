#include "ae/vertex.h"
#include "ae/array.h"
#include <ctype.h>
#include <string.h>
#include <GL/glew.h>

static char parse_attribute(const char *layout, uint32_t *i, vertex_attribute_t *attrib) {
    char buf[128];
    char *buf_i = buf;
    for (; layout[*i] != ':'; (*i)++) {
        if (isalpha(layout[*i]))
            *buf_i++ = layout[*i];
        else return 0;
    }
    *buf_i = '\0';
    attrib->name = malloc((uint32_t)(buf_i - buf) + 1);
    strncpy(attrib->name, buf, (uint32_t)(buf_i - buf) + 1);
    for (++(*i), buf_i = buf; isdigit(layout[*i]); (*i)++)
        *buf_i++ = layout[*i];
    *buf_i = '\0';
    int len = atoi(buf);
    if (len < 0) len *= -1;
    attrib->length = len;
    switch (layout[*i]) {
        case 'b': case 'B':
            attrib->type = GL_BYTE;
            attrib->size = attrib->length * sizeof(char);
            break;
        case 'i': case 'I':
            attrib->type = GL_INT;
            attrib->size = attrib->length * sizeof(int);
            break;
        case 'u': case 'U':
            if (tolower(layout[*i + 1]) == 'b') {
                attrib->type = GL_UNSIGNED_BYTE;
                attrib->size = attrib->length * sizeof(uint8_t);
                break;
            }
            attrib->type = GL_UNSIGNED_INT;
            attrib->size = attrib->length * sizeof(uint32_t);
            break;
        case 'f': case 'F':
            attrib->type = GL_FLOAT;
            attrib->size = attrib->length * sizeof(float);
            break;
        case 'd': case 'D':
            attrib->type = GL_DOUBLE;
            attrib->size = attrib->length * sizeof(double);
            break;
        default: return 0;
    }
    return 1;
}

vertex_buffer_t* new_vertex_buffer(const char *src) {
    vertex_attribute_t *attributes = arr_new(sizeof(vertex_attribute_t), 2, 2);
    uint32_t i, l = strlen(src);
    for (i  = 0; i < l; i++) {
        while (isspace(src[i])) ++i;
        vertex_attribute_t a;
        if (!parse_attribute(src, &i, &a))
            return NULL;
        arr_add(attributes, a);
    }
    vertex_buffer_t *vertex_buffer = calloc(1, sizeof(vertex_buffer_t) + sizeof(vertex_attribute_t) * arr_len(attributes));
    vertex_buffer->attribute_count = arr_len(attributes);
    memcpy(vertex_buffer->attributes, attributes, sizeof(vertex_attribute_t) * arr_len(attributes));
    arr_delete(attributes);
    for (i = 0; i < vertex_buffer->attribute_count; i++)
        vertex_buffer->vertex_size += vertex_buffer->attributes[i].size;
    glGenBuffers(1, (GLuint*)&vertex_buffer->buffer_id);
    vertex_buffer->shader_bindings = arr_new(sizeof(vertex_shader_binding_t), 1, 1);
    return vertex_buffer;
}