/**
 * @file lv_vglite_buf.c
 *
 */

/**
 * MIT License
 *
 * Copyright 2023 NXP
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next paragraph)
 * shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_vglite_buf.h"

#if LV_USE_GPU_NXP_VG_LITE

/*********************
 *      DEFINES
 *********************/

#if LV_COLOR_DEPTH == 16
    #define VG_LITE_PX_FMT VG_LITE_RGB565
#elif LV_COLOR_DEPTH == 32
    #define VG_LITE_PX_FMT VG_LITE_BGRA8888
#else
    #error Only 16bit and 32bit color depth are supported. Set LV_COLOR_DEPTH to 16 or 32.
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static inline void lv_vglite_set_dest_buf(const lv_color_t * buf, const lv_area_t * area, lv_coord_t stride);
static inline void lv_vglite_set_buf_ptr(vg_lite_buffer_t * vgbuf, const lv_color_t * buf);

/**********************
 *  STATIC VARIABLES
 **********************/

static vg_lite_buffer_t dest_vgbuf;
static vg_lite_buffer_t src_vgbuf;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_gpu_nxp_vglite_init_buf(const lv_color_t * buf, const lv_area_t * area, lv_coord_t stride)
{
    lv_vglite_set_dest_buf(buf, area, stride);
}

vg_lite_buffer_t * lv_vglite_get_dest_buf(void)
{
    return &dest_vgbuf;
}

vg_lite_buffer_t * lv_vglite_get_src_buf(void)
{
    return &src_vgbuf;
}

void lv_vglite_set_dest_buf_ptr(const lv_color_t * buf)
{
    lv_vglite_set_buf_ptr(&dest_vgbuf, buf);
}

void lv_vglite_set_src_buf_ptr(const lv_color_t * buf)
{
    lv_vglite_set_buf_ptr(&src_vgbuf, buf);
}

void lv_vglite_set_src_buf(const lv_color_t * buf, const lv_area_t * area, lv_coord_t stride)
{
    if(src_vgbuf.memory != (void *)buf)
        lv_vglite_set_buf(&src_vgbuf, buf, area, stride);
}

void lv_vglite_set_buf(vg_lite_buffer_t * vgbuf, const lv_color_t * buf,
                       const lv_area_t * area, lv_coord_t stride)
{
    vgbuf->format = VG_LITE_PX_FMT;
    vgbuf->tiled = VG_LITE_LINEAR;
    vgbuf->image_mode = VG_LITE_NORMAL_IMAGE_MODE;
    vgbuf->transparency_mode = VG_LITE_IMAGE_OPAQUE;

    vgbuf->width = (int32_t)lv_area_get_width(area);
    vgbuf->height = (int32_t)lv_area_get_height(area);
    vgbuf->stride = (int32_t)(stride) * sizeof(lv_color_t);

    lv_memset_00(&vgbuf->yuv, sizeof(vgbuf->yuv));

    vgbuf->memory = (void *)buf;
    vgbuf->address = (uint32_t)vgbuf->memory;
    vgbuf->handle = NULL;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static inline void lv_vglite_set_dest_buf(const lv_color_t * buf, const lv_area_t * area, lv_coord_t stride)
{
    lv_vglite_set_buf(&dest_vgbuf, buf, area, stride);
}

static inline void lv_vglite_set_buf_ptr(vg_lite_buffer_t * vgbuf, const lv_color_t * buf)
{
    vgbuf->memory = (void *)buf;
    vgbuf->address = (uint32_t)vgbuf->memory;
}

#endif /*LV_USE_GPU_NXP_VG_LITE*/
