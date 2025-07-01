/**
 * @file lv_draw_dma2d_img.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_dma2d_private.h"
#if LV_USE_DRAW_DMA2D

#include "../lv_draw_image_private.h"
#include "../lv_image_decoder_private.h"
#include "../../misc/lv_area_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void lv_draw_dma2d_opaque_image_core(lv_draw_task_t * t, const lv_draw_image_dsc_t * draw_dsc,
                                            const lv_image_decoder_dsc_t * decoder_dsc, lv_draw_image_sup_t * sup,
                                            const lv_area_t * img_coords, const lv_area_t * clipped_img_area);

static void lv_draw_dma2d_image_core(lv_draw_task_t * t, const lv_draw_image_dsc_t * draw_dsc,
                                     const lv_image_decoder_dsc_t * decoder_dsc, lv_draw_image_sup_t * sup,
                                     const lv_area_t * img_coords, const lv_area_t * clipped_img_area);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_dma2d_opaque_image(lv_draw_task_t * t, const lv_draw_image_dsc_t * draw_dsc,
                                const lv_area_t * coords)
{
    if(!draw_dsc->tile) {
        lv_draw_image_normal_helper(t, draw_dsc, coords, lv_draw_dma2d_opaque_image_core);
    }
    else {
        lv_draw_image_tiled_helper(t, draw_dsc, coords, lv_draw_dma2d_opaque_image_core);
    }
}

void lv_draw_dma2d_image(lv_draw_task_t * t, const lv_draw_image_dsc_t * draw_dsc,
                         const lv_area_t * coords)
{
    if(!draw_dsc->tile) {
        lv_draw_image_normal_helper(t, draw_dsc, coords, lv_draw_dma2d_image_core);
    }
    else {
        lv_draw_image_tiled_helper(t, draw_dsc, coords, lv_draw_dma2d_image_core);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_draw_dma2d_opaque_image_core(lv_draw_task_t * t, const lv_draw_image_dsc_t * draw_dsc,
                                            const lv_image_decoder_dsc_t * decoder_dsc, lv_draw_image_sup_t * sup,
                                            const lv_area_t * img_coords, const lv_area_t * clipped_img_area)
{
    LV_UNUSED(sup);
    LV_UNUSED(img_coords);

    lv_layer_t * layer = t->target_layer;

    void * dest_first_pixel = lv_draw_layer_go_to_xy(layer,
                                                     clipped_img_area->x1 - layer->buf_area.x1,
                                                     clipped_img_area->y1 - layer->buf_area.y1);
    int32_t dest_stride = lv_draw_buf_width_to_stride(lv_area_get_width(&layer->buf_area), layer->color_format);

    int32_t w = lv_area_get_width(clipped_img_area);
    int32_t h = lv_area_get_height(clipped_img_area);

    lv_color_format_t output_cf = layer->color_format;
    uint32_t output_cf_size = lv_color_format_get_size(output_cf);
    lv_draw_dma2d_output_cf_t output_cf_dma2d = lv_draw_dma2d_cf_to_dma2d_output_cf(output_cf);

    const lv_draw_buf_t * decoded = decoder_dsc->decoded;
    const uint8_t * src_buf = decoded->data;
    uint32_t image_stride = decoded->header.stride;
    lv_color_format_t image_cf = decoded->header.cf;
    lv_draw_dma2d_fgbg_cf_t image_cf_dma2d = (lv_draw_dma2d_fgbg_cf_t) lv_draw_dma2d_cf_to_dma2d_output_cf(image_cf);
    uint32_t image_cf_size = LV_COLOR_FORMAT_GET_SIZE(image_cf);
    if(image_stride == 0) image_stride = image_cf_size * decoded->header.w;

#if LV_DRAW_DMA2D_CACHE
    lv_draw_dma2d_cache_area_t dest_area = {
        .first_byte = dest_first_pixel,
        .width_bytes = w * output_cf_size,
        .height = h,
        .stride = dest_stride
    };
    lv_draw_dma2d_unit_t * u = (lv_draw_dma2d_unit_t *) t->draw_unit;
    lv_memcpy(&u->writing_area, &dest_area, sizeof(lv_draw_dma2d_cache_area_t));
    if(lv_color_format_has_alpha(image_cf)) {
        /* make sure the background area DMA2D is blending is up-to-date in main memory */
        lv_draw_dma2d_clean_cache(&dest_area);
    }
#endif

    const void * image_first_byte = src_buf
                                    + (image_stride * (clipped_img_area->y1 - draw_dsc->image_area.y1))
                                    + (image_cf_size * (clipped_img_area->x1 - draw_dsc->image_area.x1));

#if LV_DRAW_DMA2D_CACHE
    lv_draw_dma2d_cache_area_t src_area = {
        .first_byte = image_first_byte,
        .width_bytes = w * image_cf_size,
        .height = h,
        .stride = image_stride
    };
    /* make sure the image area is up-to-date in main memory for DMA2D */
    lv_draw_dma2d_clean_cache(&src_area);
