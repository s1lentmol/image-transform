#include "bmp.h"
#include "image.h"
#include "transform.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char* argv[]) {

    if (argc != 4) {
        return EXIT_FAILURE;
    }

    const char* source_filename = argv[1];
    const char* transformed_filename = argv[2];
    const char* transformation = argv[3];

    // Проверка типа преобразования
    if (strcmp(transformation, "none") != 0 &&
    strcmp(transformation, "cw90") != 0 &&
    strcmp(transformation, "ccw90") != 0 &&
    strcmp(transformation, "fliph") != 0 &&
    strcmp(transformation, "flipv") != 0) {
    fprintf(stderr, "Error: Неизвестное преобразование '%s'\n", transformation);
    return EXIT_FAILURE;
    }

    struct image* img = NULL;
    enum read_status status = from_bmp_filename(source_filename, &img);
    
    if (status == READ_MEMORY_ERROR){
        return ENOMEM;
    }

    else if (status != READ_OK) {
        fprintf(stderr, "Error: Не удалось прочитать файл '%s'", source_filename);
        return 2;
    }

    struct image* transformed_img = NULL;
    if (strcmp(transformation, "none") == 0) {
        transformed_img = image_copy(img);
        if (!transformed_img) {
            fprintf(stderr, "Error: Не удалось выделить память во время преобразования none");
            image_free(img);
            return ENOMEM;
        }
    }
    else if (strcmp(transformation, "cw90") == 0) {
        transformed_img = rotate_cw90(img);
        if (!transformed_img) {
            fprintf(stderr, "Error: Не удалось выделить память во время преобразования cw90");
            image_free(img);
            return ENOMEM;
        }
    }
    else if (strcmp(transformation, "ccw90") == 0) {
        transformed_img = rotate_ccw90(img);
        if (!transformed_img) {
            fprintf(stderr, "Error: Не удалось выделить память во время преобразования ccw90");
            image_free(img);
            return ENOMEM;
        }
    }
    else if (strcmp(transformation, "fliph") == 0) {
        transformed_img = flip_horizontal(img);
        if (!transformed_img) {
            fprintf(stderr, "Error: Не удалось выделить память во время преобразования fliph");
            image_free(img);
            return ENOMEM;
        }
    }
    else if (strcmp(transformation, "flipv") == 0) {
        transformed_img = flip_vertical(img);
        if (!transformed_img) {
            fprintf(stderr, "Error: Не удалось выделить память во время преобразования flipv");
            image_free(img);
            return ENOMEM;
        }
    }
    else {
        fprintf(stderr, "Error: Неизвестное преобразование '%s'\n", transformation);
        image_free(img);
        return EXIT_FAILURE;
    }

    enum write_status wstatus = to_bmp_filename(transformed_filename, transformed_img);
    if (wstatus != WRITE_OK) {
        fprintf(stderr, "Error: Не удалось записать файл BMP '%s'\n", transformed_filename);
        image_free(img);
        image_free(transformed_img);
        return EXIT_FAILURE;
    }

    // Освобождение памяти
    image_free(img);
    image_free(transformed_img);

    return EXIT_SUCCESS;
}

