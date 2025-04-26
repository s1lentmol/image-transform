#include "bmp.h"
#include "image.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PIXELS 100000000
#define BMP_FILE_TYPE 0x4D42

// Чтение заголовка BMP
enum read_status read_bmp_header(FILE* in, struct bmp_header* header) {
    if (!in || !header) return READ_INVALID_HEADER;

    size_t read_bytes = fread(header, sizeof(struct bmp_header), 1, in);
    if (read_bytes != 1) {
        if (feof(in)) {
            fprintf(stderr, "Error: BMP файл слишком короткий");
            return READ_IO_ERROR;
        }
        fprintf(stderr, "Error: Ошибка чтения BMP заголовка");
        return READ_IO_ERROR;
    }
    return READ_OK;
}

// Валидация заголовка BMP
enum read_status validate_bmp_header(const struct bmp_header* header) {
    if (header->bfType != BMP_FILE_TYPE) { 
        fprintf(stderr, "Error: Неверная сигнатура BMP");
        return READ_INVALID_SIGNATURE;
    }

    if (header->biBitCount != 24) {
        fprintf(stderr, "Error: Неподдерживаемое количество бит BMP (%d)", header->biBitCount);
        return READ_INVALID_BITS;
    }

    if (header->biCompression != 0) {
        fprintf(stderr, "Error: Сжатие BMP не поддерживается");
        return READ_INVALID_HEADER;
    }

    if (header->biWidth <= 0 || header->biHeight <= 0) {
        fprintf(stderr, "Error: Неверные размеры BMP");
        return READ_INVALID_HEADER;
    }

    if (header->bOffBits < sizeof(struct bmp_header)) {
        fprintf(stderr, "Error: Неверный размер заголовка BMP");
        return READ_INVALID_HEADER;
    }

    return READ_OK;
}

// Проверка размера изображения
enum read_status check_image_size(uint32_t width, uint32_t height) {
    if (width <= 0 || height <= 0) {
        fprintf(stderr, "Error: Неверные размеры BMP");
        return READ_INVALID_HEADER;
    }

    uint64_t total_pixels = (uint64_t)width * (uint64_t)height;
    if (total_pixels > MAX_PIXELS) {
        fprintf(stderr, "Error: В файле BMP слишком много пикселей (%" PRIu64 "). Максимально допустимое значение: %d", total_pixels, MAX_PIXELS);
        return READ_MEMORY_ERROR;
    }

    return READ_OK;
}

// Вычисление паддинга
uint32_t calculate_padding(uint32_t width) {
    return (width * 3 + 3) & (~3);
}

// Выделение буфера строки
uint8_t* allocate_row_buffer(uint32_t row_padded) {
    uint8_t* row = malloc(row_padded);
    if (!row) {
        fprintf(stderr, "Error: Ошибка выделения памяти при чтении пикселей");
    }
    return row;
}

// Чтение данных пикселей
enum read_status read_pixel_data(FILE* in, struct image* img, uint32_t row_padded) {
    uint8_t* row = allocate_row_buffer(row_padded);
    if (!row) {
        return READ_MEMORY_ERROR;
    }

    for (uint32_t y = 0; y < img->height; y++) {
        size_t bytes_read = fread(row, 1, row_padded, in);
        if (bytes_read != row_padded) {
            fprintf(stderr, "Error: Не удалось прочитать пиксели.");
            free(row);
            image_free(img);
            return READ_IO_ERROR;
        }
        for (uint32_t x = 0; x < img->width; x++) {
            uint8_t b = row[x * 3];
            uint8_t g = row[x * 3 + 1];
            uint8_t r = row[x * 3 + 2];
            image_set_pixel(img, x, img->height - 1 - y, (struct pixel){b, g, r});
        }
    }
    free(row);
    return READ_OK;
}

// Чтение BMP
enum read_status from_bmp(FILE* in, struct image** img) {
    if (!in || !img) return READ_INVALID_HEADER;

    // Проверка размера файла
    fseek(in, 0, SEEK_END);
    long file_size = ftell(in);
    rewind(in); // Возврат к началу файла

    if (file_size == 0) {
        fprintf(stderr, "Error: Файл BMP пуст");
        return READ_IO_ERROR; 
    }

    struct bmp_header header;
    enum read_status status = read_bmp_header(in, &header);
    if (status != READ_OK) {
        return status;
    }

    status = validate_bmp_header(&header);
    if (status != READ_OK) {
        return status;
    }

    status = check_image_size(header.biWidth, header.biHeight);
    if (status != READ_OK) {
        return status;
    }