#endif

    uint32_t output_offset = (dest_stride / output_cf_size) - w;
    lv_draw_dma2d_configuration_t conf = {
        .mode = LV_DRAW_DMA2D_MODE_MEMORY_TO_MEMORY_WITH_PFC,
        .w = w,
        .h = h,

        .output_address = dest_first_pixel,
        .output_offset = output_offset,
        .output_cf = output_cf_dma2d,

        .fg_address = image_first_byte,
        .fg_offset = (image_stride / image_cf_size) - w,
        .fg_cf = image_cf_dma2d
    };

    /* only process the background if the image might be transparent */
    if(lv_color_format_has_alpha(image_cf)) {
        conf.mode = LV_DRAW_DMA2D_MODE_MEMORY_TO_MEMORY_WITH_BLENDING;

        conf.bg_address = dest_first_pixel;
        conf.bg_offset = output_offset;
        conf.bg_cf = output_cf_dma2d;
    }

    /* Alpha channel should be treated as 0xFF if the cf is XRGB */
    if(image_cf == LV_COLOR_FORMAT_XRGB8888) {
        conf.fg_alpha_mode = LV_DRAW_DMA2D_ALPHA_MODE_REPLACE_ALPHA_CHANNEL;
        conf.fg_alpha = 0xff;
    }
    if(output_cf == LV_COLOR_FORMAT_XRGB8888) {
        conf.bg_alpha_mode = LV_DRAW_DMA2D_ALPHA_MODE_REPLACE_ALPHA_CHANNEL;
        conf.bg_alpha = 0xff;
    }

    lv_draw_dma2d_configure_and_start_transfer(&conf);
}

static void lv_draw_dma2d_image_core(lv_draw_task_t * t, const lv_draw_image_dsc_t * draw_dsc,
                                     const lv_image_decoder_dsc_t * decoder_dsc, lv_draw_image_sup_t * sup,
                                     const lv_area_t * img_coords, const lv_area_t * clipped_img_area)
{
    LV_UNUSED(sup);
    LV_UNUSED(img_coords);

    lv_layer_t * layer = t->target_layer;

    void * dest_first_pixel = lv_draw_layer_go_to_xy(layer,
                                                     clipped_img_area->x1 - layer->buf_area.x1,
                                                     clipped_img_area->y1 - layer->buf_area.y1);
    int32_t dest_stride = lv_draw_buf_width_to_stride(lv_area_get_width(&layer->buf_area), layer->color_format);

    int32_t w = lv_area_get_width(clipped_img_area);
    int32_t h = lv_area_get_height(clipped_img_area);

    lv_color_format_t output_cf = layer->color_format;
    uint32_t output_cf_size = lv_color_format_get_size(output_cf);
    lv_draw_dma2d_output_cf_t output_cf_dma2d = lv_draw_dma2d_cf_to_dma2d_output_cf(output_cf);

    const lv_draw_buf_t * decoded = decoder_dsc->decoded;
    const uint8_t * src_buf = decoded->data;
    uint32_t image_stride = decoded->header.stride;
    lv_color_format_t image_cf = decoded->header.cf;
    lv_opa_t opa = draw_dsc->opa;
    lv_draw_dma2d_fgbg_cf_t image_cf_dma2d = (lv_draw_dma2d_fgbg_cf_t) lv_draw_dma2d_cf_to_dma2d_output_cf(image_cf);
    uint32_t image_cf_size = LV_COLOR_FORMAT_GET_SIZE(image_cf);
    if(image_stride == 0) image_stride = image_cf_size * decoded->header.w;

#if LV_DRAW_DMA2D_CACHE
    lv_draw_dma2d_cache_area_t dest_area = {
        .first_byte = dest_first_pixel,
        .width_bytes = w * output_cf_size,
        .height = h,
        .stride = dest_stride
    };
    lv_draw_dma2d_unit_t * u = (lv_draw_dma2d_unit_t *) t->draw_unit;
    lv_memcpy(&u->writing_area, &dest_area, sizeof(lv_draw_dma2d_cache_area_t));
    /* make sure the background area DMA2D is blending is up-to-date in main memory */
    lv_draw_dma2d_clean_cache(&dest_area);
#endif

    const void * image_first_byte = src_buf
                                    + (image_stride * (clipped_img_area->y1 - draw_dsc->image_area.y1))
                                    + (image_cf_size * (clipped_img_area->x1 - draw_dsc->image_area.x1));

#if LV_DRAW_DMA2D_CACHE
    lv_draw_dma2d_cache_area_t src_area = {
        .first_byte = image_first_byte,
        .width_bytes = w * image_cf_size,
        .height = h,
        .stride = image_stride
    };
    /* make sure the image area is up-to-date in main memory for DMA2D */
    lv_draw_dma2d_clean_cache(&src_area);
#endif

    uint32_t output_offset = (dest_stride / output_cf_size) - w;
    lv_draw_dma2d_configuration_t conf = {
        .mode = LV_DRAW_DMA2D_MODE_MEMORY_TO_MEMORY_WITH_BLENDING,
        .w = w,
        .h = h,

        .output_address = dest_first_pixel,
        .output_offset = output_offset,
        .output_cf = output_cf_dma2d,

        .fg_address = image_first_byte,
        .fg_offset = (image_stride / image_cf_size) - w,
        .fg_cf = image_cf_dma2d,
        .fg_alpha_mode = LV_DRAW_DMA2D_ALPHA_MODE_MULTIPLY_IMAGE_ALPHA_CHANNEL,
        .fg_alpha = opa,

        .bg_address = dest_first_pixel,
        .bg_offset = output_offset,
        .bg_cf = output_cf_dma2d,
    };

    /* Alpha channel should be treated as 0xFF if the cf is XRGB */
    if(image_cf == LV_COLOR_FORMAT_XRGB8888) {
        conf.fg_alpha_mode = LV_DRAW_DMA2D_ALPHA_MODE_REPLACE_ALPHA_CHANNEL;
    }
    if(output_cf == LV_COLOR_FORMAT_XRGB8888) {
        conf.bg_alpha_mode = LV_DRAW_DMA2D_ALPHA_MODE_REPLACE_ALPHA_CHANNEL;
        conf.bg_alpha = 0xff;
    }

    lv_draw_dma2d_configure_and_start_transfer(&conf);
}

#endif /*LV_USE_DRAW_DMA2D*/
