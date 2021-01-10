/**
 * @file lv_gpu_nxp_vglite.c
 *
 */

/**
 * MIT License
 *
 * Copyright (c) 2020 NXP
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

#include "../lv_conf_internal.h"

#if LV_USE_GPU_NXP_VG_LITE

#include "lvgl.h"
#include "lv_gpu_nxp_vglite.h"
#include "../lv_misc/lv_log.h"
#include "fsl_cache.h"
#include "vg_lite.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

#if LV_COLOR_DEPTH==16
    #define VGLITE_PX_FMT VG_LITE_RGB565
#else
    #error Only 16bit color depth is supported. Set LV_COLOR_DEPTH to 16.
#endif

/**********************
 *  STATIC PROTOTYPES
 **********************/

static lv_res_t init_vg_buf(vg_lite_buffer_t * dst, uint32_t width, uint32_t height, uint32_t stride,
                            const lv_color_t * ptr);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/***
 * Fills rectangular area in buffer.
 * @param[in] dest_buf Destination buffer pointer (must be aligned on 32 bytes)
 * @param[in] dest_width Destination buffer width in pixels (must be aligned on 16 px)
 * @param[in] dest_height Destination buffer height in pixels
 * @param[in] fill_area Area to be filled
 * @param[in] color Fill color
 * @param[in] opa Opacity (255 = full, 128 = 50% background/50% color, 0 = no fill)
 * @retval LV_RES_OK Fill completed
 * @retval LV_RES_INV Error occurred (\see LV_GPU_NXP_VG_LITE_LOG_ERRORS)
 */
lv_res_t lv_gpu_nxp_vglite_fill(lv_color_t * dest_buf, lv_coord_t dest_width, lv_coord_t dest_height,
                                const lv_area_t * fill_area, lv_color_t color, lv_opa_t opa)
{
    vg_lite_buffer_t rt;
    vg_lite_rectangle_t rect;
    vg_lite_error_t err = VG_LITE_SUCCESS;
    lv_color32_t col32 = {.full = lv_color_to32(color)}; /* Convert color to RGBA8888 */
    lv_disp_t * disp = _lv_refr_get_disp_refreshing();

    if(init_vg_buf(&rt, dest_width, dest_height, dest_width * sizeof(lv_color_t), dest_buf) != LV_RES_OK) {
#if LV_GPU_NXP_VG_LITE_LOG_ERRORS
        LV_LOG_ERROR("init_vg_buf reported error. Fill failed.");
#endif
        return LV_RES_INV;
    }

    if(opa >= LV_OPA_MAX) {   /* Opaque fill */
        rect.x = fill_area->x1;
        rect.y = fill_area->y1;
        rect.width = (fill_area->x2 - fill_area->x1) + 1;
        rect.height = (fill_area->y2 - fill_area->y1) + 1;

        if(disp && disp->driver.clean_dcache_cb) {  /* Clean & invalidate cache */
            disp->driver.clean_dcache_cb(&disp->driver);
        }

        err |= vg_lite_clear(&rt, &rect, col32.full);
        err |= vg_lite_finish();
    }
    else {   /* fill with transparency */

        vg_lite_path_t path;
        lv_color32_t colMix;
        int16_t path_data[] = { /* VG rectangular path */
            VLC_OP_MOVE, fill_area->x1,  fill_area->y1,
            VLC_OP_LINE, fill_area->x2 + 1,  fill_area->y1,
            VLC_OP_LINE, fill_area->x2 + 1,  fill_area->y2 + 1,
            VLC_OP_LINE, fill_area->x1,  fill_area->y2 + 1,
            VLC_OP_LINE, fill_area->x1,  fill_area->y1,
            VLC_OP_END
        };

        err |= vg_lite_init_path(&path, VG_LITE_S16, VG_LITE_LOW, sizeof(path_data), path_data,
                                 fill_area->x1, fill_area->y1, fill_area->x2 + 1, fill_area->y2 + 1);
        if(err != VG_LITE_SUCCESS)  {
#if LV_GPU_NXP_VG_LITE_LOG_ERRORS
            LV_LOG_ERROR("vg_lite_init_path() failed.");
#endif
            return LV_RES_INV;
        }

        colMix.ch.red = ((uint16_t)col32.ch.red * opa) >> 8; /* Pre-multiply color */
        colMix.ch.green = ((uint16_t)col32.ch.green * opa) >> 8;
        colMix.ch.blue = ((uint16_t)col32.ch.blue * opa) >> 8;
        colMix.ch.alpha = opa;

        if(disp && disp->driver.clean_dcache_cb) {  /* Clean & invalidate cache */
            disp->driver.clean_dcache_cb(&disp->driver);
        }

        vg_lite_matrix_t matrix;
        vg_lite_identity(&matrix);

        /* Draw rectangle */
        err |= vg_lite_draw(&rt, &path, VG_LITE_FILL_EVEN_ODD, &matrix, VG_LITE_BLEND_SRC_OVER, colMix.full);
        if(err)  {
#if LV_GPU_NXP_VG_LITE_LOG_ERRORS
            LV_LOG_ERROR("vg_lite_draw() failed.");
#endif
            vg_lite_clear_path(&path);
            return LV_RES_INV;
        }

        err |= vg_lite_finish();
        err |= vg_lite_clear_path(&path);
    }

    if(err == VG_LITE_SUCCESS) {
        return LV_RES_OK;
    }
    else {
#if LV_GPU_NXP_VG_LITE_LOG_ERRORS
        LV_LOG_ERROR("VG Lite Fill failed.");
#endif
        return LV_RES_INV;
    }
}

