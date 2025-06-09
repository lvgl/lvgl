/**
 * @file lv_vglite_utils.c
 *
 */

/**
 * Copyright 2022-2024 NXP
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

#if LV_USE_VGLITE_DRAW_ASYNC
    static volatile bool _cmd_buf_flushed = false;
#endif

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

const char * vglite_error_to_string(vg_lite_error_t error)
{
    switch(error) {
            ENUM_TO_STRING(VG_LITE_SUCCESS);
            ENUM_TO_STRING(VG_LITE_INVALID_ARGUMENT);
            ENUM_TO_STRING(VG_LITE_OUT_OF_MEMORY);
            ENUM_TO_STRING(VG_LITE_NO_CONTEXT);
            ENUM_TO_STRING(VG_LITE_TIMEOUT);
            ENUM_TO_STRING(VG_LITE_OUT_OF_RESOURCES);
            ENUM_TO_STRING(VG_LITE_GENERIC_IO);
            ENUM_TO_STRING(VG_LITE_NOT_SUPPORT);
            ENUM_TO_STRING(VG_LITE_ALREADY_EXISTS);
            ENUM_TO_STRING(VG_LITE_NOT_ALIGNED);
            ENUM_TO_STRING(VG_LITE_FLEXA_TIME_OUT);
            ENUM_TO_STRING(VG_LITE_FLEXA_HANDSHAKE_FAIL);
        default:
            break;
    }

    return "VG_LITE_UNKNOWN_ERROR";
}

#if LV_USE_VGLITE_DRAW_ASYNC
bool vglite_cmd_buf_is_flushed(void)
{
    return _cmd_buf_flushed;
}
#endif

void vglite_run(void)
{
#if LV_USE_VGLITE_DRAW_ASYNC
    vg_lite_uint32_t gpu_idle = 0;

    VGLITE_CHECK_ERROR(vg_lite_get_parameter(VG_LITE_GPU_IDLE_STATE, 1, (vg_lite_pointer)&gpu_idle));

    if(!gpu_idle) {
        _cmd_buf_flushed = false;

        return;
    }
#endif

    /*
     * If LV_USE_VGLITE_DRAW_ASYNC is enabled, simply flush the command buffer and the
     * vglite draw thread will signal asynchronous the dispatcher for completed tasks.
     * Without draw async, process the tasks and signal them as complete one by one.
     */
#if LV_USE_VGLITE_DRAW_ASYNC
    VGLITE_CHECK_ERROR(vg_lite_flush());
    _cmd_buf_flushed = true;
#else
    VGLITE_CHECK_ERROR(vg_lite_finish());
#endif
}

#if LV_USE_VGLITE_DRAW_ASYNC
void vglite_wait_for_finish(void)
{
    VGLITE_CHECK_ERROR(vg_lite_finish());
}
#endif

vg_lite_color_t vglite_get_color(lv_color32_t lv_col32, bool gradient)
{
    vg_lite_color_t vg_col32;

    /* Pre-multiply alpha */
    lv_col32.red = LV_UDIV255(lv_col32.red * lv_col32.alpha);
    lv_col32.green = LV_UDIV255(lv_col32.green * lv_col32.alpha);
    lv_col32.blue = LV_UDIV255(lv_col32.blue * lv_col32.alpha);

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
    vg_lite_blend_t vg_blend_mode = VG_LITE_BLEND_NONE;

    if(vg_lite_query_feature(gcFEATURE_BIT_VG_LVGL_SUPPORT)) {
        switch(lv_blend_mode) {
            case LV_BLEND_MODE_NORMAL:
                vg_blend_mode = VG_LITE_BLEND_NORMAL_LVGL;
                break;
            case LV_BLEND_MODE_ADDITIVE:
                vg_blend_mode = VG_LITE_BLEND_ADDITIVE_LVGL;
                break;
            case LV_BLEND_MODE_SUBTRACTIVE:
                vg_blend_mode = VG_LITE_BLEND_SUBTRACT_LVGL;
                break;
            case LV_BLEND_MODE_MULTIPLY:
                vg_blend_mode = VG_LITE_BLEND_MULTIPLY_LVGL;
                break;
            default:
                VGLITE_ASSERT_MSG(false, "Unsupported blend mode.");
                break;
        }
    }
    else {
        switch(lv_blend_mode) {
            case LV_BLEND_MODE_NORMAL:
                vg_blend_mode = VG_LITE_BLEND_SRC_OVER;
                break;
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
                VGLITE_ASSERT_MSG(false, "Unsupported blend mode.");
                break;
        }
    }

    return vg_blend_mode;
}

vg_lite_buffer_format_t vglite_get_buf_format(lv_color_format_t cf)
{
    vg_lite_buffer_format_t vg_buffer_format = VG_LITE_BGR565;

    switch(cf) {
        case LV_COLOR_FORMAT_L8:
            vg_buffer_format = VG_LITE_L8;
            break;
        case LV_COLOR_FORMAT_A4:
            vg_buffer_format = VG_LITE_A4;
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
        case LV_COLOR_FORMAT_RGB565:
            vg_buffer_format = VG_LITE_BGR565;
            break;
        case LV_COLOR_FORMAT_ARGB8565:
            vg_buffer_format = VG_LITE_BGRA5658;
            break;
        case LV_COLOR_FORMAT_RGB888:
            vg_buffer_format = VG_LITE_BGR888;
            break;
        case LV_COLOR_FORMAT_ARGB8888:
            vg_buffer_format = VG_LITE_BGRA8888;
            break;
        case LV_COLOR_FORMAT_XRGB8888:
            vg_buffer_format = VG_LITE_BGRX8888;
            break;

        default:
            VGLITE_ASSERT_MSG(false, "Unsupported color format.");
            break;
    }

    return vg_buffer_format;
}

uint8_t vglite_get_stride_alignment(lv_color_format_t cf)
{
    uint8_t align_bytes = LV_COLOR_DEPTH / 8 * 16; /*16 pixels*/

    switch(cf) {
        case LV_COLOR_FORMAT_I1:
        case LV_COLOR_FORMAT_I2:
        case LV_COLOR_FORMAT_I4:
        case LV_COLOR_FORMAT_A4:
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
        case LV_COLOR_FORMAT_ARGB8565:
        case LV_COLOR_FORMAT_RGB888:
            align_bytes = 48;
            break;
        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_XRGB8888:
            align_bytes = 64;
            break;

        default:
            VGLITE_ASSERT_MSG(false, "Unsupported buffer format.");
            break;
    }

    return align_bytes;
}

bool vglite_src_buf_aligned(const void * buf, uint32_t stride, lv_color_format_t cf)
{
    /* No alignment requirement for destination buffer when using mode VG_LITE_LINEAR */

    /* Test for pointer alignment */
    if((uintptr_t)buf % LV_ATTRIBUTE_MEM_ALIGN_SIZE)
        return false;

    /* Test for stride alignment */
    if(stride == 0 || stride % vglite_get_stride_alignment(cf))
        return false;

    return true;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_DRAW_VGLITE*/
