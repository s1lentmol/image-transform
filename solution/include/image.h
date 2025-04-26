#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>
struct pixel {
    uint8_t b, g, r;
};

struct image {
    uint64_t width, height;
    struct pixel* data;
};
struct image* image_create(uint64_t width, uint64_t height);
void image_free(struct image* img);
struct image* image_copy(struct image const* src);
struct pixel image_get_pixel(struct image const* img, uint64_t x, uint64_t y);
void image_set_pixel(struct image* img, uint64_t x, uint64_t y, struct pixel p);

#endif
