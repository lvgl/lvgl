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

#include "lv_gpu_nxp_vglite.h"

#if LV_USE_GPU_NXP_VG_LITE

#include "lvgl.h"
#include "../misc/lv_log.h"
#include "fsl_cache.h"
#include "vg_lite.h"
#include "fsl_debug_console.h"

/*********************
 *      DEFINES
 *********************/

#if LV_COLOR_DEPTH==16
    #define VGLITE_PX_FMT VG_LITE_RGB565
#else
    #error Only 16bit color depth is supported. Set LV_COLOR_DEPTH to 16.
#endif

/* Enable BLIT quality degradation workaround for RT595 */
#define RT595_BLIT_WRKRND_ENABLED 1

/* If LV_HOR_RES_MAX/LV_VER_RES_MAX is higher than this value, workaround will be enabled */
#define RT595_BLIT_WRKRND_THR 352

/* Print detailed info to SDK console (NOT to LVGL log system) */
#define BLIT_DBG_VERBOSE 0

/* Draw rectangles around BLIT tiles */
#define BLIT_DBG_AREAS   0

/* Redirect PRINT to SDK PRINTF */
#define PRINT PRINTF

/* Verbose debug print */
#if BLIT_DBG_VERBOSE
    #define PRINT_BLT PRINTF
#else
    #define PRINT_BLT(...)
#endif

/* Internal compound symbol */
#if (defined(CPU_MIMXRT595SFFOB) || defined(CPU_MIMXRT595SFFOB_cm33) || \
    defined(CPU_MIMXRT595SFFOC) || defined(CPU_MIMXRT595SFFOC_cm33)) && \
    ((LV_HOR_RES_MAX > RT595_BLIT_WRKRND_THR) || (LV_VER_RES_MAX > RT595_BLIT_WRKRND_THR)) && \
    RT595_BLIT_WRKRND_ENABLED
#define _BLIT_SPLIT_ENABLED 1
#else
#define _BLIT_SPLIT_ENABLED 0
#endif

/* BLIT split threshold - BLITs with width or height higher than this value will be done
 * in multiple steps. Value must be 16-aligned. Don't change.
 * */
#define LV_GPU_NXP_VG_LITE_BLIT_SPLIT_THR 352


/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static lv_res_t _init_vg_buf(vg_lite_buffer_t * dst, uint32_t width, uint32_t height, uint32_t stride,
                             const lv_color_t * ptr, bool source);

static lv_res_t _lv_gpu_nxp_vglite_blit_single(lv_gpu_nxp_vglite_blit_info_t * blit);
#if _BLIT_SPLIT_ENABLED
static void _align_x(lv_area_t * area, lv_color_t ** buf);
static void _align_y(lv_area_t * area, lv_color_t ** buf, uint32_t stridePx);
static void _sw_blit(lv_gpu_nxp_vglite_blit_info_t * blit);
#if BLIT_DBG_AREAS
static void _draw_rectangle(lv_color_t * dest_buf, lv_coord_t dest_width, lv_coord_t dest_height,
                            lv_area_t * fill_area, lv_color_t color);
#endif
static lv_res_t _lv_gpu_nxp_vglite_check_blit(lv_gpu_nxp_vglite_blit_info_t * blit);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

