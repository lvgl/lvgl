/**
 * @file lv_draw_vglite_blend.c
 *
 */

/**
 * MIT License
 *
 * Copyright 2020-2023 NXP
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

#include "lv_draw_vglite_blend.h"

#if LV_USE_GPU_NXP_VG_LITE
#include "lv_vglite_buf.h"
#include "lv_vglite_utils.h"

/*********************
 *      DEFINES
 *********************/

/** Stride in px required by VG-Lite HW*/
#define LV_GPU_NXP_VG_LITE_STRIDE_ALIGN_PX 16U

#if VG_LITE_BLIT_SPLIT_ENABLED
    /**
    * BLIT split threshold - BLITs with width or height higher than this value will be done
    * in multiple steps. Value must be 16-aligned. Don't change.
    */
    #define LV_GPU_NXP_VG_LITE_BLIT_SPLIT_THR 352
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**
 * Blit image, with optional opacity.
 *
 * @param[in] src_area Source area with relative coordinates of source buffer
 * @param[in] opa Opacity
 *
 * @retval LV_RES_OK Transfer complete
 * @retval LV_RES_INV Error occurred (\see LV_GPU_NXP_VG_LITE_LOG_ERRORS)
 */
static lv_res_t lv_vglite_blit(const lv_area_t * src_area, lv_opa_t opa);

/**
 * Check source memory and stride alignment.
 *
 * @param[in] src_buf Source buffer
 * @param[in] src_stride Stride of source buffer in pixels
 *
 * @retval LV_RES_OK Alignment OK
 * @retval LV_RES_INV Error occurred (\see LV_GPU_NXP_VG_LITE_LOG_ERRORS)
 */
static lv_res_t check_src_alignment(const lv_color_t * src_buf, lv_coord_t src_stride);

/**
 * Creates matrix that translates to origin of given destination area.
 *
 * @param[in] dest_area Area with relative coordinates of destination buffer
 */
static inline void lv_vglite_set_translation_matrix(const lv_area_t * dest_area);

/**
 * Creates matrix that translates to origin of given destination area with transformation (scale or rotate).
 *
 * @param[in] dest_area Area with relative coordinates of destination buffer
 * @param[in] dsc Image descriptor
 */
static inline void lv_vglite_set_transformation_matrix(const lv_area_t * dest_area, const lv_draw_img_dsc_t * dsc);

#if VG_LITE_BLIT_SPLIT_ENABLED
/**
 * Move buffer pointer as close as possible to area, but with respect to alignment requirements. X-axis only.
 *
 * @param[in/out] area Area to be updated
 * @param[in/out] buf Pointer to be updated
 */
static void align_x(lv_area_t * area, lv_color_t ** buf);

/**
 * Move buffer pointer to the area start and update variables, Y-axis only.
 *
 * @param[in/out] area Area to be updated
 * @param[in/out] buf Pointer to be updated
 * @param[in] stride Buffer stride in pixels
 */
static void align_y(lv_area_t * area, lv_color_t ** buf, lv_coord_t stride);

/**
 * Blit image split in tiles, with optional opacity.
 *
 * @param[in/out] dest_buf Destination buffer
 * @param[in] dest_area Area with relative coordinates of destination buffer
 * @param[in] dest_stride Stride of destination buffer in pixels
 * @param[in] src_buf Source buffer
 * @param[in] src_area Source area with relative coordinates of source buffer
 * @param[in] src_stride Stride of source buffer in pixels
 * @param[in] opa Opacity
 *
 * @retval LV_RES_OK Transfer complete
 * @retval LV_RES_INV Error occurred (\see LV_GPU_NXP_VG_LITE_LOG_ERRORS)
 */
static lv_res_t lv_vglite_blit_split(lv_color_t * dest_buf, lv_area_t * dest_area, lv_coord_t dest_stride,
                                     const lv_color_t * src_buf, lv_area_t * src_area, lv_coord_t src_stride,
                                     lv_opa_t opa);
#endif /*VG_LITE_BLIT_SPLIT_ENABLED*/

/**********************
 *  STATIC VARIABLES
 **********************/

