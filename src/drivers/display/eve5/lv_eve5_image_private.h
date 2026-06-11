/**
 * @file lv_eve5_image_private.h
 *
 * EVE5 (BT820) Image Private Utilities
 *
 * Image header parsing, file extension checking, and EVE bitmap format
 * queries. Shared between the EVE5 draw unit and display/filesystem drivers.
 *
 * Copyright (C) 2025-2026  Bridgetek Pte Ltd
 * Author: Jan Boon <jan.boon@kaetemi.be>
 * SPDX-License-Identifier: MIT
 */
#ifndef LV_EVE5_IMAGE_PRIVATE_H
#define LV_EVE5_IMAGE_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../../../lvgl_public.h"

#if LV_USE_EVE5

#include "EVE_Hal.h"

#include <stdint.h>
#include <stdbool.h>

/**********************
 * IMAGE HEADER PARSING
 **********************/

/** Parse JPEG header to extract image dimensions (SOF0/SOF2 markers). */
bool eve5_parse_jpeg_dimensions(const uint8_t * data, uint32_t size,
                                uint32_t * width, uint32_t * height);

/** Parse PNG IHDR chunk to extract image dimensions. */
bool eve5_parse_png_dimensions(const uint8_t * data, uint32_t size,
                               uint32_t * width, uint32_t * height);

/**********************
 * FILE EXTENSION CHECK
 **********************/

/** Check if a file path ends with a given extension (case-insensitive, ext includes dot). */
bool eve5_has_extension(const char * path, const char * ext);

/** Check if a path is a JPEG or PNG file. Sets *is_jpeg and *is_png accordingly. */
bool eve5_is_jpeg_or_png(const char * path, bool *is_jpeg, bool *is_png);

/**********************
 * EVE FORMAT HELPERS
 **********************/

/** Get bytes per pixel for an EVE bitmap format. */
static inline int32_t eve5_format_bpp(uint32_t eve_format)
{
    switch(eve_format) {
#if (EVE_SUPPORT_CHIPID >= EVE_BT820)
        case ARGB8:
            return 4;
        case RGB8:
            return 3;
#endif
        case RGB565:
        case ARGB1555:
        case ARGB4:
#if (EVE_SUPPORT_CHIPID >= EVE_BT820)
        case YCBCR: /* 2x2 block format: line stride is 2 bytes/pixel, but each
                     * stored line covers two display rows (1 byte/pixel surface
                     * total — see eve5_rt_surface_size for sizing) */
#endif
            return 2;
        case L8:
#if (EVE_SUPPORT_CHIPID >= EVE_BT820)
        case PALETTEDARGB8:
#endif
            return 1;
        default:
            return 2;
    }
}

#endif /* LV_USE_EVE5 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_EVE5_IMAGE_PRIVATE_H */
