/**
 * @file lv_vglite_buf.c
 *
 */

/**
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: MIT
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_vglite_buf.h"

#if LV_USE_DRAW_VGLITE
#include "lv_vglite_utils.h"

#include "../../../stdlib/lv_string.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static inline void _set_vgbuf_ptr(vg_lite_buffer_t * vgbuf, void * buf);

/**********************
 *  STATIC VARIABLES
 **********************/

static vg_lite_buffer_t _dest_vgbuf;
static vg_lite_buffer_t _src_vgbuf;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

vg_lite_buffer_t * vglite_get_dest_buf(void)
{
    return &_dest_vgbuf;
}

vg_lite_buffer_t * vglite_get_src_buf(void)
{
    return &_src_vgbuf;
}

void vglite_set_dest_buf_ptr(void * buf)
{
    _set_vgbuf_ptr(&_dest_vgbuf, buf);
}

void vglite_set_src_buf_ptr(const void * buf)
{
    _set_vgbuf_ptr(&_src_vgbuf, (void *)buf);
}

void vglite_set_dest_buf(const void * buf, uint32_t width, uint32_t height, uint32_t stride,
                         lv_color_format_t cf)
{
    vglite_set_buf(&_dest_vgbuf, (void *)buf, width, height, stride, cf);
}

void vglite_set_src_buf(const void * buf, uint32_t width, uint32_t height, uint32_t stride,
                        lv_color_format_t cf)
{
    vglite_set_buf(&_src_vgbuf, (void *)buf, width, height, stride, cf);
}

void vglite_set_buf(vg_lite_buffer_t * vgbuf, void * buf,
                    uint32_t width, uint32_t height, uint32_t stride,
                    lv_color_format_t cf)
{
    vg_lite_buffer_format_t vgformat = vglite_get_buf_format(cf);

    vgbuf->format = vgformat;
    vgbuf->tiled = VG_LITE_LINEAR;
    vgbuf->image_mode = VG_LITE_NORMAL_IMAGE_MODE;
    vgbuf->transparency_mode = VG_LITE_IMAGE_OPAQUE;

    vgbuf->width = (int32_t)width;
    vgbuf->height = (int32_t)height;
    vgbuf->stride = (int32_t)stride;

    lv_memzero(&vgbuf->yuv, sizeof(vgbuf->yuv));

    vgbuf->memory = buf;
    vgbuf->address = (uint32_t)vgbuf->memory;
    vgbuf->handle = NULL;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static inline void _set_vgbuf_ptr(vg_lite_buffer_t * vgbuf, void * buf)
{
    vgbuf->memory = buf;
    vgbuf->address = (uint32_t)vgbuf->memory;
}

#endif /*LV_USE_DRAW_VGLITE*/
