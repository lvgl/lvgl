/**
 * @file lv_vglite_utils.c
 *
 */

/**
 * Copyright 2022, 2023 NXP
 *
 * SPDX-License-Identifier: MIT
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_vglite_utils.h"

#if LV_USE_DRAW_VGLITE
#include "lv_vglite_buf.h"

#include "../../../core/lv_refr.h"

#if LV_USE_OS
    #include "vg_lite_gpu.h"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

#if LV_USE_OS
    static volatile bool _cmd_buf_flushed = false;
#endif

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

#if LV_USE_OS
bool vglite_cmd_buf_is_flushed(void)
{
    return _cmd_buf_flushed;
}
#endif

void vglite_run(void)
{
#if LV_USE_OS
    vg_lite_gpu_state_t gpu_state = vg_lite_get_gpu_state();

    if(gpu_state == VG_LITE_GPU_BUSY) {
        _cmd_buf_flushed = false;

        return;
    }
#endif

    /*
     * For multithreading version (with OS), we simply flush the command buffer
     * and the vglite draw thread will signal the dispatcher for completed tasks.
     * Without OS, we process the tasks and signal them as complete one by one.
     */
#if LV_USE_OS
    LV_ASSERT_MSG(vg_lite_flush() == VG_LITE_SUCCESS, "Flush failed.");
    _cmd_buf_flushed = true;
#else
    LV_ASSERT_MSG(vg_lite_finish() == VG_LITE_SUCCESS, "Finish failed.");
#endif
}

vg_lite_color_t vglite_get_color(lv_color32_t lv_col32, bool gradient)
{
    vg_lite_color_t vg_col32;

    /* Only pre-multiply color if hardware pre-multiplication is not present */
    if(!vg_lite_query_feature(gcFEATURE_BIT_VG_PE_PREMULTIPLY)) {
        lv_col32.red = (uint8_t)((lv_col32.red * lv_col32.alpha) >> 8);
        lv_col32.green = (uint8_t)((lv_col32.green * lv_col32.alpha) >> 8);
        lv_col32.blue = (uint8_t)((lv_col32.blue * lv_col32.alpha) >> 8);
    }

    if(!gradient)
        /* The color is in ABGR8888 format with red channel in the lower 8 bits. */
        vg_col32 = ((vg_lite_color_t)lv_col32.alpha << 24) | ((vg_lite_color_t)lv_col32.blue << 16) |
                   ((vg_lite_color_t)lv_col32.green << 8) | (vg_lite_color_t)lv_col32.red;
    else
        /* The gradient color is in ARGB8888 format with blue channel in the lower 8 bits. */
        vg_col32 = ((vg_lite_color_t)lv_col32.alpha << 24) | ((vg_lite_color_t)lv_col32.red << 16) |
                   ((vg_lite_color_t)lv_col32.green << 8) | (vg_lite_color_t)lv_col32.blue;

    return vg_col32;
}

vg_lite_blend_t vglite_get_blend_mode(lv_blend_mode_t lv_blend_mode)
{
    vg_lite_blend_t vg_blend_mode;

    switch(lv_blend_mode) {
        case LV_BLEND_MODE_ADDITIVE:
            vg_blend_mode = VG_LITE_BLEND_ADDITIVE;
            break;
        case LV_BLEND_MODE_SUBTRACTIVE:
            vg_blend_mode = VG_LITE_BLEND_SUBTRACT;
            break;
        case LV_BLEND_MODE_MULTIPLY:
            vg_blend_mode = VG_LITE_BLEND_MULTIPLY;
            break;
        default:
            vg_blend_mode = VG_LITE_BLEND_SRC_OVER;
            break;
    }

    return vg_blend_mode;
}

