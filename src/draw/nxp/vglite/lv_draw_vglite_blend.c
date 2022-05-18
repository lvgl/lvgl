/**
 * @file lv_draw_vglite_blend.c
 *
 */

/**
 * MIT License
 *
 * Copyright 2020-2022 NXP
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

/*********************
 *      DEFINES
 *********************/

/* Enable BLIT quality degradation workaround for RT595, recommended for screen's dimension > 352 pixels */
#define RT595_BLIT_WRKRND_ENABLED 1

/* Internal compound symbol */
#if (defined(CPU_MIMXRT595SFFOB) || defined(CPU_MIMXRT595SFFOB_cm33) || \
    defined(CPU_MIMXRT595SFFOC) || defined(CPU_MIMXRT595SFFOC_cm33)) && \
    RT595_BLIT_WRKRND_ENABLED
#define VG_LITE_BLIT_SPLIT_ENABLED 1
#else
#define VG_LITE_BLIT_SPLIT_ENABLED 0
#endif

/**
 * BLIT split threshold - BLITs with width or height higher than this value will be done
 * in multiple steps. Value must be 16-aligned. Don't change.
 */
#define LV_GPU_NXP_VG_LITE_BLIT_SPLIT_THR 352


/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**
 * BLock Image Transfer - single direct BLIT.
 *
 * @param[in] blit Description of the transfer
 * @retval LV_RES_OK Transfer complete
 * @retval LV_RES_INV Error occurred (\see LV_GPU_NXP_VG_LITE_LOG_ERRORS)
 */
static lv_res_t _lv_gpu_nxp_vglite_blit_single(lv_gpu_nxp_vglite_blit_info_t * blit);

