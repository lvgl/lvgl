/**
 * @file lv_draw_eve_image.c
 *
 */

/*  Created on: 17 jun 2023
 *      Author: juanj
 *
 *  Modified by LVGL
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_eve_private.h"
#if LV_USE_DRAW_EVE

#include "../lv_draw_image_private.h"
#include "lv_draw_eve_ram_g.h"
#include "lv_eve.h"

/*********************
 *      DEFINES
 *********************/


/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void convert_row_rgb565a8_to_argb4444(const uint8_t * src, const uint8_t * src_alpha, uint8_t * dst,
                                             uint32_t width);
static void convert_row_argb8888_to_argb4444(const uint8_t * src, uint8_t * dst, uint32_t width);

/**********************
 *  STATIC VARIABLES
 **********************/



/**********************
 *      MACROS
 **********************/

#define F16_PIVOT_SHIFT(x) ((int32_t)((((x) >> 1)) * 65536L))
#define F16_SCALE_DIV_256(x) ((int32_t)(((x) / 256.0f) * 65536L))

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_eve_image(lv_draw_task_t * t, const lv_draw_image_dsc_t * draw_dsc, const lv_area_t * coords)
{
    if(!lv_draw_eve_image_src_check(draw_dsc->src)) {
        return;
    }

    const lv_image_dsc_t * img_dsc = draw_dsc->src;

    int32_t src_w = img_dsc->header.w;
    int32_t src_h = img_dsc->header.h;
    int32_t src_stride = img_dsc->header.stride;
    lv_color_format_t src_cf = img_dsc->header.cf;

    if(src_stride == 0) {
        src_stride = src_w * lv_color_format_get_size(src_cf);
    }

    uint8_t eve_format;
    int32_t eve_stride;

    switch(src_cf) {
        case LV_COLOR_FORMAT_L8:
            eve_format = EVE_L8;
            eve_stride = src_stride;
            break;
        case LV_COLOR_FORMAT_RGB565:
            eve_format = EVE_RGB565;
            eve_stride = src_stride;
            break;
        case LV_COLOR_FORMAT_RGB565A8:
        case LV_COLOR_FORMAT_ARGB8888:
            eve_format = EVE_ARGB4;
            eve_stride = src_w * 2;
            break;
        default :
            LV_ASSERT(0);
    }

    uint32_t ramg_addr = lv_draw_eve_image_upload_image(true, img_dsc);
    if(ramg_addr == LV_DRAW_EVE_RAMG_OUT_OF_RAMG) {
        LV_LOG_WARN("Could not load image because space could not be allocated in RAM_G.");
        return;
    }

    lv_eve_scissor(t->clip_area.x1, t->clip_area.y1, t->clip_area.x2, t->clip_area.y2);

    lv_eve_save_context();

    lv_eve_color_opa(draw_dsc->opa);

    if(draw_dsc->recolor_opa > LV_OPA_MIN) {
        lv_eve_color(lv_color_mix(draw_dsc->recolor, lv_color_white(), draw_dsc->recolor_opa));
    }

    lv_eve_primitive(LV_EVE_PRIMITIVE_BITMAPS);
    EVE_cmd_dl_burst(BITMAP_SOURCE(ramg_addr));
    /*real height and width is mandatory for rotation and scale (Clip Area)*/
    EVE_cmd_dl_burst(BITMAP_SIZE(EVE_NEAREST, EVE_BORDER, EVE_BORDER, src_w, src_h));

    EVE_cmd_dl_burst(BITMAP_LAYOUT(eve_format, eve_stride, src_h));

    if(draw_dsc->rotation || draw_dsc->scale_x != LV_SCALE_NONE || draw_dsc->scale_y != LV_SCALE_NONE) {
        EVE_cmd_dl_burst(CMD_LOADIDENTITY);

        EVE_cmd_translate_burst(F16(coords->x1 - t->clip_area.x1 + draw_dsc->pivot.x),
                                F16(coords->y1 - t->clip_area.y1 + draw_dsc->pivot.y));
        if(draw_dsc->scale_x != LV_SCALE_NONE || draw_dsc->scale_y != LV_SCALE_NONE) {
            /*Image Scale*/
            EVE_cmd_scale_burst(F16_SCALE_DIV_256(draw_dsc->scale_x), F16_SCALE_DIV_256(draw_dsc->scale_y));
        }
        if(draw_dsc->rotation != 0) {
            /*Image Rotate*/
            EVE_cmd_rotate_burst(DEGREES(draw_dsc->rotation));
        }
        EVE_cmd_translate_burst(-F16(draw_dsc->pivot.x), -F16(draw_dsc->pivot.y));
        EVE_cmd_dl_burst(CMD_SETMATRIX);
        EVE_cmd_dl_burst(CMD_LOADIDENTITY);
        lv_eve_vertex_2f(t->clip_area.x1, t->clip_area.y1);
    }
    else {
        lv_eve_vertex_2f(coords->x1, coords->y1);
    }
    lv_eve_restore_context();
}

bool lv_draw_eve_image_src_check(const void * src)
{
    if(lv_image_src_get_type(src) != LV_IMAGE_SRC_VARIABLE) {
        LV_LOG_WARN("lv_draw_eve can only render images from variables (not files or symbols) for now.");
        return false;
    }

    const lv_image_dsc_t * img_dsc = src;

    switch(img_dsc->header.cf) {
        case LV_COLOR_FORMAT_L8:
        case LV_COLOR_FORMAT_RGB565:
        case LV_COLOR_FORMAT_RGB565A8:
        case LV_COLOR_FORMAT_ARGB8888:
            break;
        default :
            LV_LOG_WARN("lv_draw_eve can only render L8, RGB565, RGB565A8, and ARGB8888 images for now.");
            return false;
    }

    return true;
}

