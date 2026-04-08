/**
 * @file lv_eve5_image_private.c
 *
 * EVE5 (BT820) Image Private Utilities
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */

#include "../../../lv_conf_internal.h"

#if LV_USE_EVE5

#include "lv_eve5_image_private.h"

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

#endif /* LV_USE_EVE5 */
