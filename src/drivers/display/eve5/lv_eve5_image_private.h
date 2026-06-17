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

/**********************
 * ESDM METADATA SIDECAR
 *
 * Binary metadata sidecar placed next to an asset file: for "image.raw" the
 * sidecar is "image.raw.esdm". Provides the EVE bitmap format, dimensions,
 * stride, and load method that a raw/compressed asset file does not carry
 * itself. Mirrors the ESD Core "BMP" record (see esd_core/Esd_BitmapInfo.c
 * and IMAGE_FORMATS.md). Only the BMP type is consumed here.
 **********************/

/** Metadata sidecar filename extension (matches EVE_METADATA_FILE_EXT). */
#define EVE5_ESDM_EXT     ".esdm"
#define EVE5_ESDM_EXT_LEN 5
/** Largest metadata sidecar we read (matches ESD ESD_METADATA_MAX). */
#define EVE5_ESDM_MAX     64
/** Size of a complete BMP metadata record (12-byte header + 44 type bytes). */
#define EVE5_ESDM_BMP_SIZE 56

/** Resource compression / load method (matches ESD Esd_ResourceCompression). */
#define EVE5_ESDM_RAW     0  /**< Uncompressed; direct copy / CMD_FSREAD */
#define EVE5_ESDM_DEFLATE 1  /**< Deflate compressed; CMD_INFLATE */
#define EVE5_ESDM_IMAGE   2  /**< JPEG/PNG; CMD_LOADIMAGE (handled by the .jpg/.png path) */
#define EVE5_ESDM_ASSET   3  /**< Relocatable .reloc asset; CMD_LOADASSET */

/** Parsed BMP-type metadata sidecar. */
typedef struct {
    uint8_t  compression;     /**< EVE5_ESDM_* load method */
    uint8_t  ext_len;         /**< Length of the resource file's extension (e.g. 4 for ".raw") */
    uint32_t raw_size;        /**< Uncompressed bitmap data size in bytes (excludes the palette) */
    int32_t  width;
    int32_t  height;
    int32_t  stride;          /**< Row stride in bytes */
    uint32_t format;          /**< EVE bitmap format enum value */
    uint16_t palette_size;    /**< Palette sidecar file size in bytes (0 = not paletted) */
    char     palette_ext[11]; /**< Palette filename suffix, NUL-terminated (empty = use ".pal.raw") */
    uint16_t cells;
    bool     has_swizzle;     /**< BT815+ swizzle enable flag (not applied by the LVGL driver) */
    uint8_t  swizzle_r, swizzle_g, swizzle_b, swizzle_a;
} eve5_esdm_bmp_t;

/** Parse a BMP-type .esdm sidecar buffer. Returns false if it isn't a valid
 *  version-1 "BMP" record with positive width/height/stride. */
bool eve5_parse_esdm_bmp(const uint8_t * data, uint32_t size, eve5_esdm_bmp_t * out);

/** Build "<path>.esdm" into @p out. Returns false if it doesn't fit. */
bool eve5_esdm_meta_path(const char * path, char * out, uint32_t out_size);

/** Build the palette sidecar path for a paletted resource: strips @p ext_len
 *  trailing characters from @p path and appends @p pal_ext, or ".pal.raw" when
 *  @p pal_ext is empty (the converter leaves the metadata field blank). Returns
 *  false if @p ext_len exceeds the path length or the result doesn't fit. */
bool eve5_esdm_palette_path(const char * path, uint8_t ext_len, const char * pal_ext,
                            char * out, uint32_t out_size);

#endif /* LV_USE_EVE5 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_EVE5_IMAGE_PRIVATE_H */