uint32_t lv_draw_eve_image_upload_image(bool burst_is_active, const lv_image_dsc_t * img_dsc)
{
    const uint8_t * src_buf = img_dsc->data;
    int32_t src_w = img_dsc->header.w;
    int32_t src_h = img_dsc->header.h;
    int32_t src_stride = img_dsc->header.stride;
    lv_color_format_t src_cf = img_dsc->header.cf;

    if(src_stride == 0) {
        src_stride = src_w * lv_color_format_get_size(src_cf);
    }

    int32_t eve_stride;
    uint8_t eve_alignment;

    switch(src_cf) {
        case LV_COLOR_FORMAT_L8:
            eve_stride = src_stride;
            eve_alignment = 1;
            break;
        case LV_COLOR_FORMAT_RGB565:
            eve_stride = src_stride;
            eve_alignment = 2;
            break;
        case LV_COLOR_FORMAT_RGB565A8:
        case LV_COLOR_FORMAT_ARGB8888:
            eve_stride = src_w * 2;
            eve_alignment = 2;
            break;
        default :
            LV_ASSERT(0);
    }

    int32_t eve_size = eve_stride * src_h;

    uint32_t ramg_addr;
    bool img_is_loaded = lv_draw_eve_ramg_get_addr(&ramg_addr, (uintptr_t) src_buf, eve_size, eve_alignment);

    /* New image to load  */
    if(!img_is_loaded && ramg_addr != LV_DRAW_EVE_RAMG_OUT_OF_RAMG) {

        /* Load image to RAM_G */

        if(burst_is_active) {
            EVE_end_cmd_burst();
        }

        switch(src_cf) {
            case LV_COLOR_FORMAT_L8:
            case LV_COLOR_FORMAT_RGB565:
                EVE_memWrite_flash_buffer(ramg_addr, src_buf, eve_size);
                break;
            case LV_COLOR_FORMAT_RGB565A8: {
                    uint8_t * tmp_buf = lv_malloc(eve_stride);
                    LV_ASSERT_MALLOC(tmp_buf);
                    const uint8_t * src_alpha_buf = src_buf + src_h * src_stride;
                    int32_t src_alpha_stride = src_stride / 2;
                    for(uint32_t y = 0; y < src_h; y++) {
                        convert_row_rgb565a8_to_argb4444(src_buf + y * src_stride, src_alpha_buf + y * src_alpha_stride, tmp_buf, src_w);
                        EVE_memWrite_flash_buffer(ramg_addr + y * eve_stride, tmp_buf, eve_stride);
                    }
                    lv_free(tmp_buf);
                    break;
                }
            case LV_COLOR_FORMAT_ARGB8888: {
                    uint8_t * tmp_buf = lv_malloc(eve_stride);
                    LV_ASSERT_MALLOC(tmp_buf);
                    for(uint32_t y = 0; y < src_h; y++) {
                        convert_row_argb8888_to_argb4444(src_buf + y * src_stride, tmp_buf, src_w);
                        EVE_memWrite_flash_buffer(ramg_addr + y * eve_stride, tmp_buf, eve_stride);
                    }
                    lv_free(tmp_buf);
                    break;
                }
            default:
                LV_ASSERT(0);
        }

        if(burst_is_active) {
            EVE_start_cmd_burst();
        }
    }

    return ramg_addr;
}




/**********************
 *   STATIC FUNCTIONS
 **********************/

static void convert_row_rgb565a8_to_argb4444(const uint8_t * src, const uint8_t * src_alpha, uint8_t * dst,
                                             uint32_t width)
{
    for(uint32_t x = 0; x < width; x++) {
        uint16_t rgb565 = ((const uint16_t *) src)[x];

        uint8_t r5 = (rgb565 >> 11) & 0x1F;
        uint8_t g6 = (rgb565 >> 5) & 0x3F;
        uint8_t b5 = rgb565 & 0x1F;
        uint8_t alpha = src_alpha[x];

        uint8_t r4 = r5 >> 1;
        uint8_t g4 = g6 >> 2;
        uint8_t b4 = b5 >> 1;
        uint8_t a4 = alpha >> 4;

        uint16_t argb4444 = (a4 << 12) | (r4 << 8) | (g4 << 4) | b4;

        dst[2 * x] = argb4444 & 0xFF;
        dst[2 * x + 1] = (argb4444 >> 8) & 0xFF;
    }
}

static void convert_row_argb8888_to_argb4444(const uint8_t * src, uint8_t * dst, uint32_t width)
{
    for(uint32_t x = 0; x < width; x++) {
        uint8_t blue = src[4 * x];
        uint8_t green = src[4 * x + 1];
        uint8_t red = src[4 * x + 2];
        uint8_t alpha = src[4 * x + 3];

        uint8_t r4 = red >> 4;
        uint8_t g4 = green >> 4;
        uint8_t b4 = blue >> 4;
        uint8_t a4 = alpha >> 4;

        uint16_t argb4444 = (a4 << 12) | (r4 << 8) | (g4 << 4) | b4;

        dst[2 * x] = argb4444 & 0xFF;
        dst[2 * x + 1] = (argb4444 >> 8) & 0xFF;
    }
}

#endif /*LV_USE_DRAW_EVE*/
