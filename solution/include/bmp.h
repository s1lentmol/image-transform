#ifndef BMP_H
#define BMP_H

#include "image.h"
#include <stdio.h>

#pragma pack(push, 1)
struct bmp_header {
    uint16_t bfType;  
    uint32_t bfileSize;
    uint32_t bfReserved;
    uint32_t bOffBits;
    uint32_t biSize;
    uint32_t  biWidth;
    uint32_t  biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    uint32_t  biXPelsPerMeter;
    uint32_t  biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
};
#pragma pack(pop)

enum read_status {
    READ_OK = 0,
    READ_INVALID_SIGNATURE,
    READ_INVALID_BITS,
    READ_INVALID_HEADER,
    READ_MEMORY_ERROR,
    READ_IO_ERROR
};

enum write_status {
    WRITE_OK = 0,
    WRITE_ERROR,
    WRITE_IO_ERROR
};

// Функции для чтения и записи BMP из/в FILE*
enum read_status from_bmp(FILE* in, struct image** img);
enum write_status to_bmp(FILE* out, struct image const* img);

// Открытие и закрытие файлов, вызов функции для чтения и записи в файл
enum read_status from_bmp_filename(const char* filename, struct image** img);
enum write_status to_bmp_filename(const char* filename, struct image const* img);

#endif
