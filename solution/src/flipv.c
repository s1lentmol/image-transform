#include "transform.h"
#include <stdlib.h>

struct image* flip_vertical(struct image const* src) {
    if (!src) return NULL;
    struct image* flipped = image_copy(src);
    if (!flipped) return NULL;

    for (uint64_t y = 0; y < flipped->height / 2; y++) {
        uint64_t opposite_y = flipped->height - 1 - y;
        for (uint64_t x = 0; x < flipped->width; x++) {
            struct pixel p1 = image_get_pixel(flipped, x, y);
            struct pixel p2 = image_get_pixel(flipped, x, opposite_y);
            image_set_pixel(flipped, x, y, p2);
            image_set_pixel(flipped, x, opposite_y, p1);
        }
    }
    return flipped;
}
