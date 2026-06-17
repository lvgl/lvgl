/**
 * @file lv_eve5_image_private.c
 *
 * EVE5 (BT820) Image Private Utilities
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */

#include "lv_eve5_image_private.h"

#if LV_USE_EVE5

/**********************
 * IMAGE HEADER PARSING
 **********************/

bool eve5_parse_jpeg_dimensions(const uint8_t * data, uint32_t size,
                                uint32_t * width, uint32_t * height)
{
    if(size < 11) return false;
    if(data[0] != 0xFF || data[1] != 0xD8) return false;

    uint32_t pos = 2;
    while(pos + 4 < size) {
        if(data[pos] != 0xFF) {
            pos++;
            continue;
        }

        uint8_t marker = data[pos + 1];

        /* SOF0 (baseline) or SOF2 (progressive) contain dimensions */
        if(marker == 0xC0 || marker == 0xC2) {
            if(pos + 9 > size) return false;
            *height = ((uint32_t)data[pos + 5] << 8) | data[pos + 6];
            *width = ((uint32_t)data[pos + 7] << 8) | data[pos + 8];
            return true;
        }

        /* Skip standalone markers vs length-prefixed segments */
        if(marker == 0xD8 || marker == 0xD9 || (marker >= 0xD0 && marker <= 0xD7)) {
            pos += 2;
        }
        else if(pos + 4 <= size) {
            uint32_t len = ((uint32_t)data[pos + 2] << 8) | data[pos + 3];
            pos += 2 + len;
        }
        else {
            break;
        }
    }

    return false;
}

bool eve5_parse_png_dimensions(const uint8_t * data, uint32_t size,
                               uint32_t * width, uint32_t * height)
{
    if(size < 24) return false;

    static const uint8_t png_sig[8] = {0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A};
    for(int i = 0; i < 8; i++) {
        if(data[i] != png_sig[i]) return false;
    }

    /* IHDR chunk must be first */
    if(data[12] != 'I' || data[13] != 'H' || data[14] != 'D' || data[15] != 'R') return false;

    *width = ((uint32_t)data[16] << 24) | ((uint32_t)data[17] << 16) |
             ((uint32_t)data[18] << 8) | data[19];
    *height = ((uint32_t)data[20] << 24) | ((uint32_t)data[21] << 16) |
              ((uint32_t)data[22] << 8) | data[23];

    return true;
}

/**********************
 * FILE EXTENSION CHECK
 **********************/

bool eve5_has_extension(const char * path, const char * ext)
{
    size_t path_len = lv_strlen(path);
    size_t ext_len = lv_strlen(ext);
    if(path_len < ext_len + 1) return false;

    const char * path_ext = path + path_len - ext_len;
    for(size_t i = 0; i < ext_len; i++) {
        char c1 = path_ext[i];
        char c2 = ext[i];
        if(c1 >= 'A' && c1 <= 'Z') c1 += ('a' - 'A');
        if(c2 >= 'A' && c2 <= 'Z') c2 += ('a' - 'A');
        if(c1 != c2) return false;
    }

    return true;
}

bool eve5_is_jpeg_or_png(const char * path, bool *is_jpeg, bool *is_png)
{
    *is_jpeg = eve5_has_extension(path, ".jpg") || eve5_has_extension(path, ".jpeg");
    *is_png = eve5_has_extension(path, ".png");
    return *is_jpeg || *is_png;
}

/**********************
 * ESDM METADATA SIDECAR
 **********************/

/* "BMP" type signature, NUL-terminated ASCII, little-endian (0x00504D42). */
#define EVE5_ESDM_SIG_BMP 0x00504D42u

static inline uint16_t eve5_rd16le(const uint8_t * p, uint32_t o)
{
    return (uint16_t)(p[o] | ((uint16_t)p[o + 1] << 8));
}

static inline uint32_t eve5_rd32le(const uint8_t * p, uint32_t o)
{
    return (uint32_t)p[o] | ((uint32_t)p[o + 1] << 8)
           | ((uint32_t)p[o + 2] << 16) | ((uint32_t)p[o + 3] << 24);
}

bool eve5_parse_esdm_bmp(const uint8_t * data, uint32_t size, eve5_esdm_bmp_t * out)
{
    if(data == NULL || out == NULL || size < EVE5_ESDM_BMP_SIZE) return false;

    /* Common header (12 bytes): signature, version, size, compression, extLen, rawSize. */
    if(eve5_rd32le(data, 0) != EVE5_ESDM_SIG_BMP) return false;
    if(data[4] != 1) return false;                    /* version */
    uint8_t rec_size = data[5];
    if(rec_size < EVE5_ESDM_BMP_SIZE || rec_size > size) return false;

    lv_memzero(out, sizeof(*out));
    out->compression = data[6];
    out->ext_len = data[7];
    out->raw_size = eve5_rd32le(data, 8);

    /* BMP type-specific (offsets from start, see IMAGE_FORMATS.md). */
    out->width = (int32_t)eve5_rd32le(data, 12);
    out->height = (int32_t)eve5_rd32le(data, 16);
    out->stride = (int32_t)eve5_rd32le(data, 20);
    out->format = eve5_rd32le(data, 24);
    out->palette_size = eve5_rd16le(data, 28);
    for(int i = 0; i < 10; i++) out->palette_ext[i] = (char)data[30 + i];
    out->palette_ext[10] = '\0';
    out->cells = eve5_rd16le(data, 52);

    uint16_t swizzle = eve5_rd16le(data, 54);
    out->has_swizzle = (swizzle & (1u << 12)) != 0;
    out->swizzle_b = (uint8_t)((swizzle >> 9) & 7);
    out->swizzle_g = (uint8_t)((swizzle >> 6) & 7);
    out->swizzle_r = (uint8_t)((swizzle >> 3) & 7);
    out->swizzle_a = (uint8_t)(swizzle & 7);

    if(out->width <= 0 || out->height <= 0 || out->stride <= 0) return false;
    return true;
}

bool eve5_esdm_meta_path(const char * path, char * out, uint32_t out_size)
{
    if(path == NULL || out == NULL) return false;
    uint32_t n = (uint32_t)lv_strlen(path);
    if(n + EVE5_ESDM_EXT_LEN + 1 > out_size) return false;
    lv_memcpy(out, path, n);
    lv_memcpy(out + n, EVE5_ESDM_EXT, EVE5_ESDM_EXT_LEN + 1);  /* includes NUL */
    return true;
}

bool eve5_esdm_palette_path(const char * path, uint8_t ext_len, const char * pal_ext,
                            char * out, uint32_t out_size)
{
    if(path == NULL || out == NULL) return false;
    uint32_t n = (uint32_t)lv_strlen(path);
    if(ext_len > n) return false;
    uint32_t base = n - ext_len;

    /* The converter leaves the metadata suffix blank; fall back to its fixed
     * ".pal.raw" naming so a paletted resource's palette can still be found. */
    const char * suffix = (pal_ext != NULL && pal_ext[0] != '\0') ? pal_ext : ".pal.raw";
    uint32_t slen = (uint32_t)lv_strlen(suffix);
    if(base + slen + 1 > out_size) return false;

    lv_memcpy(out, path, base);
    lv_memcpy(out + base, suffix, slen + 1);  /* includes NUL */
    return true;
}

#endif /* LV_USE_EVE5 */
