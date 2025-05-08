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
static void convert_RGB565A8_to_ARGB4444(const uint8_t * src, uint8_t * dst_argb4444, uint16_t width, uint16_t height);
static void convert_RGB565A8_to_ARGB1555(const uint8_t * src, uint8_t * dst, uint16_t width, uint16_t height);
static void convert_ARGB8888_to_ARGB4444(const uint8_t * src, uint8_t * dst, uint16_t width, uint16_t height);

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

void lv_draw_eve_layer(lv_draw_task_t * t, const lv_draw_image_dsc_t * draw_dsc, const lv_area_t * coords)
{

    /*
        lv_layer_t * layer_to_draw = (lv_layer_t *)draw_dsc->src;

        if(layer_to_draw->draw_buf == NULL) return;

        lv_image_dsc_t img_dsc = { 0 };
        img_dsc.header.w = lv_area_get_width(&layer_to_draw->buf_area);
        img_dsc.header.h = lv_area_get_height(&layer_to_draw->buf_area);
        img_dsc.header.cf = layer_to_draw->color_format;
        img_dsc.header.stride = layer_to_draw->buf_area;
        img_dsc.data = layer_to_draw->buf;

        lv_draw_image_dsc_t new_draw_dsc;
        lv_memcpy(&new_draw_dsc, draw_dsc, sizeof(lv_draw_image_dsc_t));
        new_draw_dsc.src = &img_dsc;
    */
    // TODO  lv_draw_eve_image(draw_unit, &new_draw_dsc, coords);

}


void lv_draw_eve_image(lv_draw_task_t * t, const lv_draw_image_dsc_t * draw_dsc, const lv_area_t * coords)
{

    const lv_image_dsc_t * img_dsc = draw_dsc->src;
    const uint8_t * img_src = img_dsc->data;

    int32_t img_w = img_dsc->header.w;
    int32_t img_h = img_dsc->header.h;
    uint16_t color_f = img_dsc->header.cf;
    uint16_t img_stride = img_dsc->header.stride;
    int32_t stride_px_w = img_stride / lv_color_format_get_size(color_f);
    int32_t img_size = stride_px_w * img_h * 2;

    uint32_t img_eveId = lv_draw_eve_find_ramg_image(img_src);

    if(img_eveId == NOT_FOUND_BLOCK) { /* New image to load  */

        uint32_t free_ramg_block = lv_draw_eve_next_free_ramg_block(TYPE_IMAGE);
        uint32_t start_addr_ramg = lv_draw_eve_get_ramg_ptr();


        /* Load image to RAM_G */
        EVE_end_cmd_burst();

        LV_ATTRIBUTE_MEM_ALIGN uint8_t * temp_buff = lv_malloc_zeroed(img_size);
        LV_ASSERT_MALLOC(temp_buff);

        uint8_t * buffer_converted = NULL;

        switch(color_f) {
            case LV_COLOR_FORMAT_L8 :
                buffer_converted = (uint8_t *)img_src;
                img_size = img_size / 2;
                break;
            case LV_COLOR_FORMAT_RGB565 :
                buffer_converted = (uint8_t *)img_src;
                break;
            case LV_COLOR_FORMAT_RGB565A8 :
                //convert_RGB565A8_to_ARGB4444(src_buf, temp_buff, img_w, img_h);
                convert_RGB565A8_to_ARGB1555(img_src, temp_buff, stride_px_w, img_h);
                buffer_converted = temp_buff;
                break;
            case LV_COLOR_FORMAT_ARGB8888 :
                convert_ARGB8888_to_ARGB4444(img_src, temp_buff, stride_px_w, img_h);
                buffer_converted = temp_buff;
                break;
            default :
                break;
        }

        EVE_memWrite_flash_buffer(start_addr_ramg, buffer_converted, (uint32_t)img_size);

        lv_free(temp_buff);
        /* Save RAM_G Memory Block ID info */
        lv_draw_eve_update_ramg_block(free_ramg_block, (uint8_t *)img_src, start_addr_ramg, img_size);

        EVE_start_cmd_burst();
    }

    lv_eve_scissor(t->clip_area.x1, t->clip_area.y1, t->clip_area.x2, t->clip_area.y2);

    lv_eve_save_context();

    if(draw_dsc->recolor_opa > LV_OPA_MIN) {
        lv_eve_color_opa(draw_dsc->recolor_opa);
        lv_eve_color(draw_dsc->recolor);
    }

    uint32_t img_addr = lv_draw_eve_get_bitmap_addr(img_eveId);

    lv_eve_primitive(LV_EVE_PRIMITIVE_BITMAPS);
    EVE_cmd_dl_burst(BITMAP_SOURCE(img_addr));
    //EVE_cmd_dl_burst(BITMAP_LAYOUT_H(); /*TODO*/
    //EVE_cmd_dl_burst(BITMAP_SIZE_H)();
    EVE_cmd_dl_burst(BITMAP_SIZE(EVE_NEAREST, EVE_BORDER, EVE_BORDER, img_w,
                                 img_h)); /*real height and wide is mandatory for rotation a scale (Clip Area)*/

    uint8_t eve_format = EVE_ARGB4;
    switch(color_f) {
        case LV_COLOR_FORMAT_L8 :
            eve_format = EVE_L8;
            // img_stride = img_w;
            break;
        case LV_COLOR_FORMAT_RGB565 :
            eve_format = EVE_RGB565;
            // img_stride = img_w * 2;
            break;
        case LV_COLOR_FORMAT_RGB565A8 :
            eve_format = EVE_ARGB1555;
            // img_stride = img_w * 2;
            break;
        case LV_COLOR_FORMAT_ARGB8888 :
            eve_format = EVE_ARGB4;
            // img_stride = img_w * 2;
            break;
        default :
            break;
    }

    img_stride /= 2;

    EVE_cmd_dl_burst(BITMAP_LAYOUT(eve_format, img_stride, img_h));

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
    // EVE_end_cmd_burst();
    // EVE_execute_cmd();
    // EVE_start_cmd_burst();
}