#if VG_LITE_BLIT_SPLIT_ENABLED

    /**
    * Move buffer pointer as close as possible to area, but with respect to alignment requirements. X-axis only.
    *
    * @param[in,out] area Area to be updated
    * @param[in,out] buf Pointer to be updated
    */
    static void _align_x(lv_area_t * area, lv_color_t ** buf);

    /**
    * Move buffer pointer to the area start and update variables, Y-axis only.
    *
    * @param[in,out] area Area to be updated
    * @param[in,out] buf Pointer to be updated
    * @param[in] stridePx Buffer stride in pixels
    */
    static void _align_y(lv_area_t * area, lv_color_t ** buf, uint32_t stridePx);

    /**
    * Software BLIT as a fall-back scenario.
    *
    * @param[in] blit BLIT configuration
    */
    static void _sw_blit(lv_gpu_nxp_vglite_blit_info_t * blit);

    /**
    * Verify BLIT structure - widths, stride, pointer alignment
    *
    * @param[in] blit BLIT configuration
    * @retval LV_RES_OK
    * @retval LV_RES_INV Error occurred (\see LV_GPU_NXP_VG_LITE_LOG_ERRORS)
    */
    static lv_res_t _lv_gpu_nxp_vglite_check_blit(lv_gpu_nxp_vglite_blit_info_t * blit);

    /**
    * BLock Image Transfer - split BLIT.
    *
    * @param[in] blit BLIT configuration
    * @retval LV_RES_OK Transfer complete
    * @retval LV_RES_INV Error occurred (\see LV_GPU_NXP_VG_LITE_LOG_ERRORS)
    */
    static lv_res_t _lv_gpu_nxp_vglite_blit_split(lv_gpu_nxp_vglite_blit_info_t * blit);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_res_t lv_gpu_nxp_vglite_fill(lv_color_t * dest_buf, lv_coord_t dest_width, lv_coord_t dest_height,
                                const lv_area_t * fill_area, lv_color_t color, lv_opa_t opa)
{
    uint32_t area_size = lv_area_get_size(fill_area);
    lv_coord_t area_w = lv_area_get_width(fill_area);
    lv_coord_t area_h = lv_area_get_height(fill_area);

    if(opa >= (lv_opa_t)LV_OPA_MAX) {
        if(area_size < LV_GPU_NXP_VG_LITE_FILL_SIZE_LIMIT)
            VG_LITE_RETURN_INV("Area size %d smaller than limit %d.", area_size, LV_GPU_NXP_VG_LITE_FILL_SIZE_LIMIT);
    }
    else {
        if(area_size < LV_GPU_NXP_VG_LITE_FILL_OPA_SIZE_LIMIT)
            VG_LITE_RETURN_INV("Area size %d smaller than limit %d.", area_size, LV_GPU_NXP_VG_LITE_FILL_OPA_SIZE_LIMIT);
    }

    vg_lite_buffer_t vgbuf;
    vg_lite_rectangle_t rect;
    vg_lite_error_t err = VG_LITE_SUCCESS;
    lv_color32_t col32 = {.full = lv_color_to32(color)}; /*Convert color to RGBA8888*/
    vg_lite_color_t vgcol; /* vglite takes ABGR */

    if(lv_vglite_init_buf(&vgbuf, (uint32_t)dest_width, (uint32_t)dest_height, (uint32_t)dest_width * sizeof(lv_color_t),
                          (const lv_color_t *)dest_buf, false) != LV_RES_OK)
        VG_LITE_RETURN_INV("Init buffer failed.");

    if(opa >= (lv_opa_t)LV_OPA_MAX) {   /*Opaque fill*/
        rect.x = fill_area->x1;
        rect.y = fill_area->y1;
        rect.width = area_w;
        rect.height = area_h;

        /*Clean & invalidate cache*/
        lv_vglite_invalidate_cache();

#if LV_COLOR_DEPTH==16
        vgcol = col32.full;
#else /*LV_COLOR_DEPTH==32*/
        vgcol = ((uint32_t)col32.ch.alpha << 24) | ((uint32_t)col32.ch.blue << 16) | ((uint32_t)col32.ch.green << 8) |
                (uint32_t)col32.ch.red;
#endif

        err = vg_lite_clear(&vgbuf, &rect, vgcol);
        VG_LITE_ERR_RETURN_INV(err, "Clear failed.");

        err = vg_lite_finish();
        VG_LITE_ERR_RETURN_INV(err, "Finish failed.");
    }
    else {   /*fill with transparency*/

        vg_lite_path_t path;
        int32_t path_data[] = { /*VG rectangular path*/
            VLC_OP_MOVE, fill_area->x1,  fill_area->y1,
            VLC_OP_LINE, fill_area->x2 + 1,  fill_area->y1,
            VLC_OP_LINE, fill_area->x2 + 1,  fill_area->y2 + 1,
            VLC_OP_LINE, fill_area->x1,  fill_area->y2 + 1,
            VLC_OP_LINE, fill_area->x1,  fill_area->y1,
            VLC_OP_END
        };

        err = vg_lite_init_path(&path, VG_LITE_S32, VG_LITE_LOW, sizeof(path_data), path_data,
                                (vg_lite_float_t) fill_area->x1, (vg_lite_float_t) fill_area->y1,
                                ((vg_lite_float_t) fill_area->x2) + 1.0f, ((vg_lite_float_t) fill_area->y2) + 1.0f);
        VG_LITE_ERR_RETURN_INV(err, "Init path failed.");

        /* Only pre-multiply color if hardware pre-multiplication is not present */
        if(!vg_lite_query_feature(gcFEATURE_BIT_VG_PE_PREMULTIPLY)) {
            col32.ch.red = (uint8_t)(((uint16_t)col32.ch.red * opa) >> 8);
            col32.ch.green = (uint8_t)(((uint16_t)col32.ch.green * opa) >> 8);
            col32.ch.blue = (uint8_t)(((uint16_t)col32.ch.blue * opa) >> 8);
        }
        col32.ch.alpha = opa;

#if LV_COLOR_DEPTH==16
        vgcol = col32.full;
#else /*LV_COLOR_DEPTH==32*/
        vgcol = ((uint32_t)col32.ch.alpha << 24) | ((uint32_t)col32.ch.blue << 16) | ((uint32_t)col32.ch.green << 8) |
                (uint32_t)col32.ch.red;
#endif

        /*Clean & invalidate cache*/
        lv_vglite_invalidate_cache();

        vg_lite_matrix_t matrix;
        vg_lite_identity(&matrix);

        /*Draw rectangle*/
        err = vg_lite_draw(&vgbuf, &path, VG_LITE_FILL_EVEN_ODD, &matrix, VG_LITE_BLEND_SRC_OVER, vgcol);
        VG_LITE_ERR_RETURN_INV(err, "Draw rectangle failed.");

        err = vg_lite_finish();
        VG_LITE_ERR_RETURN_INV(err, "Finish failed.");

        err = vg_lite_clear_path(&path);
        VG_LITE_ERR_RETURN_INV(err, "Clear path failed.");
    }

    return LV_RES_OK;
}

