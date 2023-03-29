#ifndef __POLYGON_H__
#define __POLYGON_H__

#include "vertex.h"
#include <stdint.h>

typedef struct {
    vertex_buffer_t *vertex_buffer;
    size_t vertex_buffer_offset;
    uint32_t vertex_count;
    float vertex_min_x, vertex_min_y;
    float vertex_max_x, vertex_max_y;
} polygon_t;

polygon_t* new_polygon(uint32_t vertex_count, float *vertices);
polygon_t* update_polygon(polygon_t *polygon, float *vertices);
void delete_polygon(polygon_t **polygon);

#endif