static vg_lite_matrix_t vgmatrix;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_res_t lv_gpu_nxp_vglite_fill(const lv_area_t * dest_area, lv_color_t color, lv_opa_t opa)
{
    vg_lite_error_t err = VG_LITE_SUCCESS;
    lv_color32_t col32 = {.full = lv_color_to32(color)}; /*Convert color to RGBA8888*/
    vg_lite_color_t vgcol; /* vglite takes ABGR */
    vg_lite_buffer_t * vgbuf = lv_vglite_get_dest_buf();

    vg_lite_buffer_format_t color_format = LV_COLOR_DEPTH == 16 ? VG_LITE_BGRA8888 : VG_LITE_ABGR8888;
    if(lv_vglite_premult_and_swizzle(&vgcol, col32, opa, color_format) != LV_RES_OK)
        VG_LITE_RETURN_INV("Premultiplication and swizzle failed.");

    if(opa >= (lv_opa_t)LV_OPA_MAX) {   /*Opaque fill*/
        vg_lite_rectangle_t rect = {
            .x = dest_area->x1,
            .y = dest_area->y1,
            .width = lv_area_get_width(dest_area),
            .height = lv_area_get_height(dest_area)
        };

        err = vg_lite_clear(vgbuf, &rect, vgcol);
        VG_LITE_ERR_RETURN_INV(err, "Clear failed.");

        if(lv_vglite_run() != LV_RES_OK)
            VG_LITE_RETURN_INV("Run failed.");
    }
    else {   /*fill with transparency*/

        vg_lite_path_t path;
        int32_t path_data[] = { /*VG rectangular path*/
            VLC_OP_MOVE, dest_area->x1,  dest_area->y1,
            VLC_OP_LINE, dest_area->x2 + 1,  dest_area->y1,
            VLC_OP_LINE, dest_area->x2 + 1,  dest_area->y2 + 1,
            VLC_OP_LINE, dest_area->x1,  dest_area->y2 + 1,
            VLC_OP_LINE, dest_area->x1,  dest_area->y1,
            VLC_OP_END
        };

        err = vg_lite_init_path(&path, VG_LITE_S32, VG_LITE_LOW, sizeof(path_data), path_data,
                                (vg_lite_float_t) dest_area->x1, (vg_lite_float_t) dest_area->y1,
                                ((vg_lite_float_t) dest_area->x2) + 1.0f, ((vg_lite_float_t) dest_area->y2) + 1.0f);
        VG_LITE_ERR_RETURN_INV(err, "Init path failed.");

        vg_lite_matrix_t matrix;
        vg_lite_identity(&matrix);

        /*Draw rectangle*/
        err = vg_lite_draw(vgbuf, &path, VG_LITE_FILL_EVEN_ODD, &matrix, VG_LITE_BLEND_SRC_OVER, vgcol);
        VG_LITE_ERR_RETURN_INV(err, "Draw rectangle failed.");

        if(lv_vglite_run() != LV_RES_OK)
            VG_LITE_RETURN_INV("Run failed.");

        err = vg_lite_clear_path(&path);
        VG_LITE_ERR_RETURN_INV(err, "Clear path failed.");
    }

    return LV_RES_OK;
}

#if VG_LITE_BLIT_SPLIT_ENABLED
lv_res_t lv_gpu_nxp_vglite_blit_split(lv_color_t * dest_buf, lv_area_t * dest_area, lv_coord_t dest_stride,
                                      const lv_color_t * src_buf, lv_area_t * src_area, lv_coord_t src_stride,
                                      lv_opa_t opa)
{
    /* Set src_vgbuf structure. */
    lv_vglite_set_src_buf(src_buf, src_area, src_stride);

    lv_color_t * orig_dest_buf = dest_buf;

    lv_res_t rv = lv_vglite_blit_split(dest_buf, dest_area, dest_stride, src_buf, src_area, src_stride, opa);

    /* Restore the original dest_vgbuf memory address. */
    lv_vglite_set_dest_buf_ptr(orig_dest_buf);

    return rv;
}
#else
lv_res_t lv_gpu_nxp_vglite_blit(const lv_area_t * dest_area,
                                const lv_color_t * src_buf, const lv_area_t * src_area, lv_coord_t src_stride,
                                lv_opa_t opa)
{
    if(check_src_alignment(src_buf, src_stride) != LV_RES_OK)
        VG_LITE_RETURN_INV("Check src alignment failed.");

    /* Set src_vgbuf structure. */
    lv_vglite_set_src_buf(src_buf, src_area, src_stride);

    /* Set scissor. */
    lv_vglite_set_scissor(dest_area);

    /* Set vgmatrix. */
    lv_vglite_set_translation_matrix(dest_area);

    /* Start blit. */
    lv_res_t rv = lv_vglite_blit(src_area, opa);

    /* Disable scissor. */
    lv_vglite_disable_scissor();

    return rv;
}