lv_res_t lv_gpu_nxp_vglite_blit(lv_gpu_nxp_vglite_blit_info_t * blit)
{
    uint32_t dest_size = lv_area_get_size(&blit->dst_area);

    if(blit->opa >= (lv_opa_t)LV_OPA_MAX) {
        if(dest_size < LV_GPU_NXP_VG_LITE_BLIT_SIZE_LIMIT)
            VG_LITE_RETURN_INV("Area size %d smaller than limit %d.", dest_size, LV_GPU_NXP_VG_LITE_BLIT_SIZE_LIMIT);
    }
    else {
        if(dest_size < LV_GPU_NXP_VG_LITE_BLIT_OPA_SIZE_LIMIT)
            VG_LITE_RETURN_INV("Area size %d smaller than limit %d.", dest_size, LV_GPU_NXP_VG_LITE_BLIT_OPA_SIZE_LIMIT);
    }

#if VG_LITE_BLIT_SPLIT_ENABLED
    return _lv_gpu_nxp_vglite_blit_split(blit);
#endif /* non RT595 */

    /* Just pass down */
    return _lv_gpu_nxp_vglite_blit_single(blit);
}

lv_res_t lv_gpu_nxp_vglite_blit_transform(lv_gpu_nxp_vglite_blit_info_t * blit)
{
    uint32_t dest_size = lv_area_get_size(&blit->dst_area);

    if(blit->opa >= (lv_opa_t)LV_OPA_MAX) {
        if(dest_size < LV_GPU_NXP_VG_LITE_BLIT_SIZE_LIMIT)
            VG_LITE_RETURN_INV("Area size %d smaller than limit %d.", dest_size, LV_GPU_NXP_VG_LITE_BLIT_SIZE_LIMIT);
    }
    else {
        if(dest_size < LV_GPU_NXP_VG_LITE_BLIT_OPA_SIZE_LIMIT)
            VG_LITE_RETURN_INV("Area size %d smaller than limit %d.", dest_size, LV_GPU_NXP_VG_LITE_BLIT_OPA_SIZE_LIMIT);
    }

    return _lv_gpu_nxp_vglite_blit_single(blit);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#if VG_LITE_BLIT_SPLIT_ENABLED
static lv_res_t _lv_gpu_nxp_vglite_blit_split(lv_gpu_nxp_vglite_blit_info_t * blit)
{
    lv_res_t rv = LV_RES_INV;

    if(_lv_gpu_nxp_vglite_check_blit(blit) != LV_RES_OK) {
        PRINT_BLT("Blit check failed\n");
        return LV_RES_INV;
    }

    PRINT_BLT("BLIT from: "
              "Area: %03d,%03d - %03d,%03d "
              "Addr: %d\n\n",
              blit->src_area.x1, blit->src_area.y1,
              blit->src_area.x2, blit->src_area.y2,
              (uintptr_t) blit->src);

    PRINT_BLT("BLIT to: "
              "Area: %03d,%03d - %03d,%03d "
              "Addr: %d\n\n",
              blit->dst_area.x1, blit->dst_area.y1,
              blit->dst_area.x2, blit->dst_area.y2,
              (uintptr_t) blit->src);

    /* Stage 1: Move starting pointers as close as possible to [x1, y1], so coordinates are as small as possible.  */
    _align_x(&blit->src_area, (lv_color_t **)&blit->src);
    _align_y(&blit->src_area, (lv_color_t **)&blit->src,  blit->src_stride / sizeof(lv_color_t));
    _align_x(&blit->dst_area, (lv_color_t **)&blit->dst);
    _align_y(&blit->dst_area, (lv_color_t **)&blit->dst,  blit->dst_stride / sizeof(lv_color_t));

    /* Stage 2: If we're in limit, do a single BLIT */
    if((blit->src_area.x2 < LV_GPU_NXP_VG_LITE_BLIT_SPLIT_THR) &&
       (blit->src_area.y2 < LV_GPU_NXP_VG_LITE_BLIT_SPLIT_THR)) {
        PRINT_BLT("Simple blit!\n");
        return _lv_gpu_nxp_vglite_blit_single(blit);
    };

    /* Stage 3: Split the BLIT into multiple tiles */
    PRINT_BLT("Split blit!\n");

    PRINT_BLT("Blit "
              "([%03d,%03d], [%03d,%03d]) -> "
              "([%03d,%03d], [%03d,%03d]) | "
              "([%03dx%03d] -> [%03dx%03d]) | "
              "A:(%d -> %d)\n",
              blit->src_area.x1, blit->src_area.y1, blit->src_area.x2, blit->src_area.y2,
              blit->dst_area.x1, blit->dst_area.y1, blit->dst_area.x2, blit->dst_area.y2,
              lv_area_get_width(&blit->src_area), lv_area_get_height(&blit->src_area),
              lv_area_get_width(&blit->dst_area), lv_area_get_height(&blit->dst_area),
              (uintptr_t) blit->src, (uintptr_t) blit->dst);


    lv_coord_t totalWidth = lv_area_get_width(&blit->src_area);
    lv_coord_t totalHeight = lv_area_get_height(&blit->src_area);

    lv_gpu_nxp_vglite_blit_info_t tileBlit;

    /* Number of tiles needed */
    int totalTilesX = (blit->src_area.x1 + totalWidth + LV_GPU_NXP_VG_LITE_BLIT_SPLIT_THR - 1) /
                      LV_GPU_NXP_VG_LITE_BLIT_SPLIT_THR;
    int totalTilesY = (blit->src_area.y1 + totalHeight + LV_GPU_NXP_VG_LITE_BLIT_SPLIT_THR - 1) /
                      LV_GPU_NXP_VG_LITE_BLIT_SPLIT_THR;

    /* src and dst buffer shift against each other. Src buffer real data [0,0] may start actually at [3,0] in buffer, as
     * the buffer pointer has to be aligned, while dst buffer real data [0,0] may start at [1,0] in buffer. alignment may be
     * different */
    int shiftSrcX = (blit->src_area.x1 > blit->dst_area.x1) ? (blit->src_area.x1 - blit->dst_area.x1) : 0;
    int shiftDstX = (blit->src_area.x1 < blit->dst_area.x1) ? (blit->dst_area.x1 - blit->src_area.x1) : 0;

    PRINT_BLT("\n");
    PRINT_BLT("Align shift: src: %d, dst: %d\n", shiftSrcX, shiftDstX);

    tileBlit = *blit;

    for(int tileY = 0; tileY < totalTilesY; tileY++) {

        tileBlit.src_area.y1 = 0; /* no vertical alignment, always start from 0 */
        tileBlit.src_area.y2 = totalHeight - tileY * LV_GPU_NXP_VG_LITE_BLIT_SPLIT_THR - 1;
        if(tileBlit.src_area.y2 >= LV_GPU_NXP_VG_LITE_BLIT_SPLIT_THR) {
            tileBlit.src_area.y2 = LV_GPU_NXP_VG_LITE_BLIT_SPLIT_THR - 1; /* Should never happen */
        }
        tileBlit.src = blit->src + tileY * LV_GPU_NXP_VG_LITE_BLIT_SPLIT_THR * blit->src_stride / sizeof(
                           lv_color_t); /* stride in px! */

        tileBlit.dst_area.y1 = tileBlit.src_area.y1; /* y has no alignment, always in sync with src */
        tileBlit.dst_area.y2 = tileBlit.src_area.y2;

        tileBlit.dst = blit->dst + tileY * LV_GPU_NXP_VG_LITE_BLIT_SPLIT_THR * blit->dst_stride / sizeof(
                           lv_color_t); /* stride in px! */

        for(int tileX = 0; tileX < totalTilesX; tileX++) {

            if(tileX == 0) {
                /* 1st tile is special - there may be a gap between buffer start pointer
                 * and area.x1 value, as the pointer has to be aligned.
                 * tileBlit.src pointer - keep init value from Y-loop.
                 * Also, 1st tile start is not shifted! shift is applied from 2nd tile */
                tileBlit.src_area.x1 = blit->src_area.x1;
                tileBlit.dst_area.x1 = blit->dst_area.x1;
            }
            else {
                /* subsequent tiles always starts from 0, but shifted*/
                tileBlit.src_area.x1 = 0 + shiftSrcX;
                tileBlit.dst_area.x1 = 0 + shiftDstX;
                /* and advance start pointer + 1 tile size */
                tileBlit.src += LV_GPU_NXP_VG_LITE_BLIT_SPLIT_THR;
                tileBlit.dst += LV_GPU_NXP_VG_LITE_BLIT_SPLIT_THR;
            }

            /* Clip tile end coordinates */
            tileBlit.src_area.x2 = totalWidth + blit->src_area.x1 - tileX * LV_GPU_NXP_VG_LITE_BLIT_SPLIT_THR - 1;
            if(tileBlit.src_area.x2 >= LV_GPU_NXP_VG_LITE_BLIT_SPLIT_THR) {
                tileBlit.src_area.x2 = LV_GPU_NXP_VG_LITE_BLIT_SPLIT_THR - 1;
            }

            tileBlit.dst_area.x2 = totalWidth + blit->dst_area.x1 - tileX * LV_GPU_NXP_VG_LITE_BLIT_SPLIT_THR - 1;
            if(tileBlit.dst_area.x2 >= LV_GPU_NXP_VG_LITE_BLIT_SPLIT_THR) {
                tileBlit.dst_area.x2 = LV_GPU_NXP_VG_LITE_BLIT_SPLIT_THR - 1;
            }

            if(tileX < (totalTilesX - 1)) {
                /* And adjust end coords if shifted,  but not for last tile! */
                tileBlit.src_area.x2 += shiftSrcX;
                tileBlit.dst_area.x2 += shiftDstX;
            }

            rv = _lv_gpu_nxp_vglite_blit_single(&tileBlit);

#if BLIT_DBG_AREAS
            lv_vglite_dbg_draw_rectangle((lv_color_t *) tileBlit.dst, tileBlit.dst_width, tileBlit.dst_height, &tileBlit.dst_area,
                                         LV_COLOR_RED);
            lv_vglite_dbg_draw_rectangle((lv_color_t *) tileBlit.src, tileBlit.src_width, tileBlit.src_height, &tileBlit.src_area,
                                         LV_COLOR_GREEN);
#endif

            PRINT_BLT("Tile [%d, %d]: "
                      "([%d,%d], [%d,%d]) -> "
                      "([%d,%d], [%d,%d]) | "
                      "([%dx%d] -> [%dx%d]) | "
                      "A:(0x%8X -> 0x%8X) %s\n",
                      tileX, tileY,
                      tileBlit.src_area.x1, tileBlit.src_area.y1, tileBlit.src_area.x2, tileBlit.src_area.y2,
                      tileBlit.dst_area.x1, tileBlit.dst_area.y1, tileBlit.dst_area.x2, tileBlit.dst_area.y2,
                      lv_area_get_width(&tileBlit.src_area), lv_area_get_height(&tileBlit.src_area),
                      lv_area_get_width(&tileBlit.dst_area), lv_area_get_height(&tileBlit.dst_area),
                      (uintptr_t) tileBlit.src, (uintptr_t) tileBlit.dst,
                      rv == LV_RES_OK ? "OK!" : "!!! FAILED !!!");

            if(rv != LV_RES_OK) { /* if anything goes wrong... */
#if LV_GPU_NXP_VG_LITE_LOG_ERRORS
                LV_LOG_ERROR("Split blit failed. Trying SW blit instead.");
#endif
                _sw_blit(&tileBlit);
                rv = LV_RES_OK; /* Don't report error, as SW BLIT was performed */
            }

        }
        PRINT_BLT(" \n");
    }

    return rv; /* should never fail */
}
#endif /* VG_LITE_BLIT_SPLIT_ENABLED */

static lv_res_t _lv_gpu_nxp_vglite_blit_single(lv_gpu_nxp_vglite_blit_info_t * blit)
{
    vg_lite_buffer_t src_vgbuf, dst_vgbuf;
    vg_lite_error_t err = VG_LITE_SUCCESS;
    uint32_t rect[4];
    vg_lite_float_t scale = 1.0;

    if(blit == NULL) {
        /*Wrong parameter*/
        return LV_RES_INV;
    }

    if(blit->opa < (lv_opa_t) LV_OPA_MIN) {
        return LV_RES_OK; /*Nothing to BLIT*/
    }

    /*Wrap src/dst buffer into VG-Lite buffer*/
    if(lv_vglite_init_buf(&src_vgbuf, (uint32_t)blit->src_width, (uint32_t)blit->src_height, (uint32_t)blit->src_stride,
                          blit->src, true) != LV_RES_OK)
        VG_LITE_RETURN_INV("Init buffer failed.");

    if(lv_vglite_init_buf(&dst_vgbuf, (uint32_t)blit->dst_width, (uint32_t)blit->dst_height, (uint32_t)blit->dst_stride,
                          blit->dst, false) != LV_RES_OK)
        VG_LITE_RETURN_INV("Init buffer failed.");

    rect[0] = (uint32_t)blit->src_area.x1; /* start x */
    rect[1] = (uint32_t)blit->src_area.y1; /* start y */
    rect[2] = (uint32_t)blit->src_area.x2 - (uint32_t)blit->src_area.x1 + 1U; /* width */
    rect[3] = (uint32_t)blit->src_area.y2 - (uint32_t)blit->src_area.y1 + 1U; /* height */

    vg_lite_matrix_t matrix;
    vg_lite_identity(&matrix);
    vg_lite_translate((vg_lite_float_t)blit->dst_area.x1, (vg_lite_float_t)blit->dst_area.y1, &matrix);

    if((blit->angle != 0) || (blit->zoom != LV_IMG_ZOOM_NONE)) {
        vg_lite_translate(blit->pivot.x, blit->pivot.y, &matrix);
        vg_lite_rotate(blit->angle / 10.0f, &matrix);   /* angle is 1/10 degree */
        scale = 1.0f * blit->zoom / LV_IMG_ZOOM_NONE;
        vg_lite_scale(scale, scale, &matrix);
        vg_lite_translate(0.0f - blit->pivot.x, 0.0f - blit->pivot.y, &matrix);
    }

    /*Clean & invalidate cache*/
    lv_vglite_invalidate_cache();

    uint32_t color;
    vg_lite_blend_t blend;
    if(blit->opa >= (lv_opa_t)LV_OPA_MAX) {
        color = 0xFFFFFFFFU;
        blend = VG_LITE_BLEND_SRC_OVER;
        src_vgbuf.transparency_mode = VG_LITE_IMAGE_TRANSPARENT;
    }
    else {
        uint32_t opa = (uint32_t)blit->opa;
        if(vg_lite_query_feature(gcFEATURE_BIT_VG_PE_PREMULTIPLY)) {
            color = (opa << 24) | 0x00FFFFFFU;
        }
        else {
            color = (opa << 24) | (opa << 16) | (opa << 8) | opa;
        }
        blend = VG_LITE_BLEND_SRC_OVER;
        src_vgbuf.image_mode = VG_LITE_MULTIPLY_IMAGE_MODE;
        src_vgbuf.transparency_mode = VG_LITE_IMAGE_TRANSPARENT;
    }

    err = vg_lite_blit_rect(&dst_vgbuf, &src_vgbuf, rect, &matrix, blend, color, VG_LITE_FILTER_POINT);
    VG_LITE_ERR_RETURN_INV(err, "Blit rectangle failed.");

    err = vg_lite_finish();
    VG_LITE_ERR_RETURN_INV(err, "Finish failed.");

    return LV_RES_OK;
}

#if VG_LITE_BLIT_SPLIT_ENABLED

static void _sw_blit(lv_gpu_nxp_vglite_blit_info_t * blit)
{
    int x, y;

    lv_coord_t w = lv_area_get_width(&blit->src_area);
    lv_coord_t h = lv_area_get_height(&blit->src_area);

    int32_t srcStridePx = blit->src_stride / (int32_t)sizeof(lv_color_t);
    int32_t dstStridePx = blit->dst_stride / (int32_t)sizeof(lv_color_t);

    lv_color_t * src = (lv_color_t *)blit->src + blit->src_area.y1 * srcStridePx + blit->src_area.x1;
    lv_color_t * dst = (lv_color_t *)blit->dst + blit->dst_area.y1 * dstStridePx + blit->dst_area.x1;

    if(blit->opa >= (lv_opa_t)LV_OPA_MAX) {
        /* simple copy */
        for(y = 0; y < h; y++) {
            lv_memcpy(dst, src, (uint32_t)w * sizeof(lv_color_t));
            src += srcStridePx;
            dst += dstStridePx;
        }
    }
    else if(blit->opa >= LV_OPA_MIN) {
        /* alpha blending */
        for(y = 0; y < h; y++) {
            for(x = 0; x < w; x++) {
                dst[x] = lv_color_mix(src[x], dst[x], blit->opa);
            }
            src += srcStridePx;
            dst += dstStridePx;
        }
    }
}

static lv_res_t _lv_gpu_nxp_vglite_check_blit(lv_gpu_nxp_vglite_blit_info_t * blit)
{

    /* Test for minimal width */
    if(lv_area_get_width(&blit->src_area) < (lv_coord_t)LV_GPU_NXP_VG_LITE_STRIDE_ALIGN_PX)
        VG_LITE_RETURN_INV("Src area width (%d) is smaller than required (%d).", lv_area_get_width(&blit->src_area),
                           LV_GPU_NXP_VG_LITE_STRIDE_ALIGN_PX);

    /* Test for minimal width */
    if(lv_area_get_width(&blit->dst_area) < (lv_coord_t)LV_GPU_NXP_VG_LITE_STRIDE_ALIGN_PX)
        VG_LITE_RETURN_INV("Dest area width (%d) is smaller than required (%d).", lv_area_get_width(&blit->dst_area),
                           LV_GPU_NXP_VG_LITE_STRIDE_ALIGN_PX);

    /* Test for pointer alignment */
    if((((uintptr_t) blit->src) % LV_ATTRIBUTE_MEM_ALIGN_SIZE) != 0x0)
        VG_LITE_RETURN_INV("Src buffer ptr (0x%X) not aligned to %d.", (size_t) blit->src, LV_ATTRIBUTE_MEM_ALIGN_SIZE);

    /* No alignment requirement for destination pixel buffer when using mode VG_LITE_LINEAR */

    /* Test for stride alignment */
    if((blit->src_stride % (LV_GPU_NXP_VG_LITE_STRIDE_ALIGN_PX * LV_COLOR_DEPTH / 8)) !=  0x0)
        VG_LITE_RETURN_INV("Src buffer stride (%d px) not aligned to %d px.", blit->src_stride,
                           LV_GPU_NXP_VG_LITE_STRIDE_ALIGN_PX);

    /* Test for stride alignment */
    if((blit->dst_stride % (LV_GPU_NXP_VG_LITE_STRIDE_ALIGN_PX * LV_COLOR_DEPTH / 8)) !=  0x0)
        VG_LITE_RETURN_INV("Dest buffer stride (%d px) not aligned to %d px.", blit->dst_stride,
                           LV_GPU_NXP_VG_LITE_STRIDE_ALIGN_PX);

    if((lv_area_get_width(&blit->src_area) != lv_area_get_width(&blit->dst_area)) ||
       (lv_area_get_height(&blit->src_area) != lv_area_get_height(&blit->dst_area)))
        VG_LITE_RETURN_INV("Src and dest buffer areas are not equal.");

    return LV_RES_OK;
}

static void _align_x(lv_area_t * area, lv_color_t ** buf)
{

    int alignedAreaStartPx = area->x1 - (area->x1 % (LV_ATTRIBUTE_MEM_ALIGN_SIZE * 8 / LV_COLOR_DEPTH));
    VG_LITE_COND_STOP(alignedAreaStartPx < 0, "Negative X alignment.");

    area->x1 -= alignedAreaStartPx;
    area->x2 -= alignedAreaStartPx;
    *buf += alignedAreaStartPx;
}

static void _align_y(lv_area_t * area, lv_color_t ** buf, uint32_t stridePx)
{
    int LineToAlignMem;
    int alignedAreaStartPy;
    /* find how many lines of pixels will respect memory alignment requirement */
    if(stridePx % (uint32_t)LV_ATTRIBUTE_MEM_ALIGN_SIZE == 0U) {
        alignedAreaStartPy = area->y1;
    }
    else {
        LineToAlignMem = LV_ATTRIBUTE_MEM_ALIGN_SIZE / (sizeof(lv_color_t) * LV_GPU_NXP_VG_LITE_STRIDE_ALIGN_PX);
        VG_LITE_COND_STOP(LV_ATTRIBUTE_MEM_ALIGN_SIZE % (sizeof(lv_color_t) * LV_GPU_NXP_VG_LITE_STRIDE_ALIGN_PX),
                          "Complex case: need gcd function.");
        alignedAreaStartPy = area->y1 - (area->y1 % LineToAlignMem);
        VG_LITE_COND_STOP(alignedAreaStartPy < 0, "Negative Y alignment.");
    }

    area->y1 -= alignedAreaStartPy;
    area->y2 -= alignedAreaStartPy;
    *buf += (uint32_t)alignedAreaStartPy * stridePx;
}
#endif /*VG_LITE_BLIT_SPLIT_ENABLED*/

#endif /*LV_USE_GPU_NXP_VG_LITE*/