vg_lite_buffer_format_t vglite_get_buf_format(lv_color_format_t cf)
{
    vg_lite_buffer_format_t vg_buffer_format = VG_LITE_BGR565;

    switch(cf) {
        /*<=1 byte (+alpha) formats*/
        case LV_COLOR_FORMAT_L8:
            vg_buffer_format = VG_LITE_L8;
            break;
        case LV_COLOR_FORMAT_A8:
            vg_buffer_format = VG_LITE_A8;
            break;
        case LV_COLOR_FORMAT_I1:
            vg_buffer_format = VG_LITE_INDEX_1;
            break;
        case LV_COLOR_FORMAT_I2:
            vg_buffer_format = VG_LITE_INDEX_2;
            break;
        case LV_COLOR_FORMAT_I4:
            vg_buffer_format = VG_LITE_INDEX_4;
            break;
        case LV_COLOR_FORMAT_I8:
            vg_buffer_format = VG_LITE_INDEX_8;
            break;

        /*2 byte (+alpha) formats*/
        case LV_COLOR_FORMAT_RGB565:
            vg_buffer_format = VG_LITE_BGR565;
            break;
        case LV_COLOR_FORMAT_RGB565A8:
            LV_ASSERT_MSG(false, "Unsupported color format.");
            break;

        /*3 byte (+alpha) formats*/
        case LV_COLOR_FORMAT_RGB888:
            LV_ASSERT_MSG(false, "Unsupported color format.");
            break;
        case LV_COLOR_FORMAT_ARGB8888:
            vg_buffer_format = VG_LITE_BGRA8888;
            break;
        case LV_COLOR_FORMAT_XRGB8888:
            vg_buffer_format = VG_LITE_BGRX8888;
            break;

        default:
            LV_ASSERT_MSG(false, "Unsupported color format.");
            break;
    }

    return vg_buffer_format;
}

uint8_t vglite_get_px_size(lv_color_format_t cf)
{
    uint8_t bits_per_pixel = LV_COLOR_DEPTH;

    switch(cf) {
        case LV_COLOR_FORMAT_I1:
            bits_per_pixel = 1;
            break;
        case LV_COLOR_FORMAT_I2:
            bits_per_pixel = 2;
            break;
        case LV_COLOR_FORMAT_I4:
            bits_per_pixel = 4;
            break;
        case LV_COLOR_FORMAT_I8:
        case LV_COLOR_FORMAT_A8:
        case LV_COLOR_FORMAT_L8:
            bits_per_pixel = 8;
            break;
        case LV_COLOR_FORMAT_RGB565:
            bits_per_pixel = 16;
            break;
        case LV_COLOR_FORMAT_RGB565A8:
        case LV_COLOR_FORMAT_RGB888:
            bits_per_pixel = 24;
            break;
        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_XRGB8888:
            bits_per_pixel = 32;
            break;

        default:
            LV_ASSERT_MSG(false, "Unsupported buffer format.");
            break;
    }

    return bits_per_pixel;
}

uint8_t vglite_get_alignment(lv_color_format_t cf)
{
    uint8_t align_bytes = LV_COLOR_DEPTH / 8 * 16; /*16 pixels*/

    switch(cf) {
        case LV_COLOR_FORMAT_I1:
        case LV_COLOR_FORMAT_I2:
        case LV_COLOR_FORMAT_I4:
            align_bytes = 8;
            break;
        case LV_COLOR_FORMAT_I8:
        case LV_COLOR_FORMAT_A8:
        case LV_COLOR_FORMAT_L8:
            align_bytes = 16;
            break;
        case LV_COLOR_FORMAT_RGB565:
            align_bytes = 32;
            break;
        case LV_COLOR_FORMAT_RGB565A8:
        case LV_COLOR_FORMAT_RGB888:
            align_bytes = 48;
            break;
        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_XRGB8888:
            align_bytes = 64;
            break;

        default:
            LV_ASSERT_MSG(false, "Unsupported buffer format.");
            break;
    }

    return align_bytes;
}

bool vglite_buf_aligned(const void * buf, uint32_t stride, lv_color_format_t cf)
{
    uint8_t align_bytes = vglite_get_alignment(cf);

    /* No alignment requirement for destination buffer when using mode VG_LITE_LINEAR */

    /* Test for pointer alignment */
    if((uintptr_t)buf % align_bytes) {
        LV_LOG_ERROR("Buffer address (0x%x) not aligned to %d bytes.",
                     (size_t)buf, align_bytes);
        return false;
    }

    /* Test for stride alignment */
    if(stride % align_bytes) {
        LV_LOG_ERROR("Buffer stride (%d bytes) not aligned to %d bytes.",
                     stride, align_bytes);
        return false;
    }

    return true;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_VGLITE*/