#define CHECK(cond, txt)                \
    do {                                \
        if (cond) {                     \
            PRINT("%s. STOP!\n", txt);  \
            for ( ; ; );                \
        }                               \
    } while(0)

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
    lv_color32_t col32 = {.full = lv_color_to32(color)}; /*Convert color to RGBA8888*/
    lv_disp_t * disp = _lv_refr_get_disp_refreshing();

    if(_init_vg_buf(&rt, (uint32_t) dest_width, (uint32_t) dest_height, (uint32_t) dest_width * sizeof(lv_color_t),
                    (const lv_color_t *) dest_buf, false) != LV_RES_OK) {
#if LV_GPU_NXP_VG_LITE_LOG_ERRORS
        LV_LOG_ERROR("init_vg_buf reported error. Fill failed.");
#endif
        return LV_RES_INV;
    }

    if(opa >= (lv_opa_t) LV_OPA_MAX) {   /*Opaque fill*/
        rect.x = fill_area->x1;
        rect.y = fill_area->y1;
        rect.width = (int32_t) fill_area->x2 - (int32_t) fill_area->x1 + 1;
        rect.height = (int32_t) fill_area->y2 - (int32_t) fill_area->y1 + 1;

        if(disp != NULL && disp->driver->clean_dcache_cb != NULL) {  /*Clean & invalidate cache*/
            disp->driver->clean_dcache_cb(disp->driver);
        }

        err = vg_lite_clear(&rt, &rect, col32.full);
        if(err != VG_LITE_SUCCESS) {
#if LV_GPU_NXP_VG_LITE_LOG_ERRORS
            LV_LOG_ERROR("vg_lite_clear reported error. Fill failed.");
#endif
            return LV_RES_INV;
        }
        err = vg_lite_finish();
        if(err != VG_LITE_SUCCESS) {
#if LV_GPU_NXP_VG_LITE_LOG_ERRORS
            LV_LOG_ERROR("vg_lite_finish reported error. Fill failed.");
#endif
            return LV_RES_INV;
        }
    }
    else {   /*fill with transparency*/

        vg_lite_path_t path;
        lv_color32_t colMix;
        int16_t path_data[] = { /*VG rectangular path*/
            VLC_OP_MOVE, fill_area->x1,  fill_area->y1,
            VLC_OP_LINE, fill_area->x2 + 1,  fill_area->y1,
            VLC_OP_LINE, fill_area->x2 + 1,  fill_area->y2 + 1,
            VLC_OP_LINE, fill_area->x1,  fill_area->y2 + 1,
            VLC_OP_LINE, fill_area->x1,  fill_area->y1,
            VLC_OP_END
        };

        err = vg_lite_init_path(&path, VG_LITE_S16, VG_LITE_LOW, sizeof(path_data), path_data,
                                (vg_lite_float_t) fill_area->x1, (vg_lite_float_t) fill_area->y1, ((vg_lite_float_t) fill_area->x2) + 1.0f,
                                ((vg_lite_float_t) fill_area->y2) + 1.0f);
        if(err != VG_LITE_SUCCESS)  {
#if LV_GPU_NXP_VG_LITE_LOG_ERRORS
            LV_LOG_ERROR("vg_lite_init_path() failed.");
#endif
            return LV_RES_INV;
        }

        colMix.ch.red = (uint8_t)(((uint16_t)col32.ch.red * opa) >> 8); /*Pre-multiply color*/
        colMix.ch.green = (uint8_t)(((uint16_t)col32.ch.green * opa) >> 8);
        colMix.ch.blue = (uint8_t)(((uint16_t)col32.ch.blue * opa) >> 8);
        colMix.ch.alpha = opa;

        if((disp != NULL) && (disp->driver->clean_dcache_cb != NULL)) {  /*Clean & invalidate cache*/
            disp->driver->clean_dcache_cb(disp->driver);
        }

        vg_lite_matrix_t matrix;
        vg_lite_identity(&matrix);

        /*Draw rectangle*/
        err = vg_lite_draw(&rt, &path, VG_LITE_FILL_EVEN_ODD, &matrix, VG_LITE_BLEND_SRC_OVER, colMix.full);
        if(err != VG_LITE_SUCCESS) {
#if LV_GPU_NXP_VG_LITE_LOG_ERRORS
            LV_LOG_ERROR("vg_lite_draw() failed.");
#endif
            vg_lite_clear_path(&path);
            return LV_RES_INV;
        }

        err = vg_lite_finish();
        if(err != VG_LITE_SUCCESS) {
#if LV_GPU_NXP_VG_LITE_LOG_ERRORS
            LV_LOG_ERROR("vg_lite_finish() failed.");
#endif
            return LV_RES_INV;
        }

        err = vg_lite_clear_path(&path);
        if(err != VG_LITE_SUCCESS) {
#if LV_GPU_NXP_VG_LITE_LOG_ERRORS
            LV_LOG_ERROR("vg_lite_clear_path() failed.");
#endif
            return LV_RES_INV;
        }
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
#if _BLIT_SPLIT_ENABLED

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


    uint32_t totalWidth = lv_area_get_width(&blit->src_area);
    uint32_t totalHeight = lv_area_get_height(&blit->src_area);

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
            _draw_rectangle((lv_color_t *) tileBlit.dst, tileBlit.dst_width, tileBlit.dst_height, &tileBlit.dst_area, LV_COLOR_RED);
            _draw_rectangle((lv_color_t *) tileBlit.src, tileBlit.src_width, tileBlit.src_height, &tileBlit.src_area,
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
                LV_LOG_ERROR("Split BLIT failed. Trying SW BLIT instead.");
#endif
                _sw_blit(&tileBlit);
                rv = LV_RES_OK; /* Don't report error, as SW BLIT was performed */
            }

        }
        PRINT_BLT(" \n");
    }

    return rv; /* should never fail */

