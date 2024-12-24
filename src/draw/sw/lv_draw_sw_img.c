/**
 * @file lv_draw_sw_img.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../misc/lv_area_private.h"
#include "blend/lv_draw_sw_blend_private.h"
#include "../lv_image_decoder_private.h"
#include "../lv_draw_image_private.h"
#include "../lv_draw_private.h"
#include "lv_draw_sw.h"
#if LV_USE_DRAW_SW

#include "../../display/lv_display.h"
#include "../../display/lv_display_private.h"
#include "../../misc/lv_log.h"
#include "../../core/lv_refr_private.h"
#include "../../stdlib/lv_mem.h"
#include "../../misc/lv_math.h"
#include "../../misc/lv_color.h"
#include "../../stdlib/lv_string.h"
#include "../../core/lv_global.h"

#if LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_HELIUM
    #include "arm2d/lv_draw_sw_helium.h"
#elif LV_USE_DRAW_SW_ASM == LV_DRAW_SW_ASM_CUSTOM
    #include LV_DRAW_SW_ASM_CUSTOM_INCLUDE
#endif

/*********************
 *      DEFINES
 *********************/
#define MAX_BUF_SIZE (uint32_t) (4 * lv_display_get_horizontal_resolution(lv_refr_get_disp_refreshing()) * lv_color_format_get_size(lv_display_get_color_format(lv_refr_get_disp_refreshing())))

#ifndef LV_DRAW_SW_IMAGE
    #define LV_DRAW_SW_IMAGE(...)   LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_RGB565_RECOLOR
    #define LV_DRAW_SW_RGB565_RECOLOR(...)  LV_RESULT_INVALID
#endif

#ifndef LV_DRAW_SW_RGB888_RECOLOR
    #define LV_DRAW_SW_RGB888_RECOLOR(...)  LV_RESULT_INVALID
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void img_draw_core(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc,
                          const lv_image_decoder_dsc_t * decoder_dsc, lv_draw_image_sup_t * sup,
                          const lv_area_t * img_coords, const lv_area_t * clipped_img_area);


static void radius_only(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc,
                        const lv_image_decoder_dsc_t * decoder_dsc,
                        const lv_area_t * img_coords, const lv_area_t * clipped_img_area);

static void recolor_only(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc,
                         const lv_image_decoder_dsc_t * decoder_dsc,
                         const lv_area_t * img_coords, const lv_area_t * clipped_img_area);

static void transform_and_recolor(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc,
                                  const lv_image_decoder_dsc_t * decoder_dsc, lv_draw_image_sup_t * sup,
                                  const lv_area_t * img_coords, const lv_area_t * clipped_img_area);


static void recolor(lv_area_t relative_area, uint8_t * src_buf, uint8_t * dest_buf, int32_t src_stride,
                    lv_color_format_t cf, const lv_draw_image_dsc_t * draw_dsc);

static bool apply_mask(const lv_draw_image_dsc_t * draw_dsc);

/**********************
 *  STATIC VARIABLES
 **********************/
