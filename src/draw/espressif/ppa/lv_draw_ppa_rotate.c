/**
 * @file lv_draw_ppa_rotate.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_ppa_private.h"
#include "lv_draw_ppa.h"

#if LV_USE_PPA

/*********************
 *      DEFINES
 *********************/

/*Smaller areas are faster on the CPU. Each PPA call costs about 0.5 ms; rotating by
 *90 or 270 degrees is slow on the CPU, but rotating by 180 degrees is not.*/
#ifndef LV_PPA_ROTATE_MIN_PIXELS
    #define LV_PPA_ROTATE_MIN_PIXELS 1024
#endif
#ifndef LV_PPA_ROTATE_180_MIN_PIXELS
    #define LV_PPA_ROTATE_180_MIN_PIXELS 8192
#endif

/*Only RGB565 is supported, 2 bytes per pixel*/
#define PPA_ROTATE_PX_SIZE 2

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_result_t lv_draw_ppa_rotate(const void * src, void * dst, int32_t src_width, int32_t src_height,
                               int32_t src_stride, int32_t dst_stride, lv_rotation_t rotation,
                               lv_color_format_t cf)
{
    if(cf != LV_COLOR_FORMAT_RGB565) return LV_RESULT_INVALID;
    if(src_width <= 0 || src_height <= 0 || src_stride <= 0 || dst_stride <= 0) return LV_RESULT_INVALID;
    if(src_stride % PPA_ROTATE_PX_SIZE != 0 || dst_stride % PPA_ROTATE_PX_SIZE != 0) return LV_RESULT_INVALID;

    const uint32_t pixels = (uint32_t)src_width * (uint32_t)src_height;
    const uint32_t min_pixels = rotation == LV_ROTATION_180 ? LV_PPA_ROTATE_180_MIN_PIXELS : LV_PPA_ROTATE_MIN_PIXELS;
    if(pixels < min_pixels) return LV_RESULT_INVALID;

    ppa_srm_rotation_angle_t angle;
    bool swap_xy;
    switch(rotation) {
        /*LVGL and the PPA both count these angles counter-clockwise*/
        case LV_ROTATION_90:
            angle = PPA_SRM_ROTATION_ANGLE_90;
            swap_xy = true;
            break;
        case LV_ROTATION_180:
            angle = PPA_SRM_ROTATION_ANGLE_180;
            swap_xy = false;
            break;
        case LV_ROTATION_270:
            angle = PPA_SRM_ROTATION_ANGLE_270;
            swap_xy = true;
            break;
        default:
            return LV_RESULT_INVALID;
    }
    const uint32_t out_w = (uint32_t)(swap_xy ? src_height : src_width);
    const uint32_t out_h = (uint32_t)(swap_xy ? src_width : src_height);

    /*The output buffer must start and end on a cache line, so describe the block
     *as an offset into the cache line aligned memory around it.*/
    const uintptr_t align = LV_DRAW_PPA_CACHE_LINE_SIZE;
    uint8_t * out_base = (uint8_t *)((uintptr_t)dst & ~(align - 1));
    const uint32_t lead = (uint32_t)((uint8_t *)dst - out_base);
    const uint32_t out_stride = (uint32_t)dst_stride;
    const uint32_t offset_x = (lead % out_stride) / PPA_ROTATE_PX_SIZE;
    const uint32_t offset_y = lead / out_stride;
    const uint32_t out_pic_w = out_stride / PPA_ROTATE_PX_SIZE;
    const uint32_t out_pic_h = offset_y + out_h;
    if(offset_x + out_w > out_pic_w) return LV_RESULT_INVALID;
    const size_t out_size = LV_ALIGN_UP((size_t)out_stride * out_pic_h, align);

    /*The PPA cannot rotate in place*/
    const uint8_t * in_start = src;
    const uint8_t * in_end = in_start + (size_t)src_stride * (size_t)src_height;
    if(in_start < out_base + out_size && (const uint8_t *)out_base < in_end) return LV_RESULT_INVALID;

    ppa_client_handle_t client = lv_draw_ppa_get_srm_client();
    if(client == NULL) return LV_RESULT_INVALID;

    /*The driver writes back the input and invalidates the output, but it invalidates
     *whole cache lines. Write those back first so pixels the CPU drew next to the
     *block are not lost.*/
    esp_cache_msync(out_base, out_size, ESP_CACHE_MSYNC_FLAG_DIR_C2M | ESP_CACHE_MSYNC_FLAG_TYPE_DATA);

    ppa_srm_oper_config_t oper = {
        .in = {
            .buffer = src,
            .pic_w = (uint32_t)src_stride / PPA_ROTATE_PX_SIZE,
            .pic_h = (uint32_t)src_height,
            .block_w = (uint32_t)src_width,
            .block_h = (uint32_t)src_height,
            .srm_cm = PPA_SRM_COLOR_MODE_RGB565,
        },
        .out = {
            .buffer = out_base,
            .buffer_size = (uint32_t)out_size,
            .pic_w = out_pic_w,
            .pic_h = out_pic_h,
            .block_offset_x = offset_x,
            .block_offset_y = offset_y,
            .srm_cm = PPA_SRM_COLOR_MODE_RGB565,
        },
        .rotation_angle = angle,
        .scale_x = 1.0f,
        .scale_y = 1.0f,
        .mode = PPA_TRANS_MODE_BLOCKING,
    };

    esp_err_t ret = ppa_do_scale_rotate_mirror(client, &oper);
    if(ret != ESP_OK) {
        LV_LOG_WARN("PPA rotate failed: %d", ret);
        return LV_RESULT_INVALID;
    }
    return LV_RESULT_OK;
}

#endif /*LV_USE_PPA*/