lv_res_t lv_gpu_nxp_vglite_blit_transform(const lv_area_t * dest_area, const lv_area_t * clip_area,
                                          const lv_color_t * src_buf, const lv_area_t * src_area, lv_coord_t src_stride,
                                          const lv_draw_img_dsc_t * dsc)
{
    lv_res_t rv = LV_RES_INV;

    if(check_src_alignment(src_buf, src_stride) != LV_RES_OK)
        VG_LITE_RETURN_INV("Check src alignment failed.");

    /* Set src_vgbuf structure. */
    lv_vglite_set_src_buf(src_buf, src_area, src_stride);

    /* Set scissor */
    lv_vglite_set_scissor(clip_area);

    /* Set vgmatrix. */
    lv_vglite_set_transformation_matrix(dest_area, dsc);

    /* Start blit. */
    rv = lv_vglite_blit(src_area, dsc->opa);

    /* Disable scissor. */
    lv_vglite_disable_scissor();

    return rv;
}
#endif /*VG_LITE_BLIT_SPLIT_ENABLED*/

lv_res_t lv_gpu_nxp_vglite_buffer_copy(lv_color_t * dest_buf, const lv_area_t * dest_area, lv_coord_t dest_stride,
                                       const lv_color_t * src_buf, const lv_area_t * src_area, lv_coord_t src_stride)
{
    vg_lite_error_t err = VG_LITE_SUCCESS;

    if(check_src_alignment(src_buf, src_stride) != LV_RES_OK)
        VG_LITE_RETURN_INV("Check src alignment failed.");

    vg_lite_buffer_t src_vgbuf;
    /* Set src_vgbuf structure. */
    lv_vglite_set_buf(&src_vgbuf, src_buf, src_area, src_stride);

    vg_lite_buffer_t dest_vgbuf;
    /* Set dest_vgbuf structure. */
    lv_vglite_set_buf(&dest_vgbuf, dest_buf, dest_area, dest_stride);

    uint32_t rect[] = {
        (uint32_t)src_area->x1, /* start x */
        (uint32_t)src_area->y1, /* start y */
        (uint32_t)lv_area_get_width(src_area), /* width */
        (uint32_t)lv_area_get_height(src_area) /* height */
    };

    /* Set scissor. */
    lv_vglite_set_scissor(dest_area);

    /* Set vgmatrix. */
    lv_vglite_set_translation_matrix(dest_area);

    err = vg_lite_blit_rect(&dest_vgbuf, &src_vgbuf, rect, &vgmatrix,
                            VG_LITE_BLEND_NONE, 0xFFFFFFFFU, VG_LITE_FILTER_POINT);
    if(err != VG_LITE_SUCCESS) {
        LV_LOG_ERROR("Blit rectangle failed.");
        /* Disable scissor. */
        lv_vglite_disable_scissor();

        return LV_RES_INV;
    }

    if(lv_vglite_run() != LV_RES_OK) {
        LV_LOG_ERROR("Run failed.");
        /* Disable scissor. */
        lv_vglite_disable_scissor();

        return LV_RES_INV;
    }

    /* Disable scissor. */
    lv_vglite_disable_scissor();

    return LV_RES_OK;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#if VG_LITE_BLIT_SPLIT_ENABLED
static lv_res_t lv_vglite_blit_split(lv_color_t * dest_buf, lv_area_t * dest_area, lv_coord_t dest_stride,
                                     const lv_color_t * src_buf, lv_area_t * src_area, lv_coord_t src_stride,
                                     lv_opa_t opa)
{
    lv_res_t rv = LV_RES_INV;

    VG_LITE_LOG_TRACE("Blit "
                      "Area: ([%d,%d], [%d,%d]) -> ([%d,%d], [%d,%d]) | "
                      "Size: ([%dx%d] -> [%dx%d]) | "
                      "Addr: (0x%x -> 0x%x)",
                      src_area->x1, src_area->y1, src_area->x2, src_area->y2,
                      dest_area->x1, dest_area->y1, dest_area->x2, dest_area->y2,
                      lv_area_get_width(src_area), lv_area_get_height(src_area),
                      lv_area_get_width(dest_area), lv_area_get_height(dest_area),
                      (uintptr_t)src_buf, (uintptr_t)dest_buf);

    /* Stage 1: Move starting pointers as close as possible to [x1, y1], so coordinates are as small as possible. */
    align_x(src_area, (lv_color_t **)&src_buf);
    align_y(src_area, (lv_color_t **)&src_buf, src_stride);
    align_x(dest_area, (lv_color_t **)&dest_buf);
    align_y(dest_area, (lv_color_t **)&dest_buf, dest_stride);

    /* Stage 2: If we're in limit, do a single BLIT */
    if((src_area->x2 < LV_GPU_NXP_VG_LITE_BLIT_SPLIT_THR) &&
       (src_area->y2 < LV_GPU_NXP_VG_LITE_BLIT_SPLIT_THR)) {
        if(check_src_alignment(src_buf, src_stride) != LV_RES_OK)
            VG_LITE_RETURN_INV("Check src alignment failed.");

        /* Set new dest_vgbuf and src_vgbuf memory addresses. */
        lv_vglite_set_dest_buf_ptr(dest_buf);
        lv_vglite_set_src_buf_ptr(src_buf);

        /* Set scissor */
        lv_vglite_set_scissor(dest_area);

        /* Set vgmatrix. */
        lv_vglite_set_translation_matrix(dest_area);

        /* Start blit. */
        rv = lv_vglite_blit(src_area, opa);

        /* Disable scissor. */
        lv_vglite_disable_scissor();

        VG_LITE_LOG_TRACE("Single "
                          "Area: ([%d,%d], [%d,%d]) -> ([%d,%d], [%d,%d]) | "
                          "Size: ([%dx%d] -> [%dx%d]) | "
                          "Addr: (0x%x -> 0x%x) %s",
                          src_area->x1, src_area->y1, src_area->x2, src_area->y2,
                          dest_area->x1, dest_area->y1, dest_area->x2, dest_area->y2,
                          lv_area_get_width(src_area), lv_area_get_height(src_area),
                          lv_area_get_width(dest_area), lv_area_get_height(dest_area),
                          (uintptr_t)src_buf, (uintptr_t)dest_buf,
                          rv == LV_RES_OK ? "OK!" : "FAILED!");

        return rv;
    };

    /* Stage 3: Split the BLIT into multiple tiles */
    VG_LITE_LOG_TRACE("Split "
                      "Area: ([%d,%d], [%d,%d]) -> ([%d,%d], [%d,%d]) | "
                      "Size: ([%dx%d] -> [%dx%d]) | "
                      "Addr: (0x%x -> 0x%x)",
                      src_area->x1, src_area->y1, src_area->x2, src_area->y2,
                      dest_area->x1, dest_area->y1, dest_area->x2, dest_area->y2,
                      lv_area_get_width(src_area), lv_area_get_height(src_area),
                      lv_area_get_width(dest_area), lv_area_get_height(dest_area),
                      (uintptr_t)src_buf, (uintptr_t)dest_buf);


    lv_coord_t width = lv_area_get_width(src_area);
    lv_coord_t height = lv_area_get_height(src_area);

    /* Number of tiles needed */
    int total_tiles_x = (src_area->x1 + width + LV_GPU_NXP_VG_LITE_BLIT_SPLIT_THR - 1) /
                        LV_GPU_NXP_VG_LITE_BLIT_SPLIT_THR;
    int total_tiles_y = (src_area->y1 + height + LV_GPU_NXP_VG_LITE_BLIT_SPLIT_THR - 1) /
                        LV_GPU_NXP_VG_LITE_BLIT_SPLIT_THR;

    /* src and dst buffer shift against each other. Src buffer real data [0,0] may start actually at [3,0] in buffer, as
     * the buffer pointer has to be aligned, while dst buffer real data [0,0] may start at [1,0] in buffer. alignment may be
     * different */
    int shift_src_x = (src_area->x1 > dest_area->x1) ? (src_area->x1 - dest_area->x1) : 0;
    int shift_dest_x = (src_area->x1 < dest_area->x1) ? (dest_area->x1 - src_area->x1) : 0;

    VG_LITE_LOG_TRACE("X shift: src: %d, dst: %d", shift_src_x, shift_dest_x);

    lv_color_t * tile_dest_buf;
    lv_area_t tile_dest_area;
    const lv_color_t * tile_src_buf;
    lv_area_t tile_src_area;

    for(int y = 0; y < total_tiles_y; y++) {

        tile_src_area.y1 = 0; /* no vertical alignment, always start from 0 */
        tile_src_area.y2 = height - y * LV_GPU_NXP_VG_LITE_BLIT_SPLIT_THR - 1;
        if(tile_src_area.y2 >= LV_GPU_NXP_VG_LITE_BLIT_SPLIT_THR) {
            tile_src_area.y2 = LV_GPU_NXP_VG_LITE_BLIT_SPLIT_THR - 1; /* Should never happen */
        }
        tile_src_buf = src_buf + y * LV_GPU_NXP_VG_LITE_BLIT_SPLIT_THR * src_stride;

        tile_dest_area.y1 = tile_src_area.y1; /* y has no alignment, always in sync with src */
        tile_dest_area.y2 = tile_src_area.y2;

        tile_dest_buf = dest_buf + y * LV_GPU_NXP_VG_LITE_BLIT_SPLIT_THR * dest_stride;

        for(int x = 0; x < total_tiles_x; x++) {

            if(x == 0) {
                /* 1st tile is special - there may be a gap between buffer start pointer
                 * and area.x1 value, as the pointer has to be aligned.
                 * tile_src_buf pointer - keep init value from Y-loop.
                 * Also, 1st tile start is not shifted! shift is applied from 2nd tile */
                tile_src_area.x1 = src_area->x1;
                tile_dest_area.x1 = dest_area->x1;
            }
            else {
                /* subsequent tiles always starts from 0, but shifted*/
                tile_src_area.x1 = 0 + shift_src_x;
                tile_dest_area.x1 = 0 + shift_dest_x;
                /* and advance start pointer + 1 tile size */
                tile_src_buf += LV_GPU_NXP_VG_LITE_BLIT_SPLIT_THR;
                tile_dest_buf += LV_GPU_NXP_VG_LITE_BLIT_SPLIT_THR;
            }

            /* Clip tile end coordinates */
            tile_src_area.x2 = width + src_area->x1 - x * LV_GPU_NXP_VG_LITE_BLIT_SPLIT_THR - 1;
            if(tile_src_area.x2 >= LV_GPU_NXP_VG_LITE_BLIT_SPLIT_THR) {
                tile_src_area.x2 = LV_GPU_NXP_VG_LITE_BLIT_SPLIT_THR - 1;
            }

            tile_dest_area.x2 = width + dest_area->x1 - x * LV_GPU_NXP_VG_LITE_BLIT_SPLIT_THR - 1;
            if(tile_dest_area.x2 >= LV_GPU_NXP_VG_LITE_BLIT_SPLIT_THR) {
                tile_dest_area.x2 = LV_GPU_NXP_VG_LITE_BLIT_SPLIT_THR - 1;
            }

            if(x < (total_tiles_x - 1)) {
                /* And adjust end coords if shifted, but not for last tile! */
                tile_src_area.x2 += shift_src_x;
                tile_dest_area.x2 += shift_dest_x;
            }

            if(check_src_alignment(tile_src_buf, src_stride) != LV_RES_OK)
                VG_LITE_RETURN_INV("Check src alignment failed.");

            /* Set new dest_vgbuf and src_vgbuf memory addresses. */
            lv_vglite_set_dest_buf_ptr(tile_dest_buf);
            lv_vglite_set_src_buf_ptr(tile_src_buf);

            /* Set scissor */
            lv_vglite_set_scissor(&tile_dest_area);

            /* Set vgmatrix. */
            lv_vglite_set_translation_matrix(&tile_dest_area);

            /* Start blit. */
            rv = lv_vglite_blit(&tile_src_area, opa);

            /* Disable scissor. */
            lv_vglite_disable_scissor();

            VG_LITE_LOG_TRACE("Tile [%d, %d] "
                              "Area: ([%d,%d], [%d,%d]) -> ([%d,%d], [%d,%d]) | "
                              "Size: ([%dx%d] -> [%dx%d]) | "
                              "Addr: (0x%x -> 0x%x) %s",
                              x, y,
                              tile_src_area.x1, tile_src_area.y1, tile_src_area.x2, tile_src_area.y2,
                              tile_dest_area.x1, tile_dest_area.y1, tile_dest_area.x2, tile_dest_area.y2,
                              lv_area_get_width(&tile_src_area), lv_area_get_height(&tile_src_area),
                              lv_area_get_width(&tile_dest_area), lv_area_get_height(&tile_dest_area),
                              (uintptr_t)tile_src_buf, (uintptr_t)tile_dest_buf,
                              rv == LV_RES_OK ? "OK!" : "FAILED!");

            if(rv != LV_RES_OK) {
                return rv;
            }
        }
    }

    return rv;
}
#endif /*VG_LITE_BLIT_SPLIT_ENABLED*/

static lv_res_t lv_vglite_blit(const lv_area_t * src_area, lv_opa_t opa)
{
    vg_lite_error_t err = VG_LITE_SUCCESS;
    vg_lite_buffer_t * dst_vgbuf = lv_vglite_get_dest_buf();
    vg_lite_buffer_t * src_vgbuf = lv_vglite_get_src_buf();

    uint32_t rect[] = {
        (uint32_t)src_area->x1, /* start x */
        (uint32_t)src_area->y1, /* start y */
        (uint32_t)lv_area_get_width(src_area), /* width */
        (uint32_t)lv_area_get_height(src_area) /* height */
    };

    uint32_t color;
    vg_lite_blend_t blend;
    if(opa >= (lv_opa_t)LV_OPA_MAX) {
        color = 0xFFFFFFFFU;
        blend = VG_LITE_BLEND_SRC_OVER;
        src_vgbuf->transparency_mode = VG_LITE_IMAGE_TRANSPARENT;
    }
    else {
        if(vg_lite_query_feature(gcFEATURE_BIT_VG_PE_PREMULTIPLY)) {
            color = (opa << 24) | 0x00FFFFFFU;
        }
        else {
            color = (opa << 24) | (opa << 16) | (opa << 8) | opa;
        }
        blend = VG_LITE_BLEND_SRC_OVER;
        src_vgbuf->image_mode = VG_LITE_MULTIPLY_IMAGE_MODE;
        src_vgbuf->transparency_mode = VG_LITE_IMAGE_TRANSPARENT;
    }

    err = vg_lite_blit_rect(dst_vgbuf, src_vgbuf, rect, &vgmatrix, blend, color, VG_LITE_FILTER_POINT);
    VG_LITE_ERR_RETURN_INV(err, "Blit rectangle failed.");

    if(lv_vglite_run() != LV_RES_OK)
        VG_LITE_RETURN_INV("Run failed.");

    return LV_RES_OK;
}

static lv_res_t check_src_alignment(const lv_color_t * src_buf, lv_coord_t src_stride)
{
    /* No alignment requirement for destination pixel buffer when using mode VG_LITE_LINEAR */

    /* Test for pointer alignment */
    if((((uintptr_t)src_buf) % (uintptr_t)LV_ATTRIBUTE_MEM_ALIGN_SIZE) != (uintptr_t)0x0U)
        VG_LITE_RETURN_INV("Src buffer ptr (0x%x) not aligned to 0x%x bytes.",
                           (size_t)src_buf, LV_ATTRIBUTE_MEM_ALIGN_SIZE);

    /* Test for stride alignment */
    if((src_stride % (lv_coord_t)LV_GPU_NXP_VG_LITE_STRIDE_ALIGN_PX) != 0x0U)
        VG_LITE_RETURN_INV("Src buffer stride (%d px) not aligned to %d px.",
                           src_stride, LV_GPU_NXP_VG_LITE_STRIDE_ALIGN_PX);
    return LV_RES_OK;
}

static inline void lv_vglite_set_translation_matrix(const lv_area_t * dest_area)
{
    vg_lite_identity(&vgmatrix);
    vg_lite_translate((vg_lite_float_t)dest_area->x1, (vg_lite_float_t)dest_area->y1, &vgmatrix);
}

static inline void lv_vglite_set_transformation_matrix(const lv_area_t * dest_area, const lv_draw_img_dsc_t * dsc)
{
    lv_vglite_set_translation_matrix(dest_area);

    bool has_scale = (dsc->zoom != LV_IMG_ZOOM_NONE);
    bool has_rotation = (dsc->angle != 0);

    vg_lite_translate(dsc->pivot.x, dsc->pivot.y, &vgmatrix);
    if(has_rotation)
        vg_lite_rotate(dsc->angle / 10.0f, &vgmatrix);   /* angle is 1/10 degree */
    if(has_scale) {
        vg_lite_float_t scale = 1.0f * dsc->zoom / LV_IMG_ZOOM_NONE;
        vg_lite_scale(scale, scale, &vgmatrix);
    }
    vg_lite_translate(0.0f - dsc->pivot.x, 0.0f - dsc->pivot.y, &vgmatrix);
}

#if VG_LITE_BLIT_SPLIT_ENABLED
static void align_x(lv_area_t * area, lv_color_t ** buf)
{
    int alignedAreaStartPx = area->x1 - (area->x1 % (LV_ATTRIBUTE_MEM_ALIGN_SIZE / sizeof(lv_color_t)));
    VG_LITE_COND_STOP(alignedAreaStartPx < 0, "Negative X alignment.");

    area->x1 -= alignedAreaStartPx;
    area->x2 -= alignedAreaStartPx;
    *buf += alignedAreaStartPx;
}

static void align_y(lv_area_t * area, lv_color_t ** buf, lv_coord_t stride)
{
    int LineToAlignMem;
    int alignedAreaStartPy;
    /* find how many lines of pixels will respect memory alignment requirement */
    if((stride % (lv_coord_t)LV_ATTRIBUTE_MEM_ALIGN_SIZE) == 0x0U) {
        alignedAreaStartPy = area->y1;
    }
    else {
        LineToAlignMem = LV_ATTRIBUTE_MEM_ALIGN_SIZE / (LV_GPU_NXP_VG_LITE_STRIDE_ALIGN_PX * sizeof(lv_color_t));
        VG_LITE_COND_STOP(LV_ATTRIBUTE_MEM_ALIGN_SIZE % (LV_GPU_NXP_VG_LITE_STRIDE_ALIGN_PX * sizeof(lv_color_t)),
                          "Complex case: need gcd function.");
        alignedAreaStartPy = area->y1 - (area->y1 % LineToAlignMem);
        VG_LITE_COND_STOP(alignedAreaStartPy < 0, "Negative Y alignment.");
    }

    area->y1 -= alignedAreaStartPy;
    area->y2 -= alignedAreaStartPy;
    *buf += (uint32_t)(alignedAreaStartPy * stride);
}
#endif /*VG_LITE_BLIT_SPLIT_ENABLED*/

#endif /*LV_USE_GPU_NXP_VG_LITE*/
