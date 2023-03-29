#include "ae/image.h"
#define STB_IMAGE_IMPLEMENTATION
#include "ae/libs/stb_image.h"

image_t* new_image(uint16_t width, uint16_t height, uint8_t components, uint8_t *data) {
    size_t size = width * height * components * sizeof(uint8_t);
    image_t *img = malloc(sizeof(image_t) + size);
    img->width = width;
    img->height = height;
    img->components = components;
    if (data && size) memcpy(img->data, data, size);
    else if (!data) memset(img->data, 0, size);
    return img;
}

image_t* resize_image(image_t **img, uint16_t width, uint16_t height) {
    if (!(img && *img)) return NULL;
    if (!width) width = (*img)->width;
    if (!height) height = (*img)->height;
    size_t old_size = (*img)->width * (*img)->height * (*img)->components * sizeof(uint8_t);
    uint8_t temp_data[old_size];
    memcpy(temp_data, (*img)->data, old_size);
    size_t new_size = width * height * (*img)->components * sizeof(uint8_t);
    *img = realloc(*img, sizeof(image_t) + new_size);
    uint32_t column_width = (*img)->width * (*img)->components * sizeof(uint8_t);
    for (uint32_t i = 0; i < old_size; i++) (*img)->data[i % column_width] = temp_data[i];
    (*img)->width = width;
    (*img)->height = height;
    return *img;
}

image_t* update_image(image_t **img, uint8_t components, uint8_t *data) {
    if (!(img && *img)) return NULL;
    if (!components) components = (*img)->components;
    size_t new_size = (*img)->width * (*img)->height * components * sizeof(uint8_t);
    *img = realloc(*img, sizeof(image_t) + new_size);
    memcpy((*img)->data, data, new_size);
    (*img)->components = components;
    return *img;
}

image_t* load_image(image_t **img, uint8_t components, const char *path) {
    if (!(img && *img)) return NULL;
    int width, height;
    int components_in_file;
    stbi_uc *data = stbi_load(path, &width, &height, &components_in_file, components);
    if (!data) return NULL;
    (*img)->width = width;
    (*img)->height = height;
    update_image(img, components ? components : components_in_file, data);
    stbi_image_free(data);
    return *img;
}