#else /* non RT595 */
    /* Just pass down */
    return _lv_gpu_nxp_vglite_blit_single(blit);
#endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/***
 * BLock Image Transfer - single direct BLIT.
 * @param[in] blit Description of the transfer
 * @retval LV_RES_OK Transfer complete
 * @retval LV_RES_INV Error occurred (\see LV_GPU_NXP_VG_LITE_LOG_ERRORS)
 */
static lv_res_t _lv_gpu_nxp_vglite_blit_single(lv_gpu_nxp_vglite_blit_info_t * blit)
{
    vg_lite_buffer_t src_vgbuf, dst_vgbuf;
    vg_lite_error_t err = VG_LITE_SUCCESS;
    uint32_t rect[4];
    lv_disp_t * disp = _lv_refr_get_disp_refreshing();

    if(blit == NULL) {
        /*Wrong parameter*/
        return LV_RES_INV;
    }

    if(blit->opa < (lv_opa_t) LV_OPA_MIN) {
        return LV_RES_OK; /*Nothing to BLIT*/
    }

    /*Wrap src/dst buffer into VG-Lite buffer*/
    if(_init_vg_buf(&src_vgbuf, (uint32_t) blit->src_width, (uint32_t) blit->src_height, (uint32_t) blit->src_stride,
                    blit->src, true) != LV_RES_OK) {
#if LV_GPU_NXP_VG_LITE_LOG_ERRORS
        LV_LOG_ERROR("init_vg_buf reported error. BLIT failed.");
#endif
        return LV_RES_INV;
    }

    if(_init_vg_buf(&dst_vgbuf, (uint32_t) blit->dst_width, (uint32_t) blit->dst_height, (uint32_t) blit->dst_stride,
                    blit->dst, false) != LV_RES_OK) {
#if LV_GPU_NXP_VG_LITE_LOG_ERRORS
        LV_LOG_ERROR("init_vg_buf reported error. BLIT failed.");
#endif
        return LV_RES_INV;
    }

    rect[0] = (uint32_t) blit->src_area.x1; /* start x */
    rect[1] = (uint32_t) blit->src_area.y1; /* start y */
    rect[2] = (uint32_t) blit->src_area.x2 - (uint32_t) blit->src_area.x1 + 1U; /* width */
    rect[3] = (uint32_t) blit->src_area.y2 - (uint32_t) blit->src_area.y1 + 1U; /* height */

    vg_lite_matrix_t matrix;
    vg_lite_identity(&matrix);
    vg_lite_translate((vg_lite_float_t)blit->dst_area.x1, (vg_lite_float_t)blit->dst_area.y1, &matrix);

    if((disp != NULL) && (disp->driver->clean_dcache_cb != NULL)) {  /*Clean & invalidate cache*/
        disp->driver->clean_dcache_cb(disp->driver);
    }

    uint32_t color;
    vg_lite_blend_t blend;
    if(blit->opa >= (uint8_t) LV_OPA_MAX) {
        color = 0x0;
        blend = VG_LITE_BLEND_NONE;
    }
    else {
        uint32_t opa = (uint32_t) blit->opa;
        color = (opa << 24) | (opa << 16) | (opa << 8) | opa;
        blend = VG_LITE_BLEND_SRC_OVER;
        src_vgbuf.image_mode = VG_LITE_MULTIPLY_IMAGE_MODE;
    }

    err = vg_lite_blit_rect(&dst_vgbuf, &src_vgbuf, rect, &matrix, blend, color, VG_LITE_FILTER_POINT);
    if(err != VG_LITE_SUCCESS) {
#if LV_GPU_NXP_VG_LITE_LOG_ERRORS
        LV_LOG_ERROR("vg_lite_blit_rect() failed.");
#endif
        return LV_RES_INV;
    }

    err = vg_lite_finish();
    if(err != VG_LITE_SUCCESS) {
#if LV_GPU_NXP_VG_LITE_LOG_ERRORS
        LV_LOG_ERROR("vg_lite_finish() failed.");
#endif
        return LV_RES_INV;
    }

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

/***
 * Fills vg_lite_buffer_t structure according given parameters.
 * @param[out] dst Buffer structure to be filled
 * @param[in] width Width of buffer in pixels
 * @param[in] height Height of buffer in pixels
 * @param[in] stride Stride of the buffer in bytes
 * @param[in] ptr Pointer to the buffer (must be aligned according VG-Lite requirements)
 */
static lv_res_t _init_vg_buf(vg_lite_buffer_t * dst, uint32_t width, uint32_t height, uint32_t stride,
                             const lv_color_t * ptr, bool source)
{
    if((((uintptr_t)ptr) % (uintptr_t)LV_ATTRIBUTE_MEM_ALIGN_SIZE) != 0x0U) {  /*Test for alignment*/
#if LV_GPU_NXP_VG_LITE_LOG_ERRORS
        LV_LOG_ERROR("ptr (0x%X) not aligned to %d.", (size_t) ptr, LV_ATTRIBUTE_MEM_ALIGN_SIZE);
#endif
        return LV_RES_INV;
    }

    if(source &&
       (stride % (LV_GPU_NXP_VG_LITE_STRIDE_ALIGN_PX * sizeof(lv_color_t))) != 0x0U) {  /*Test for stride alignment*/
#if LV_GPU_NXP_VG_LITE_LOG_ERRORS
        LV_LOG_ERROR("Buffer stride (%d px) not aligned to %d bytes.", stride,
                     LV_GPU_NXP_VG_LITE_STRIDE_ALIGN_PX * sizeof(lv_color_t));
#endif
        return LV_RES_INV;
    }

    dst->format = VGLITE_PX_FMT;
    dst->tiled = VG_LITE_LINEAR;
    dst->image_mode = VG_LITE_NORMAL_IMAGE_MODE;
    dst->transparency_mode = VG_LITE_IMAGE_OPAQUE;

    dst->width = (int32_t) width;
    dst->height = (int32_t) height;
    dst->stride = (int32_t) stride;

    void * r_ptr = memset(&dst->yuv, 0, sizeof(dst->yuv));
    if(r_ptr == NULL) {
        return LV_RES_INV;
    }

    dst->memory = (void *)ptr;
    dst->address = (uint32_t) dst->memory;
    dst->handle = NULL;

    return LV_RES_OK;
}

#if _BLIT_SPLIT_ENABLED

/**
 * Software BLIT as a fall-back scenario
 * @param[in] blit BLIT configuration
 */
static void _sw_blit(lv_gpu_nxp_vglite_blit_info_t * blit)
{
    int x, y;

    lv_coord_t w = lv_area_get_width(&blit->src_area);
    lv_coord_t h = lv_area_get_height(&blit->src_area);

    uint32_t srcStridePx = blit->src_stride / sizeof(lv_color_t);
    uint32_t dstStridePx = blit->dst_stride / sizeof(lv_color_t);

    lv_color_t * src = (lv_color_t *)blit->src + blit->src_area.y1 * srcStridePx + blit->src_area.x1;
    lv_color_t * dst = (lv_color_t *)blit->dst + blit->dst_area.y1 * dstStridePx + blit->dst_area.x1;

    if(blit->opa >= LV_OPA_MAX) {
        /* simple copy */
        for(y = 0; y < h; y++) {
            _lv_memcpy(dst, src, w * sizeof(lv_color_t));
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

/**
 * Verify BLIT structure - widths, stride, pointer alignment
 * @param[in] blit
 * @return
 */
static lv_res_t _lv_gpu_nxp_vglite_check_blit(lv_gpu_nxp_vglite_blit_info_t * blit)
{

    if(lv_area_get_width(&blit->src_area) < LV_GPU_NXP_VG_LITE_STRIDE_ALIGN_PX) {  /* Test for minimal width */
#if LV_GPU_NXP_VG_LITE_LOG_ERRORS
        LV_LOG_ERROR("source area width (%d) is smaller than required (%d).", lv_area_get_width(&blit->src_area),
                     LV_GPU_NXP_VG_LITE_STRIDE_ALIGN_PX);
#endif
        return LV_RES_INV;
    }

    if(lv_area_get_width(&blit->dst_area) < LV_GPU_NXP_VG_LITE_STRIDE_ALIGN_PX) {  /* Test for minimal width */
#if LV_GPU_NXP_VG_LITE_LOG_ERRORS
        LV_LOG_ERROR("destination area width (%d) is smaller than required (%d).", lv_area_get_width(&blit->dst_area),
                     LV_GPU_NXP_VG_LITE_STRIDE_ALIGN_PX);
#endif
        return LV_RES_INV;
    }

    if((((uintptr_t) blit->src) % LV_ATTRIBUTE_MEM_ALIGN_SIZE) != 0x0) {  /* Test for pointer alignment */
#if LV_GPU_NXP_VG_LITE_LOG_ERRORS
        LV_LOG_ERROR("source buffer ptr (0x%X) not aligned to %d.", (size_t) blit->src, LV_ATTRIBUTE_MEM_ALIGN_SIZE);
#endif
        return LV_RES_INV;
    }
    /* No alignment requirement for destination pixel buffer when using mode VG_LITE_LINEAR */

    if((blit->src_stride % (LV_GPU_NXP_VG_LITE_STRIDE_ALIGN_PX * LV_COLOR_DEPTH / 8)) !=
       0x0) {  /* Test for stride alignment */
#if LV_GPU_NXP_VG_LITE_LOG_ERRORS
        LV_LOG_ERROR("source buffer stride (%d px) not aligned to %d px.", blit->src_stride,
                     LV_GPU_NXP_VG_LITE_STRIDE_ALIGN_PX);
#endif
        return LV_RES_INV;
    }

    if((blit->dst_stride % (LV_GPU_NXP_VG_LITE_STRIDE_ALIGN_PX * LV_COLOR_DEPTH / 8)) !=
       0x0) {  /* Test for stride alignment */
#if LV_GPU_NXP_VG_LITE_LOG_ERRORS
        LV_LOG_ERROR("destination buffer stride (%d px) not aligned to %d px.", blit->dst_stride,
                     LV_GPU_NXP_VG_LITE_STRIDE_ALIGN_PX);
#endif
        return LV_RES_INV;
    }

    if((lv_area_get_width(&blit->src_area) != lv_area_get_width(&blit->dst_area)) ||
       (lv_area_get_height(&blit->src_area) != lv_area_get_height(&blit->dst_area))) {
#if LV_GPU_NXP_VG_LITE_LOG_ERRORS
        LV_LOG_ERROR("source and destination buffer areas are not equal.");
#endif
        return LV_RES_INV;
    }

    return LV_RES_OK;
}

/***
 * Move buffer pointer as close as possible to area, but with respect to alignment requirements. X-axis only.
 * @param[in,out] area Area to be updated
 * @param[in,out] buf Pointer to be updated
 */
static void _align_x(lv_area_t * area, lv_color_t ** buf)
{

    int alignedAreaStartPx = area->x1 - (area->x1 % (LV_ATTRIBUTE_MEM_ALIGN_SIZE * 8 / LV_COLOR_DEPTH));
    CHECK(alignedAreaStartPx < 0, "Should never happen.");

    area->x1 -= alignedAreaStartPx;
    area->x2 -= alignedAreaStartPx;
    *buf += alignedAreaStartPx;
}

/***
 * Move buffer pointer to the area start and update variables, Y-axis only.
 * @param[in,out] area Area to be updated
 * @param[in,out] buf Pointer to be updated
 * @param[in] stridePx Buffer stride in pixels
 */
static void _align_y(lv_area_t * area, lv_color_t ** buf, uint32_t stridePx)
{
    int LineToAlignMem;
    int alignedAreaStartPy;
    /* find how many lines of pixels will respect memory alignment requirement */
    if(stridePx % LV_ATTRIBUTE_MEM_ALIGN_SIZE == 0) {
        alignedAreaStartPy = area->y1;
    }
    else {
        LineToAlignMem = LV_ATTRIBUTE_MEM_ALIGN_SIZE / (sizeof(lv_color_t) * LV_GPU_NXP_VG_LITE_STRIDE_ALIGN_PX);
        CHECK(LV_ATTRIBUTE_MEM_ALIGN_SIZE % (sizeof(lv_color_t) * LV_GPU_NXP_VG_LITE_STRIDE_ALIGN_PX) != 0,
              "Complex case: need gcd function.");
        alignedAreaStartPy = area->y1 - (area->y1 % LineToAlignMem);
        CHECK(alignedAreaStartPy < 0, "Should never happen.");
    }

    area->y1 -= alignedAreaStartPy;
    area->y2 -= alignedAreaStartPy;
    *buf += alignedAreaStartPy * stridePx;
}

#if BLIT_DBG_AREAS
/***
 * Draws a simple rectangle, 1 px line width.
 * @param dest_buf Destination buffer
 * @param dest_width Destination buffer width (must be aligned on 16px)
 * @param dest_height Destination buffer height
 * @param fill_area Rectangle coordinates
 * @param color Rectangle color
 */
static void _draw_rectangle(lv_color_t * dest_buf, lv_coord_t dest_width, lv_coord_t dest_height,
                            lv_area_t * fill_area, lv_color_t color)
{

    lv_area_t a;

    /* top line */
    a.x1 = fill_area->x1;
    a.x2 = fill_area->x2;
    a.y1 = fill_area->y1;
    a.y2 = fill_area->y1;
    lv_gpu_nxp_vglite_fill(dest_buf, dest_width, dest_height, &a, color, LV_OPA_COVER);


    /* bottom line */
    a.x1 = fill_area->x1;
    a.x2 = fill_area->x2;
    a.y1 = fill_area->y2;
    a.y2 = fill_area->y2;
    lv_gpu_nxp_vglite_fill(dest_buf, dest_width, dest_height, &a, color, LV_OPA_COVER);

    /* left line */
    a.x1 = fill_area->x1;
    a.x2 = fill_area->x1;
    a.y1 = fill_area->y1;
    a.y2 = fill_area->y2;
    lv_gpu_nxp_vglite_fill(dest_buf, dest_width, dest_height, &a, color, LV_OPA_COVER);

    /* right line */
    a.x1 = fill_area->x2;
    a.x2 = fill_area->x2;
    a.y1 = fill_area->y1;
    a.y2 = fill_area->y2;
    lv_gpu_nxp_vglite_fill(dest_buf, dest_width, dest_height, &a, color, LV_OPA_COVER);
}
#endif /* BLIT_DBG_AREAS */

#endif /* _BLIT_SPLIT_ENABLED */

#endif /*LV_USE_GPU_NXP_VG_LITE*/
