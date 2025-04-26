#include "image.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct image* image_create(uint64_t width, uint64_t height) {
    struct image* img = malloc(sizeof(struct image));
    if (!img) {
        return NULL;
    }
    img->width = width;
    img->height = height;
    img->data = malloc(sizeof(struct pixel) * width * height);
    if (!img->data) {
        free(img);
        return NULL;
    }
    memset(img->data, 0, sizeof(struct pixel) * width * height);
    return img;
}

// Освобождение памяти изображения
void image_free(struct image* img) {
    if (img) {
        free(img->data);
        free(img);
    }
}

struct image* image_copy(struct image const* src) {
    if (!src) return NULL;
    struct image* copy = image_create(src->width, src->height);
    if (!copy) return NULL;
    memcpy(copy->data, src->data, sizeof(struct pixel) * src->width * src->height);
    return copy;
}

// Получение пикселя по координатам
struct pixel image_get_pixel(struct image const* img, uint64_t x, uint64_t y) {
    struct pixel p = {0, 0, 0};
    if (!img || x >= img->width || y >= img->height) return p;
    p = img->data[y * img->width + x];
    return p;
}

// Установка пикселя по координатам
void image_set_pixel(struct image* img, uint64_t x, uint64_t y, struct pixel p) {
    if (!img || x >= img->width || y >= img->height) return;
    img->data[y * img->width + x] = p;
}
