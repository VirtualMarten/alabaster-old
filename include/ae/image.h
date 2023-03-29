#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <stdint.h>
#include <stdlib.h>

typedef struct {
    uint16_t width, height;
    uint8_t components;
    uint8_t data[];
} image_t;

image_t* new_image(uint16_t width, uint16_t height, uint8_t components, uint8_t *data);
image_t* resize_image(image_t **img, uint16_t width, uint16_t height);
image_t* update_image(image_t **img, uint8_t components, uint8_t *data);
image_t* load_image(image_t **img, uint8_t components, const char *path);

static inline void delete_image(image_t **img) {
    free(*img);
    *img = NULL;
}

#endif