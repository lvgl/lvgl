/**
 * @file lv_gpu_nxp_vglite.c
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

#include "lv_gpu_nxp_vglite.h"

#if LV_USE_GPU_NXP_VG_LITE
#include "../../../core/lv_refr.h"
#if BLIT_DBG_AREAS
    #include "lv_draw_vglite_blend.h"
#endif

/*********************
 *      DEFINES
 *********************/

#if LV_COLOR_DEPTH==16
    #define VG_LITE_PX_FMT VG_LITE_RGB565
#elif LV_COLOR_DEPTH==32
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

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_res_t lv_vglite_init_buf(vg_lite_buffer_t * vgbuf, uint32_t width, uint32_t height, uint32_t stride,
                            const lv_color_t * ptr, bool source)
{
    /*Test for memory alignment*/
    if((((uintptr_t)ptr) % (uintptr_t)LV_ATTRIBUTE_MEM_ALIGN_SIZE) != (uintptr_t)0x0U)
        VG_LITE_RETURN_INV("%s buffer (0x%x) not aligned to %d.", source ? "Src" : "Dest",
                           (size_t) ptr, LV_ATTRIBUTE_MEM_ALIGN_SIZE);

    /*Test for stride alignment*/
    if(source && (stride % (LV_GPU_NXP_VG_LITE_STRIDE_ALIGN_PX * sizeof(lv_color_t))) != 0x0U)
        VG_LITE_RETURN_INV("Src buffer stride (%d bytes) not aligned to %d bytes.", stride,
                           LV_GPU_NXP_VG_LITE_STRIDE_ALIGN_PX * sizeof(lv_color_t));

    vgbuf->format = VG_LITE_PX_FMT;
    vgbuf->tiled = VG_LITE_LINEAR;
    vgbuf->image_mode = VG_LITE_NORMAL_IMAGE_MODE;
    vgbuf->transparency_mode = VG_LITE_IMAGE_OPAQUE;

    vgbuf->width = (int32_t)width;
    vgbuf->height = (int32_t)height;
    vgbuf->stride = (int32_t)stride;

    lv_memzero(&vgbuf->yuv, sizeof(vgbuf->yuv));

    vgbuf->memory = (void *)ptr;
    vgbuf->address = (uint32_t)vgbuf->memory;
    vgbuf->handle = NULL;

    return LV_RES_OK;
}

#if BLIT_DBG_AREAS
void lv_vglite_dbg_draw_rectangle(lv_color_t * dest_buf, lv_coord_t dest_width, lv_coord_t dest_height,
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

void lv_vglite_invalidate_cache(void)
{
    lv_disp_t * disp = _lv_refr_get_disp_refreshing();
    if(disp->driver->clean_dcache_cb)
        disp->driver->clean_dcache_cb(disp->driver);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_GPU_NXP_VG_LITE*/
