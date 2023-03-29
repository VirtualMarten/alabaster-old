#include "ae/polygon.h"
#include "ae/shader.h"
#include "ae/array.h"
#include <GL/glew.h>

static polygon_t **polygons = NULL;

polygon_t* update_polygon(polygon_t *polygon, float *vertices) {
    if (!(polygon && polygon->vertex_buffer && vertices)) return NULL;
    uint32_t m = polygon->vertex_count * 2;
    float *vertex_data = malloc(m * polygon->vertex_buffer->vertex_size);
    uint32_t i, j;
    float x, y;
    polygon->vertex_min_x = polygon->vertex_min_y = 0;
    polygon->vertex_max_x = polygon->vertex_max_y = 0;
    for (i = 0; i < m; i += 2) {
        x = vertices[i];
        y = vertices[i + 1];
        if (x > polygon->vertex_max_x) polygon->vertex_max_x = x;
        if (y > polygon->vertex_max_y) polygon->vertex_max_y = y;
        if (x < polygon->vertex_min_x) polygon->vertex_min_x = x;
        if (y < polygon->vertex_min_y) polygon->vertex_min_y = y;
    }
    for (i = j = 0; i < m; i += 2) {
        x = vertex_data[j++] = vertices[i];
        y = vertex_data[j++] = vertices[i + 1];
        vertex_data[j++] = (x - polygon->vertex_min_x) / polygon->vertex_max_x;
        vertex_data[j++] = (y - polygon->vertex_min_y) / polygon->vertex_max_y;
    }
    glBindBuffer(GL_ARRAY_BUFFER, polygon->vertex_buffer->buffer_id);
    glBufferSubData(
        GL_ARRAY_BUFFER,
        polygon->vertex_buffer_offset * polygon->vertex_buffer->vertex_size,
        polygon->vertex_count * polygon->vertex_buffer->vertex_size,
        vertex_data
    );
    free(vertex_data);
    return polygon;
}

polygon_t* new_polygon(uint32_t vertex_count, float *vertices) {
    if (!(vertex_count && vertices)) return NULL;
    vertex_buffer_t *vertex_buffer = get_default_polygon_vertex_buffer();
    polygon_t *polygon = calloc(1, sizeof(polygon_t));
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer->buffer_id);
    uint32_t i, offset = 0;
    if (!polygons) polygons = arr_new(sizeof(polygon_t), 1, 1);
    if (arr_len(polygons)) {
        uint32_t copy_buffer;
        glGenBuffers(1, &copy_buffer);
        glBindBuffer(GL_COPY_WRITE_BUFFER, copy_buffer);
        int32_t buffer_size;
        glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &buffer_size);
        glBufferData(GL_COPY_WRITE_BUFFER, buffer_size, NULL, GL_STATIC_COPY);
        for (i = 0; i < arr_len(polygons); i++) {
            if (!polygons[i]) continue;
            glCopyBufferSubData(
                GL_ARRAY_BUFFER, GL_COPY_WRITE_BUFFER,
                vertex_buffer->vertex_size * polygons[i]->vertex_buffer_offset,
                vertex_buffer->vertex_size * offset,
                vertex_buffer->vertex_size * polygons[i]->vertex_count
            );
            offset += polygons[i]->vertex_count;
        }
        buffer_size = (offset + vertex_count) * vertex_buffer->vertex_size;
        glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, GL_STATIC_DRAW);
        glCopyBufferSubData(GL_COPY_WRITE_BUFFER, GL_ARRAY_BUFFER, 0, 0, offset * vertex_buffer->vertex_size);
        glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
        glDeleteBuffers(1, &copy_buffer);
    }
    else glBufferData(GL_ARRAY_BUFFER, vertex_buffer->vertex_size * vertex_count, NULL, GL_STATIC_DRAW);
    polygon->vertex_buffer = vertex_buffer;
    polygon->vertex_count = vertex_count;
    polygon->vertex_buffer_offset = offset;
    arr_add(polygons, polygon);
    if (vertices) update_polygon(polygon, vertices);
    return polygon;
}

void delete_polygon(polygon_t **polygon) {
    for (uint32_t i = 0; i < arr_len(polygons); i++)
        if (polygons[i] == *polygon) arr_remove(polygons, i);
    free(*polygon);
    *polygon = NULL;
}