#include "transform.h"
#include <stdlib.h>

struct image* flip_horizontal(struct image const* src) {
    if (!src) return NULL;
    struct image* flipped = image_copy(src);
    if (!flipped) return NULL;

    for (uint64_t y = 0; y < flipped->height; y++) {
        for (uint64_t x = 0; x < flipped->width / 2; x++) {
            uint64_t opposite_x = flipped->width - 1 - x;
            struct pixel p1 = image_get_pixel(flipped, x, y);
            struct pixel p2 = image_get_pixel(flipped, opposite_x, y);
            image_set_pixel(flipped, x, y, p2);
            image_set_pixel(flipped, opposite_x, y, p1);
        }
    }
    return flipped;
}