    // Перемещение указателя файла к позиции bOffBits
    if (header.bOffBits > sizeof(struct bmp_header)) {
        if (fseek(in, header.bOffBits, SEEK_SET) != 0) {
            fprintf(stderr, "Error: Не удалось выполнить поиск данных пикселей");
            return READ_IO_ERROR;
        }
    }

    // Создание внутреннего изображения
    struct image* image = image_create(header.biWidth, header.biHeight);
    if (!image) {
        fprintf(stderr, "Error: Ошибка выделения памяти при создании изображения");
        return READ_MEMORY_ERROR;
    }

    // Вычисление padding
    uint32_t row_padded = calculate_padding(image->width);

    // Чтение пикселей
    status = read_pixel_data(in, image, row_padded);
    if (status != READ_OK) {
        return status;
    }

    *img = image;
    return READ_OK;
}

// Запись заголовка BMP
enum write_status write_bmp_header(FILE* out, const struct image* img) {
    if (!out || !img) return WRITE_ERROR;

    struct bmp_header header;
    memset(&header, 0, sizeof(struct bmp_header));

    // Заполнение заголовка
    header.bfType = BMP_FILE_TYPE; 
    header.biSize = 40;
    header.biWidth = img->width;
    header.biHeight = img->height;
    header.biPlanes = 1;
    header.biBitCount = 24;
    header.biCompression = 0;

    // Вычисление padding
    uint32_t row_padded = calculate_padding(img->width);
    header.biSizeImage = row_padded * img->height;
    header.bfileSize = sizeof(struct bmp_header) + header.biSizeImage;
    header.bOffBits = sizeof(struct bmp_header);

    // Запись заголовка
    size_t written = fwrite(&header, sizeof(struct bmp_header), 1, out);
    if (written != 1) {
        fprintf(stderr, "Error: Не удалось записать заголовок BMP");
        return WRITE_IO_ERROR;
    }

    return WRITE_OK;
}

// Запись пикселей в BMP
enum write_status write_pixel_data(FILE* out, const struct image* img, uint32_t row_padded) {
    if (!out || !img) return WRITE_ERROR;

    // Подготовка padding
    uint32_t padding_size = row_padded - img->width * 3;
    static const uint32_t padding = 0;

    for (uint32_t y = 0; y < img->height; y++) {
        uint32_t row_idx = img->height - 1 - y;
        for (uint32_t x = 0; x < img->width; x++) {
            struct pixel p = image_get_pixel(img, x, row_idx);
            if (fwrite(&p.b, 1, 1, out) != 1 ||
                fwrite(&p.g, 1, 1, out) != 1 ||
                fwrite(&p.r, 1, 1, out) != 1) {
                fprintf(stderr, "Error: Не удалось записать пиксели");
                return WRITE_IO_ERROR;
            }
        }
        // Запись padding
        if (padding_size > 0) {
            if (fwrite(&padding, 1, padding_size, out) != padding_size) {
                fprintf(stderr, "Error: Не удалось записать padding");
                return WRITE_IO_ERROR;
            }
        }
    }

    return WRITE_OK;
}

// Запись BMP
enum write_status to_bmp(FILE* out, const struct image* img) {
    if (!out || !img) return WRITE_ERROR;

    enum write_status status = write_bmp_header(out, img);
    if (status != WRITE_OK) {
        return status;
    }
    uint32_t row_padded = calculate_padding(img->width);
    status = write_pixel_data(out, img, row_padded);
    if (status != WRITE_OK) {
        return status;
    }

    return WRITE_OK;
}

enum read_status from_bmp_filename(const char* filename, struct image** img) {
    if (!filename || !img) return READ_INVALID_HEADER;
    FILE* f = fopen(filename, "rb");
    if (!f) {
        fprintf(stderr, "Error: Невозможно открыть файл '%s' для чтения", filename);
        return READ_IO_ERROR;
    }
    enum read_status status = from_bmp(f, img);
    fclose(f);
    return status;
}

enum write_status to_bmp_filename(const char* filename, const struct image* img) {
    if (!filename || !img) return WRITE_ERROR;
    FILE* f = fopen(filename, "wb");
    if (!f) {
        fprintf(stderr, "Error: Невозможно открыть файл '%s' для записи", filename);
        return WRITE_IO_ERROR;
    }
    enum write_status status = to_bmp(f, img);
    if (fclose(f) != 0) {
        fprintf(stderr, "Warning: Не удалось закрыть файл '%s'", filename);
    }
    return status;
}