#define _draw_info LV_GLOBAL_DEFAULT()->draw_info

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_sw_layer(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc, const lv_area_t * coords)
{
    lv_layer_t * layer_to_draw = (lv_layer_t *)draw_dsc->src;

    /*It can happen that nothing was draw on a layer and therefore its buffer is not allocated.
     *In this case just return. */
    if(layer_to_draw->draw_buf == NULL) return;

    if(draw_dsc->bitmap_mask_src) {
        bool visible = apply_mask(draw_dsc);
        if(!visible) return;
    }

    /*The source should be a draw_buf, not a layer*/
    lv_draw_image_dsc_t new_draw_dsc = *draw_dsc;
    new_draw_dsc.src = layer_to_draw->draw_buf;

    lv_draw_sw_image(draw_unit, &new_draw_dsc, coords);
#if LV_USE_LAYER_DEBUG || LV_USE_PARALLEL_DRAW_DEBUG
    lv_area_t area_rot;
    lv_area_copy(&area_rot, coords);
    if(draw_dsc->rotation || draw_dsc->scale_x != LV_SCALE_NONE || draw_dsc->scale_y != LV_SCALE_NONE) {
        int32_t w = lv_area_get_width(coords);
        int32_t h = lv_area_get_height(coords);

        lv_image_buf_get_transformed_area(&area_rot, w, h, draw_dsc->rotation, draw_dsc->scale_x, draw_dsc->scale_y,
                                          &draw_dsc->pivot);

        area_rot.x1 += coords->x1;
        area_rot.y1 += coords->y1;
        area_rot.x2 += coords->x1;
        area_rot.y2 += coords->y1;
    }
    lv_area_t draw_area;
    if(!lv_area_intersect(&draw_area, &area_rot, draw_unit->clip_area)) return;
#endif

#if LV_USE_LAYER_DEBUG
    lv_draw_fill_dsc_t fill_dsc;
    lv_draw_fill_dsc_init(&fill_dsc);
    fill_dsc.color = lv_color_hex(layer_to_draw->color_format == LV_COLOR_FORMAT_ARGB8888 ? 0xff0000 : 0x00ff00);
    fill_dsc.opa = LV_OPA_20;
    lv_draw_sw_fill(draw_unit, &fill_dsc, &area_rot);

    lv_draw_border_dsc_t border_dsc;
    lv_draw_border_dsc_init(&border_dsc);
    border_dsc.color = fill_dsc.color;
    border_dsc.opa = LV_OPA_60;
    border_dsc.width = 2;
    lv_draw_sw_border(draw_unit, &border_dsc, &area_rot);

#endif

#if LV_USE_PARALLEL_DRAW_DEBUG
    uint32_t idx = 0;
    lv_draw_unit_t * draw_unit_tmp = _draw_info.unit_head;
    while(draw_unit_tmp != draw_unit) {
        draw_unit_tmp = draw_unit_tmp->next;
        idx++;
    }

    lv_draw_fill_dsc_t fill_dsc;
    lv_draw_fill_dsc_init(&fill_dsc);
    fill_dsc.color = lv_palette_main(idx % LV_PALETTE_LAST);
    fill_dsc.opa = LV_OPA_10;
    lv_draw_sw_fill(draw_unit, &fill_dsc, &area_rot);

    lv_draw_border_dsc_t border_dsc;
    lv_draw_border_dsc_init(&border_dsc);
    border_dsc.color = lv_palette_main(idx % LV_PALETTE_LAST);
    border_dsc.opa = LV_OPA_60;
    border_dsc.width = 1;
    lv_draw_sw_border(draw_unit, &border_dsc, &area_rot);

    lv_point_t txt_size;
    lv_text_get_size(&txt_size, "W", LV_FONT_DEFAULT, 0, 0, 100, LV_TEXT_FLAG_NONE);

    lv_area_t txt_area;
    txt_area.x1 = draw_area.x1;
    txt_area.x2 = draw_area.x1 + txt_size.x - 1;
    txt_area.y2 = draw_area.y2;
    txt_area.y1 = draw_area.y2 - txt_size.y + 1;

    lv_draw_fill_dsc_init(&fill_dsc);
    fill_dsc.color = lv_color_black();
    lv_draw_sw_fill(draw_unit, &fill_dsc, &txt_area);

    char buf[8];
    lv_snprintf(buf, sizeof(buf), "%d", idx);
    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    label_dsc.color = lv_color_white();
    label_dsc.text = buf;
    lv_draw_sw_label(draw_unit, &label_dsc, &txt_area);
#endif
}

