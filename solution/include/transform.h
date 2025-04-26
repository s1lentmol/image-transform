#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "image.h"

struct image* rotate_cw90(struct image const* src);

struct image* rotate_ccw90(struct image const* src);

struct image* flip_horizontal(struct image const* src);

struct image* flip_vertical(struct image const* src);

#endif