/**********************
 *   STATIC FUNCTIONS
 **********************/

static void convert_RGB565A8_to_ARGB4444(const uint8_t * src, uint8_t * dst, uint16_t width, uint16_t height)
{
    int pixel_count = width * height;
    uint16_t * src_rgb565 = (uint16_t *) src;
    uint8_t * src_alpha = (uint8_t *)src + 2 * pixel_count;

    for(int i = 0; i < pixel_count; i++) {
        uint16_t rgb565 = src_rgb565[i];
        uint8_t alpha = src_alpha[i];
        uint8_t r5 = (rgb565 >> 11) & 0x1F;
        uint8_t g6 = (rgb565 >> 5) & 0x3F;
        uint8_t b5 = rgb565 & 0x1F;
        uint8_t r4 = r5 >> 1;
        uint8_t g4 = g6 >> 2;
        uint8_t b4 = b5 >> 1;
        uint8_t a4 = alpha >> 4;
        uint16_t argb4444 = (a4 << 12) | (r4 << 8) | (g4 << 4) | b4;

        dst[2 * i] = argb4444 & 0xFF;
        dst[2 * i + 1] = (argb4444 >> 8) & 0xFF;
    }
}

static void convert_RGB565A8_to_ARGB1555(const uint8_t * src, uint8_t * dst, uint16_t width, uint16_t height)
{
    int pixel_count = width * height;
    uint16_t * src_rgb565 = (uint16_t *) src;
    uint8_t * src_alpha = (uint8_t *)src + 2 * pixel_count;

    for(int i = 0; i < pixel_count; i++) {

        uint16_t rgb565 = src_rgb565[i];
        uint8_t alpha = src_alpha[i];
        uint8_t r5 = (rgb565 >> 11) & 0x1F;
        uint8_t g6 = (rgb565 >> 5) & 0x3F;
        uint8_t b5 = rgb565 & 0x1F;
        uint8_t a1 = alpha >= 128 ? 1 : 0;

        uint16_t argb1555 = (a1 << 15) | (r5 << 10) | ((g6 >> 1) << 5) | b5;

        dst[2 * i] = argb1555 & 0xFF;
        dst[2 * i + 1] = (argb1555 >> 8) & 0xFF;

    }
}

static void convert_ARGB8888_to_ARGB4444(const uint8_t * src, uint8_t * dst, uint16_t width, uint16_t height)
{
    int pixel_count = width * height;

    for(int i = 0; i < pixel_count; i++) {
        uint8_t blue = src[4 * i];
        uint8_t green = src[4 * i + 1];
        uint8_t red = src[4 * i + 2];
        uint8_t alpha = src[4 * i + 3];
        uint8_t r4 = red >> 4;
        uint8_t g4 = green >> 4;
        uint8_t b4 = blue >> 4;
        uint8_t a4 = alpha >> 4;
        uint16_t argb4444 = (a4 << 12) | (r4 << 8) | (g4 << 4) | b4;

        dst[2 * i] = argb4444 & 0xFF;
        dst[2 * i + 1] = (argb4444 >> 8) & 0xFF;
    }
}

#endif /*LV_USE_DRAW_EVE*/