/***
 * BLock Image Transfer.
 * @param[in] blit Description of the transfer
 * @retval LV_RES_OK Transfer complete
 * @retval LV_RES_INV Error occurred (\see LV_GPU_NXP_VG_LITE_LOG_ERRORS)
 */
lv_res_t lv_gpu_nxp_vglite_blit(lv_gpu_nxp_vglite_blit_info_t * blit)
{
    vg_lite_buffer_t src_vgbuf, dst_vgbuf;
    vg_lite_error_t err = VG_LITE_SUCCESS;
    uint32_t rect[4];
    lv_disp_t * disp = _lv_refr_get_disp_refreshing();

    if(blit->opa < LV_OPA_MIN) {
        return LV_RES_OK; /* Nothing to BLIT */
    }

    if(!blit) {
        /* Wrong parameter */
        return LV_RES_INV;
    }

    /* Wrap src/dst buffer into VG-Lite buffer */
    if(init_vg_buf(&src_vgbuf, blit->src_width, blit->src_height, blit->src_stride, blit->src) != LV_RES_OK) {
#if LV_GPU_NXP_VG_LITE_LOG_ERRORS
        LV_LOG_ERROR("init_vg_buf reported error. BLIT failed.");
#endif
        return LV_RES_INV;
    }

    if(init_vg_buf(&dst_vgbuf, blit->dst_width, blit->dst_height, blit->dst_stride, blit->dst) != LV_RES_OK) {
#if LV_GPU_NXP_VG_LITE_LOG_ERRORS
        LV_LOG_ERROR("init_vg_buf reported error. BLIT failed.");
#endif
        return LV_RES_INV;
    }

    rect[0] = 0; /* Crop */
    rect[1] = 0;
    rect[2] = blit->src_width;
    rect[3] = blit->src_height;

    vg_lite_matrix_t matrix;
    vg_lite_identity(&matrix);
    vg_lite_translate(blit->dst_area.x1, blit->dst_area.y1, &matrix);

    if(disp && disp->driver.clean_dcache_cb) {  /* Clean & invalidate cache */
        disp->driver.clean_dcache_cb(&disp->driver);
    }

    uint32_t color;
    vg_lite_blend_t blend;
    if(blit->opa >= LV_OPA_MAX) {
        color = 0x0;
        blend = VG_LITE_BLEND_NONE;
    }
    else {
        color = ((blit->opa) << 24) | ((blit->opa) << 16) | ((blit->opa) << 8) | (blit->opa);
        blend = VG_LITE_BLEND_SRC_OVER;
        src_vgbuf.image_mode = VG_LITE_MULTIPLY_IMAGE_MODE;
    }
    err |= vg_lite_blit_rect(&dst_vgbuf, &src_vgbuf, rect, &matrix, blend, color, VG_LITE_FILTER_POINT);

    err |= vg_lite_finish();

    if(err == VG_LITE_SUCCESS) {
        return LV_RES_OK;
    }
    else {
#if LV_GPU_NXP_VG_LITE_LOG_ERRORS
        LV_LOG_ERROR("vg_lite_blit_rect or vg_lite_finish reported error. BLIT failed.");
#endif
        return LV_RES_INV;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/***
 * Fills vg_lite_buffer_t structure according given parameters.
 * @param[out] dst Buffer structure to be filled
 * @param[in] width Width of buffer in pixels
 * @param[in] height Height of buffer in pixels
 * @param[in] stride Stride of the buffer in bytes
 * @param[in] ptr Pointer to the buffer (must be aligned according VG-Lite requirements)
 */
static lv_res_t init_vg_buf(vg_lite_buffer_t * dst, uint32_t width, uint32_t height, uint32_t stride,
                            const lv_color_t * ptr)
{
    if((((uintptr_t)ptr) % LV_ATTRIBUTE_MEM_ALIGN_SIZE) != 0x0) {  /* Test for alignment */
#if LV_GPU_NXP_VG_LITE_LOG_ERRORS
        LV_LOG_ERROR("ptr (0x%X) not aligned to %d.", (size_t) ptr, LV_ATTRIBUTE_MEM_ALIGN_SIZE);
#endif
        return LV_RES_INV;
    }

    if((stride % LV_GPU_NXP_VG_LITE_STRIDE_ALIGN_PX) != 0x0) {  /* Test for stride alignment */
#if LV_GPU_NXP_VG_LITE_LOG_ERRORS
        LV_LOG_ERROR("Buffer stride (%d px) not aligned to %d px.", stride, LV_GPU_NXP_VG_LITE_STRIDE_ALIGN_PX);
#endif
        return LV_RES_INV;
    }

    dst->format = VGLITE_PX_FMT;
    dst->tiled = VG_LITE_LINEAR;
    dst->image_mode = VG_LITE_NORMAL_IMAGE_MODE;
    dst->transparency_mode = VG_LITE_IMAGE_OPAQUE;

    dst->width = width;
    dst->height = height;
    dst->stride = stride;

    memset(&dst->yuv, 0, sizeof(dst->yuv));

    dst->memory = (void *) ptr;
    dst->address = (uint32_t) dst->memory;
    dst->handle = 0x0;

    return LV_RES_OK;
}

#endif /* LV_USE_GPU_NXP_VG_LITE */
