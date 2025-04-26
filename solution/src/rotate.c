#include "transform.h"
#include <stdlib.h>

struct image* rotate_cw90(struct image const* src) {
    if (!src) return NULL;
    struct image* rotated = image_create(src->height, src->width);
    if (!rotated) return NULL;

    for (uint64_t y = 0; y < src->height; y++) {
        for (uint64_t x = 0; x < src->width; x++) {
            struct pixel p = image_get_pixel(src, x, y);
            image_set_pixel(rotated, src->height - 1 - y, x, p);
        }
    }
    return rotated;
}

struct image* rotate_ccw90(struct image const* src) {
    if (!src) return NULL;
    struct image* rotated = image_create(src->height, src->width);
    if (!rotated) return NULL;

    for (uint64_t y = 0; y < src->height; y++) {
        for (uint64_t x = 0; x < src->width; x++) {
            struct pixel p = image_get_pixel(src, x, y);
            image_set_pixel(rotated, y, src->width - 1 - x, p);
        }
    }
    return rotated;
}