void lv_draw_sw_image(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc,
                      const lv_area_t * coords)
{
    if(!draw_dsc->tile) {
        lv_draw_image_normal_helper(draw_unit, draw_dsc, coords, img_draw_core);
    }
    else {
        lv_draw_image_tiled_helper(draw_unit, draw_dsc, coords, img_draw_core);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void img_draw_core(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc,
                          const lv_image_decoder_dsc_t * decoder_dsc, lv_draw_image_sup_t * sup,
                          const lv_area_t * img_coords, const lv_area_t * clipped_img_area)
{
    bool transformed = draw_dsc->rotation != 0 || draw_dsc->scale_x != LV_SCALE_NONE ||
                       draw_dsc->scale_y != LV_SCALE_NONE ? true : false;

    bool radius = draw_dsc->clip_radius > 0;

    const lv_draw_buf_t * decoded = decoder_dsc->decoded;
    const uint8_t * src_buf = decoded->data;
    const lv_image_header_t * header = &decoded->header;
    uint32_t img_stride = decoded->header.stride;
    lv_color_format_t cf = decoded->header.cf;

    lv_draw_sw_blend_dsc_t blend_dsc;
    lv_memzero(&blend_dsc, sizeof(lv_draw_sw_blend_dsc_t));
    blend_dsc.opa = draw_dsc->opa;
    blend_dsc.blend_mode = draw_dsc->blend_mode;
    blend_dsc.src_stride = img_stride;

    if(!transformed && !radius && cf == LV_COLOR_FORMAT_A8) {
        lv_area_t clipped_coords;
        if(!lv_area_intersect(&clipped_coords, img_coords, draw_unit->clip_area)) return;

        blend_dsc.mask_buf = (lv_opa_t *)src_buf;
        blend_dsc.mask_area = img_coords;
        blend_dsc.mask_stride = img_stride;
        blend_dsc.src_buf = NULL;
        blend_dsc.color = draw_dsc->recolor;
        blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;

        blend_dsc.blend_area = img_coords;
        lv_draw_sw_blend(draw_unit, &blend_dsc);
    }
    else if(!transformed && !radius && cf == LV_COLOR_FORMAT_RGB565A8 && draw_dsc->recolor_opa <= LV_OPA_MIN) {
        int32_t src_h = lv_area_get_height(img_coords);
        int32_t src_w = lv_area_get_width(img_coords);
        blend_dsc.src_area = img_coords;
        blend_dsc.src_buf = src_buf;
        blend_dsc.mask_buf = (lv_opa_t *)src_buf;
        blend_dsc.mask_buf += img_stride * src_w / header->w * src_h;
        /**
         * Note, for RGB565A8, lacking of stride parameter, we always use
         * always half of RGB map stride as alpha map stride. The image should
         * be generated in this way too.
         */
        blend_dsc.mask_stride = img_stride / 2;
        blend_dsc.blend_area = img_coords;
        blend_dsc.mask_area = img_coords;
        blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;
        blend_dsc.src_color_format = LV_COLOR_FORMAT_RGB565;
        lv_draw_sw_blend(draw_unit, &blend_dsc);
    }
    else if(!transformed && !radius && (cf == LV_COLOR_FORMAT_L8 || cf == LV_COLOR_FORMAT_AL88)) {
        blend_dsc.src_area = img_coords;
        blend_dsc.src_buf = src_buf;
        blend_dsc.blend_area = img_coords;
        blend_dsc.src_color_format = cf;
        lv_draw_sw_blend(draw_unit, &blend_dsc);
    }
    /*The simplest case just copy the pixels into the draw_buf. Blending will convert the colors if needed*/
    else if(!transformed && !radius && draw_dsc->recolor_opa <= LV_OPA_MIN) {
        blend_dsc.src_area = img_coords;
        blend_dsc.src_buf = src_buf;
        blend_dsc.blend_area = img_coords;
        blend_dsc.src_color_format = cf;
        lv_draw_sw_blend(draw_unit, &blend_dsc);
    }
    else if(!transformed && !radius && draw_dsc->recolor_opa > LV_OPA_MIN) {
        recolor_only(draw_unit, draw_dsc, decoder_dsc, img_coords,  clipped_img_area);
    }
    /*Handle masked RGB565, RGB888, XRGB888, or ARGB8888 images*/
    else if(!transformed && radius && draw_dsc->recolor_opa <= LV_OPA_MIN) {
        radius_only(draw_unit, draw_dsc, decoder_dsc, img_coords,  clipped_img_area);
    }
    /* check whether it is possible to accelerate the operation in synchronous mode */
    else if(LV_RESULT_INVALID == LV_DRAW_SW_IMAGE(transformed,      /* whether require transform */
                                                  cf,               /* image format */
                                                  src_buf,          /* image buffer */
                                                  img_coords,       /* src_h, src_w, src_x1, src_y1 */
                                                  img_stride,       /* image stride */
                                                  clipped_img_area, /* blend area */
                                                  draw_unit,        /* target buffer, buffer width, buffer height, buffer stride */
                                                  draw_dsc)) {      /* opa, recolour_opa and colour */
        /*In the other cases every pixel need to be checked one-by-one*/
        transform_and_recolor(draw_unit, draw_dsc, decoder_dsc, sup, img_coords, clipped_img_area);

    }
}
static void radius_only(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc,
                        const lv_image_decoder_dsc_t * decoder_dsc,
                        const lv_area_t * img_coords, const lv_area_t * clipped_img_area)
{

    const lv_draw_buf_t * decoded = decoder_dsc->decoded;
    uint32_t img_stride = decoded->header.stride;
    lv_color_format_t cf = decoded->header.cf;
    lv_color_format_t cf_ori = cf;
    if(cf == LV_COLOR_FORMAT_RGB565A8) {
        cf = LV_COLOR_FORMAT_RGB565;
    }


    lv_draw_sw_blend_dsc_t blend_dsc;
    lv_memzero(&blend_dsc, sizeof(lv_draw_sw_blend_dsc_t));
    blend_dsc.opa = draw_dsc->opa;
    blend_dsc.blend_mode = draw_dsc->blend_mode;
    blend_dsc.src_stride = img_stride;
    blend_dsc.src_area = img_coords;
    blend_dsc.src_buf = decoded->data;
    blend_dsc.src_color_format = cf;

    lv_area_t blend_area = *clipped_img_area;
    blend_dsc.blend_area = &blend_area;
    int32_t y_last = blend_area.y2;
    blend_area.y2 = blend_area.y1;

    int32_t blend_w = lv_area_get_width(&blend_area);
    uint8_t * mask_buf = lv_malloc(blend_w);
    blend_dsc.mask_buf = mask_buf;
    blend_dsc.mask_area = &blend_area;
    blend_dsc.mask_stride = blend_w;

    if(cf == LV_COLOR_FORMAT_A8) {
        blend_dsc.src_buf = NULL;
        blend_dsc.color = draw_dsc->recolor;
    }

    lv_draw_sw_mask_radius_param_t mask_param;
    lv_draw_sw_mask_radius_init(&mask_param, &draw_dsc->image_area, draw_dsc->clip_radius, false);

    void * masks[2] = {0};
    masks[0] = &mask_param;

    int32_t image_h = lv_area_get_height(img_coords);
    while(blend_area.y1 <= y_last) {
        if(cf_ori == LV_COLOR_FORMAT_RGB565A8) {
            const uint8_t * mask_start = decoded->data + img_stride * image_h;
            int32_t y_ofs = blend_area.y1 - img_coords->y1;
            int32_t x_ofs = blend_area.x1 - img_coords->x1;
            lv_memcpy(mask_buf, mask_start + y_ofs * img_stride / 2  + x_ofs, blend_w);
        }
        else if(cf_ori == LV_COLOR_FORMAT_A8) {
            int32_t y_ofs = blend_area.y1 - img_coords->y1;
            int32_t x_ofs = blend_area.x1 - img_coords->x1;
            lv_memcpy(mask_buf, decoded->data + y_ofs * img_stride + x_ofs, blend_w);
        }
        else {
            lv_memset(mask_buf, 0xff, blend_w);

        }

        blend_dsc.mask_res = lv_draw_sw_mask_apply(masks, mask_buf, blend_area.x1, blend_area.y1, blend_w);

        if(cf_ori == LV_COLOR_FORMAT_RGB565A8 || cf_ori == LV_COLOR_FORMAT_A8) {
            blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;
        }

        /*Blend*/
        lv_draw_sw_blend(draw_unit, &blend_dsc);

        /*Go to the next area*/
        blend_area.y1 ++;
        blend_area.y2 ++;
    }
    lv_free(mask_buf);

}
static void recolor_only(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc,
                         const lv_image_decoder_dsc_t * decoder_dsc,
                         const lv_area_t * img_coords, const lv_area_t * clipped_img_area)
{
    lv_area_t blend_area = *clipped_img_area;

    const lv_draw_buf_t * decoded = decoder_dsc->decoded;
    uint32_t img_stride = decoded->header.stride;
    lv_color_format_t cf = decoded->header.cf;
    uint32_t px_size = lv_color_format_get_size(cf);
    int32_t src_h = lv_area_get_height(img_coords);
    int32_t blend_w = lv_area_get_width(&blend_area);
    int32_t blend_h = lv_area_get_height(&blend_area);
    uint8_t * tmp_buf;
    int32_t buf_h;
    uint32_t buf_stride = blend_w * px_size;
    if(buf_stride == 0) {
        buf_stride = 1;
    }
    buf_h = MAX_BUF_SIZE / buf_stride;
    if(buf_h > blend_h) buf_h = blend_h;
    tmp_buf = lv_malloc(buf_stride * buf_h);

    lv_draw_sw_blend_dsc_t blend_dsc;
    lv_memzero(&blend_dsc, sizeof(lv_draw_sw_blend_dsc_t));
    blend_dsc.opa = draw_dsc->opa;
    blend_dsc.blend_mode = draw_dsc->blend_mode;
    blend_dsc.src_stride = blend_w * px_size;
    blend_dsc.src_area = &blend_area;
    blend_dsc.blend_area = &blend_area;
    blend_dsc.src_buf = tmp_buf;
    blend_dsc.src_color_format = cf;
    if(cf == LV_COLOR_FORMAT_RGB565A8) {
        blend_dsc.mask_area = img_coords;
        blend_dsc.mask_buf = decoded->data + img_stride * src_h;
        blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;
        blend_dsc.src_color_format = LV_COLOR_FORMAT_RGB565;
        blend_dsc.mask_stride = img_stride / 2;
    }

    int32_t y_last = blend_area.y2;
    blend_area.y2 = blend_area.y1 + buf_h - 1;
    while(blend_area.y1 <= y_last) {
        lv_area_t relative_area;
        lv_area_copy(&relative_area, &blend_area);
        lv_area_move(&relative_area, -img_coords->x1, -img_coords->y1);
        recolor(relative_area, decoded->data, tmp_buf, img_stride, blend_dsc.src_color_format, draw_dsc);

        lv_draw_sw_blend(draw_unit, &blend_dsc);

        /*Go to the next area*/
        blend_area.y1 = blend_area.y2 + 1;
        blend_area.y2 = blend_area.y1 + buf_h - 1;
        if(blend_area.y2 > y_last) {
            blend_area.y2 = y_last;
        }
    }

    lv_free(tmp_buf);


}

static void transform_and_recolor(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc,
                                  const lv_image_decoder_dsc_t * decoder_dsc, lv_draw_image_sup_t * sup,
                                  const lv_area_t * img_coords, const lv_area_t * clipped_img_area)

{
    const lv_draw_buf_t * decoded = decoder_dsc->decoded;
    uint32_t img_stride = decoded->header.stride;
    lv_color_format_t cf = decoded->header.cf;
    lv_draw_sw_blend_dsc_t blend_dsc;
    lv_memzero(&blend_dsc, sizeof(lv_draw_sw_blend_dsc_t));
    blend_dsc.opa = draw_dsc->opa;
    blend_dsc.blend_mode = draw_dsc->blend_mode;

    lv_area_t blend_area = *clipped_img_area;
    blend_dsc.blend_area = &blend_area;

    int32_t src_w = lv_area_get_width(img_coords);
    int32_t src_h = lv_area_get_height(img_coords);
    int32_t blend_w = lv_area_get_width(&blend_area);
    int32_t blend_h = lv_area_get_height(&blend_area);

    bool do_recolor = draw_dsc->recolor_opa > LV_OPA_MIN;
    if(cf == LV_COLOR_FORMAT_L8 || cf == LV_COLOR_FORMAT_A8) {
        blend_dsc.color = draw_dsc->recolor;
        do_recolor = false;
    }

    lv_color_format_t cf_final = cf;
    if(cf_final == LV_COLOR_FORMAT_RGB888 || cf_final == LV_COLOR_FORMAT_XRGB8888) cf_final = LV_COLOR_FORMAT_ARGB8888;
    else if(cf_final == LV_COLOR_FORMAT_RGB565) cf_final = LV_COLOR_FORMAT_RGB565A8;
    else if(cf_final == LV_COLOR_FORMAT_L8) cf_final = LV_COLOR_FORMAT_AL88;

    uint8_t * transformed_buf;
    int32_t buf_h;
    if(cf_final == LV_COLOR_FORMAT_RGB565A8) {
        uint32_t buf_stride = blend_w * 3;
        buf_h = MAX_BUF_SIZE / buf_stride;
        if(buf_h > blend_h) buf_h = blend_h;
        transformed_buf = lv_malloc(buf_stride * buf_h);
    }
    else {
        uint32_t buf_stride = blend_w * lv_color_format_get_size(cf_final);
        buf_h = MAX_BUF_SIZE / buf_stride;
        if(buf_h > blend_h) buf_h = blend_h;
        transformed_buf = lv_malloc(buf_stride * buf_h);
    }
    LV_ASSERT_MALLOC(transformed_buf);

    blend_dsc.src_buf = transformed_buf;
    blend_dsc.src_color_format = cf_final;
    int32_t y_last = blend_area.y2;
    blend_area.y2 = blend_area.y1 + buf_h - 1;

    blend_dsc.src_area = &blend_area;
    const uint8_t * src_buf = decoded->data;
    if(cf_final == LV_COLOR_FORMAT_RGB565A8) {
        /*RGB565A8 images will blended as RGB565 + mask
         *Therefore the stride can be different. */
        blend_dsc.src_stride = blend_w * 2;
        blend_dsc.mask_area = &blend_area;
        blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;
        blend_dsc.mask_buf = transformed_buf + blend_w * 2 * buf_h;
        blend_dsc.src_color_format = LV_COLOR_FORMAT_RGB565;
    }
    else if(cf_final == LV_COLOR_FORMAT_A8) {
        blend_dsc.mask_buf = transformed_buf;
        blend_dsc.mask_stride = blend_w;
        blend_dsc.mask_area = &blend_area;
        blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;
        blend_dsc.src_buf = NULL;
    }
    else {
        blend_dsc.src_stride = blend_w * lv_color_format_get_size(cf_final);
    }

    while(blend_area.y1 <= y_last) {
        /*Apply transformations if any or separate the channels*/
        lv_area_t relative_area;
        lv_area_copy(&relative_area, &blend_area);
        lv_area_move(&relative_area, -img_coords->x1, -img_coords->y1);
        lv_draw_sw_transform(draw_unit, &relative_area, src_buf, src_w, src_h, img_stride,
                             draw_dsc, sup, cf, transformed_buf);

        if(do_recolor) {
            lv_area_t relative_area2;
            lv_area_copy(&relative_area2, &blend_area);
            lv_area_move(&relative_area2, -blend_area.x1, -blend_area.y1);
            recolor(relative_area2, transformed_buf, transformed_buf, blend_dsc.src_stride, cf_final, draw_dsc);
        }

        /*Blend*/
        lv_draw_sw_blend(draw_unit, &blend_dsc);

        /*Go to the next area*/
        blend_area.y1 = blend_area.y2 + 1;
        blend_area.y2 = blend_area.y1 + buf_h - 1;
        if(blend_area.y2 > y_last) {
            blend_area.y2 = y_last;
            if(cf_final == LV_COLOR_FORMAT_RGB565A8) {
                blend_dsc.mask_buf =  transformed_buf + blend_w * 2 * lv_area_get_height(&blend_area);
            }
        }
    }

    lv_free(transformed_buf);
}

static void recolor(lv_area_t relative_area, uint8_t * src_buf, uint8_t * dest_buf, int32_t src_stride,
                    lv_color_format_t cf, const lv_draw_image_dsc_t * draw_dsc)
{
    int32_t w = lv_area_get_width(&relative_area);
    int32_t h = lv_area_get_height(&relative_area);

    /*Apply recolor*/
    lv_color_t color = draw_dsc->recolor;
    lv_opa_t mix = draw_dsc->recolor_opa;
    lv_opa_t mix_inv = 255 - mix;

    if(cf == LV_COLOR_FORMAT_RGB565A8 || cf == LV_COLOR_FORMAT_RGB565) {
        if(LV_RESULT_INVALID == LV_DRAW_SW_RGB565_RECOLOR(dest_buf, blend_area, color, mix)) {
            const uint8_t * src_buf_tmp = src_buf + src_stride * relative_area.y1 + relative_area.x1 * 2;
            int32_t img_stride_px = src_stride / 2;

            uint16_t c_mult[3];
            c_mult[0] = (color.blue >> 3) * mix;
            c_mult[1] = (color.green >> 2) * mix;
            c_mult[2] = (color.red >> 3) * mix;
            uint16_t * buf16_src = (uint16_t *)src_buf_tmp;
            uint16_t * buf16_dest = (uint16_t *)dest_buf;

            int32_t y;
            for(y = 0; y < h; y++) {
                int32_t x;
                for(x = 0; x < w; x++) {
                    *buf16_dest = (((c_mult[2] + ((buf16_src[x] >> 11) & 0x1F) * mix_inv) << 3) & 0xF800) +
                                  (((c_mult[1] + ((buf16_src[x] >> 5) & 0x3F) * mix_inv) >> 3) & 0x07E0) +
                                  ((c_mult[0] + (buf16_src[x] & 0x1F) * mix_inv) >> 8);
                    buf16_dest++;
                }
                buf16_src += img_stride_px;
            }
        }
    }
    else if(cf == LV_COLOR_FORMAT_RGB888 || cf == LV_COLOR_FORMAT_XRGB8888 || cf == LV_COLOR_FORMAT_ARGB8888) {
        if(LV_RESULT_INVALID == LV_DRAW_SW_RGB888_RECOLOR(dest_buf, blend_area, color, mix, cf_final)) {
            uint32_t px_size = lv_color_format_get_size(cf);
            src_buf += src_stride * relative_area.y1 + relative_area.x1 * px_size;
            uint16_t c_mult[3];
            c_mult[0] = color.blue * mix;
            c_mult[1] = color.green * mix;
            c_mult[2] = color.red * mix;
            int32_t y;
            for(y = 0; y < h; y++) {
                int32_t x;
                for(x = 0; x < w; x++) {
                    dest_buf[0] = (c_mult[0] + (src_buf[0] * mix_inv)) >> 8;
                    dest_buf[1] = (c_mult[1] + (src_buf[1] * mix_inv)) >> 8;
                    dest_buf[2] = (c_mult[2] + (src_buf[2] * mix_inv)) >> 8;
                    if(cf == LV_COLOR_FORMAT_ARGB8888) dest_buf[3] = src_buf[3];
                    src_buf += px_size;
                    dest_buf += px_size;
                }
                src_buf += src_stride - w * px_size;
            }
        }
    }
}

static bool apply_mask(const lv_draw_image_dsc_t * draw_dsc)
{
    lv_layer_t * layer_to_draw = (lv_layer_t *)draw_dsc->src;
    lv_draw_buf_t * image_draw_buf = layer_to_draw->draw_buf;
    lv_image_decoder_dsc_t mask_decoder_dsc;
    lv_area_t mask_area;
    uint32_t mask_stride;

    lv_result_t decoder_res = lv_image_decoder_open(&mask_decoder_dsc, draw_dsc->bitmap_mask_src, NULL);
    if(decoder_res != LV_RESULT_OK || mask_decoder_dsc.decoded == NULL) {
        LV_LOG_WARN("Could open the mask. The mask is not applied.");
        return true;
    }

    if(mask_decoder_dsc.decoded->header.cf != LV_COLOR_FORMAT_A8 &&
       mask_decoder_dsc.decoded->header.cf != LV_COLOR_FORMAT_L8) {
        LV_LOG_WARN("The mask image is not A8/L8 format. The mask is not applied.");
        return true;

    }

    const lv_draw_buf_t * mask_draw_buf = mask_decoder_dsc.decoded;
    mask_stride = mask_draw_buf->header.stride;

    /*Align the mask to the center*/
    lv_area_t image_area;
    image_area = draw_dsc->image_area;  /*Use the whole image area for the alignment*/
    lv_area_set(&mask_area, 0, 0, mask_draw_buf->header.w - 1, mask_draw_buf->header.h - 1);
    lv_area_align(&image_area, &mask_area, LV_ALIGN_CENTER, 0, 0);

    image_area =
        layer_to_draw->buf_area; /*The image can be smaller if only a part was rendered. Use this are during rendering*/

    /*Only the intersection of the mask and image needs to be rendered
     *If if there is no intersection there is nothing to render as the image is out of the mask.*/
    lv_area_t masked_area;
    if(!lv_area_intersect(&masked_area, &mask_area, &image_area)) return false;

    /*Clear the sides if any*/
    lv_area_t side_area = {0};
    /*Top*/
    side_area.x2 = layer_to_draw->draw_buf->header.w - 1;
    side_area.y2 = masked_area.y1 - 1 - image_area.y1;
    lv_draw_buf_clear(layer_to_draw->draw_buf, &side_area);

    /*Bottom*/
    side_area.y1 = masked_area.y2 + 1 - image_area.y1;
    side_area.y2 = layer_to_draw->draw_buf->header.h - 1;
    lv_draw_buf_clear(layer_to_draw->draw_buf, &side_area);

    /*Left*/
    side_area.y1 = 0;
    side_area.x1 = 0;
    side_area.x2 = masked_area.x1 - 1 - image_area.x1;
    lv_draw_buf_clear(layer_to_draw->draw_buf, &side_area);

    /*Right*/
    side_area.x1 = masked_area.x2 + 1 - image_area.x1;
    side_area.x2 = layer_to_draw->draw_buf->header.w - 1;
    lv_draw_buf_clear(layer_to_draw->draw_buf, &side_area);

    /*Seek to the first of the image and mask on the masked area*/
    uint8_t * img_start = lv_draw_buf_goto_xy(image_draw_buf,
                                              masked_area.x1 - image_area.x1,
                                              masked_area.y1 - image_area.y1);
    uint8_t * mask_start = lv_draw_buf_goto_xy(mask_decoder_dsc.decoded,
                                               masked_area.x1 - mask_area.x1,
                                               masked_area.y1 - mask_area.y1);

    int32_t h = lv_area_get_height(&masked_area);
    int32_t w = lv_area_get_width(&masked_area);

    int32_t y;
    for(y = 0; y < h; y++) {
        int32_t x;
        for(x = 0; x < w; x++) {
            img_start[x * 4 + 3] = LV_OPA_MIX2(mask_start[x], img_start[x * 4 + 3]);
        }
        img_start += layer_to_draw->draw_buf->header.stride;
        mask_start += mask_stride;
    }

    return true;
}

#endif /*LV_USE_DRAW_SW*/
