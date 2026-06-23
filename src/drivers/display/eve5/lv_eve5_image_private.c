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
