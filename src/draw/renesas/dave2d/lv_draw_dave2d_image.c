#include "lv_draw_dave2d.h"
#if LV_USE_DRAW_DAVE2D

#include "../../sw/lv_draw_sw.h"

#include "../../../display/lv_display.h"
#include "../../../display/lv_display_private.h"
#include "../../../misc/lv_log.h"
#include "../../../core/lv_refr.h"
#include "../../../stdlib/lv_mem.h"
#include "../../../misc/lv_cache.h"
#include "../../../misc/lv_math.h"
#include "../../../misc/lv_color.h"
#include "../../../stdlib/lv_string.h"
#include "../../../core/lv_global.h"



/*********************
 *      DEFINES
 *********************/
#define MAX_BUF_SIZE (uint32_t) (4 * lv_display_get_horizontal_resolution(_lv_refr_get_disp_refreshing()) * lv_color_format_get_size(lv_display_get_color_format(_lv_refr_get_disp_refreshing())))


static void dave2d_img_draw_normal(lv_draw_dave2d_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc,
                                   const lv_area_t * coords);

static void dave2d_img_draw_tiled(lv_draw_dave2d_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc,
                                  const lv_area_t * coords);

static void dave2d_img_decode_and_draw(lv_draw_dave2d_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc,
                                       lv_image_decoder_dsc_t * decoder_dsc,
                                       const lv_area_t * img_area, const lv_area_t * clipped_img_area);

static void dave2d_img_draw_core(lv_draw_dave2d_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc,
                                 const lv_image_decoder_dsc_t * decoder_dsc, lv_draw_image_sup_t * sup,
                                 const lv_area_t * img_coords, const lv_area_t * clipped_img_area);

static void sw_fallback_img_draw_core(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc,
                                      const lv_image_decoder_dsc_t * decoder_dsc, lv_draw_image_sup_t * sup,
                                      const lv_area_t * img_coords, const lv_area_t * clipped_img_area);

void lv_draw_dave2d_image(lv_draw_dave2d_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc,
                          const lv_area_t * coords)
{
    if(!draw_dsc->tile) {
        dave2d_img_draw_normal(draw_unit, draw_dsc, coords);
    }
    else {
        dave2d_img_draw_tiled(draw_unit, draw_dsc, coords);
    }
}

static void dave2d_img_draw_normal(lv_draw_dave2d_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc,
                                   const lv_area_t * coords)
{
    lv_area_t draw_area;
    lv_area_copy(&draw_area, coords);
    if(draw_dsc->rotation || draw_dsc->scale_x != LV_SCALE_NONE || draw_dsc->scale_y != LV_SCALE_NONE) {
        int32_t w = lv_area_get_width(coords);
        int32_t h = lv_area_get_height(coords);

        _lv_image_buf_get_transformed_area(&draw_area, w, h, draw_dsc->rotation, draw_dsc->scale_x, draw_dsc->scale_y,
                                           &draw_dsc->pivot);

        draw_area.x1 += coords->x1;
        draw_area.y1 += coords->y1;
        draw_area.x2 += coords->x1;
        draw_area.y2 += coords->y1;
    }

    lv_area_t clipped_img_area;
    if(!_lv_area_intersect(&clipped_img_area, &draw_area, draw_unit->base_unit.clip_area)) {
        return;
    }

    lv_image_decoder_dsc_t decoder_dsc;
    lv_result_t res = lv_image_decoder_open(&decoder_dsc, draw_dsc->src, NULL);
    if(res != LV_RESULT_OK) {
        LV_LOG_ERROR("Failed to open image");
        return;
    }

    dave2d_img_decode_and_draw(draw_unit, draw_dsc, &decoder_dsc, coords, &clipped_img_area);

    lv_image_decoder_close(&decoder_dsc);
}

static void dave2d_img_draw_tiled(lv_draw_dave2d_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc,
                                  const lv_area_t * coords)
{
    lv_image_decoder_dsc_t decoder_dsc;
    lv_result_t res = lv_image_decoder_open(&decoder_dsc, draw_dsc->src, NULL);
    if(res != LV_RESULT_OK) {
        LV_LOG_ERROR("Failed to open image");
        return;
    }

    int32_t img_w = lv_area_get_width(coords);
    int32_t img_h = lv_area_get_height(coords);

    lv_area_t tile_area = *coords;
    int32_t tile_x_start = tile_area.x1;

    while(tile_area.y1 < draw_unit->base_unit.clip_area->y2) {
        while(tile_area.x1 < draw_unit->base_unit.clip_area->x2) {

            lv_area_t clipped_img_area;
            if(_lv_area_intersect(&clipped_img_area, &tile_area, draw_unit->base_unit.clip_area)) {
                dave2d_img_decode_and_draw(draw_unit, draw_dsc, &decoder_dsc, &tile_area, &clipped_img_area);
            }

            tile_area.x1 += img_w;
            tile_area.x2 += img_w;
        }

        tile_area.y1 += img_h;
        tile_area.y2 += img_h;
        tile_area.x1 = tile_x_start;
        tile_area.x2 = tile_x_start + img_w - 1;
    }

    lv_image_decoder_close(&decoder_dsc);
}

static void dave2d_img_decode_and_draw(lv_draw_dave2d_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc,
                                       lv_image_decoder_dsc_t * decoder_dsc,
                                       const lv_area_t * img_area, const lv_area_t * clipped_img_area)
{
    lv_draw_image_sup_t sup;
    sup.alpha_color = draw_dsc->recolor;
    sup.palette = decoder_dsc->palette;
    sup.palette_size = decoder_dsc->palette_size;

    /*The whole image is available, just draw it*/
    if(decoder_dsc->img_data) {
        dave2d_img_draw_core(draw_unit, draw_dsc, decoder_dsc, &sup, img_area, clipped_img_area);
    }
    /*Draw in smaller pieces*/
    else {
        lv_area_t relative_full_area_to_decode = *clipped_img_area;
        lv_area_move(&relative_full_area_to_decode, -img_area->x1, -img_area->y1);

        lv_area_t relative_decoded_area;
        relative_decoded_area.x1 = LV_COORD_MIN;
        relative_decoded_area.y1 = LV_COORD_MIN;
        relative_decoded_area.x2 = LV_COORD_MIN;
        relative_decoded_area.y2 = LV_COORD_MIN;
        lv_result_t res = LV_RESULT_OK;

        while(res == LV_RESULT_OK) {
            res = lv_image_decoder_get_area(decoder_dsc, &relative_full_area_to_decode, &relative_decoded_area);

            lv_area_t absolute_decoded_area = relative_decoded_area;
            lv_area_move(&absolute_decoded_area, img_area->x1, img_area->y1);
            if(res == LV_RESULT_OK) {
                /*Limit draw area to the current decoded area and draw the image*/
                lv_area_t clipped_img_area_sub;
                if(_lv_area_intersect(&clipped_img_area_sub, clipped_img_area, &absolute_decoded_area)) {
                    dave2d_img_draw_core(draw_unit, draw_dsc, decoder_dsc, &sup,
                                         &absolute_decoded_area, &clipped_img_area_sub);
                }
            }
        }
    }
}

static void dave2d_img_draw_core(lv_draw_dave2d_unit_t * u, const lv_draw_image_dsc_t * draw_dsc,
                                 const lv_image_decoder_dsc_t * decoder_dsc, lv_draw_image_sup_t * sup,
                                 const lv_area_t * img_coords, const lv_area_t * clipped_img_area)
{
    bool transformed = draw_dsc->rotation != 0 || draw_dsc->scale_x != LV_SCALE_NONE ||
                       draw_dsc->scale_y != LV_SCALE_NONE ? true : false;

    const uint8_t * src_buf = decoder_dsc->img_data;
    const lv_image_header_t * header = &decoder_dsc->header;
    lv_area_t buffer_area;
    lv_area_t draw_area;
    lv_area_t clipped_area;
    int32_t x;
    int32_t y;
    d2_u8 a_texture_op = d2_to_one;
    d2_u8 r_texture_op = d2_to_copy;
    d2_u8 g_texture_op = d2_to_copy;
    d2_u8 b_texture_op = d2_to_copy;
    d2_u8 current_fill_mode;
    d2_u32 src_blend_mode;
    d2_u32 dst_blend_mode;


    if(LV_COLOR_FORMAT_RGB565A8 == header->cf) {
        /* Colour format not support by Dave2D */
        sw_fallback_img_draw_core(&u->base_unit, draw_dsc, decoder_dsc, sup, img_coords, clipped_img_area);
        return;
    }

#if LV_USE_OS
    lv_result_t  status;
    status = lv_mutex_lock(u->pd2Mutex);
    if(LV_RESULT_OK != status) {
        __BKPT(0);
    }
#endif

    buffer_area = u->base_unit.target_layer->buf_area;
    draw_area = *img_coords;
    clipped_area = *clipped_img_area;

    x = 0 - u->base_unit.target_layer->buf_area.x1;
    y = 0 - u->base_unit.target_layer->buf_area.y1;

    lv_area_move(&draw_area, x, y);
    lv_area_move(&buffer_area, x, y);
    lv_area_move(&clipped_area, x, y);

    //
    // Generate render operations
    //
#if D2_RENDER_EACH_OPERATION
    d2_selectrenderbuffer(u->d2_handle, u->renderbuffer);
#endif

    current_fill_mode = d2_getfillmode(u->d2_handle);
    a_texture_op      = d2_gettextureoperationa(u->d2_handle);
    r_texture_op      = d2_gettextureoperationr(u->d2_handle);
    g_texture_op      = d2_gettextureoperationg(u->d2_handle);
    b_texture_op      = d2_gettextureoperationb(u->d2_handle);
    src_blend_mode    = d2_getblendmodesrc(u->d2_handle);
    dst_blend_mode    = d2_getblendmodedst(u->d2_handle);

    d2_framebuffer(u->d2_handle,
                   u->base_unit.target_layer->buf,
                   (d2_s32)u->base_unit.target_layer->buf_stride / lv_color_format_get_size(u->base_unit.target_layer->color_format),
                   (d2_u32)lv_area_get_width(&buffer_area),
                   (d2_u32)lv_area_get_height(&buffer_area),
                   lv_draw_dave2d_cf_fb_get());

    d2_cliprect(u->d2_handle, (d2_border)clipped_area.x1, (d2_border)clipped_area.y1, (d2_border)clipped_area.x2,
                (d2_border)clipped_area.y2);

#if defined(RENESAS_CORTEX_M85)
#if (BSP_CFG_DCACHE_ENABLED)
    d1_cacheblockflush(u->d2_handle, 0, src_buf,
                       decoder_dsc->header.stride * decoder_dsc->header.h); //Stride is in bytes, not pixels/texels
#endif
#endif

    d2_settexopparam(u->d2_handle, d2_cc_alpha, draw_dsc->opa, 0);

    if(LV_COLOR_FORMAT_RGB565 == header->cf) {
        d2_settextureoperation(u->d2_handle, d2_to_replace, d2_to_copy, d2_to_copy, d2_to_copy);
    }
    else { //Formats with an alpha channel,
        d2_settextureoperation(u->d2_handle, d2_to_multiply, d2_to_copy, d2_to_copy, d2_to_copy);
    }

    if(LV_BLEND_MODE_NORMAL == draw_dsc->blend_mode) { /**< Simply mix according to the opacity value*/
        d2_setblendmode(u->d2_handle, d2_bm_alpha, d2_bm_one_minus_alpha);  //direct linear blend
    }
    else if(LV_BLEND_MODE_ADDITIVE == draw_dsc->blend_mode) { /**< Add the respective color channels*/
        /* TODO */
        d2_setblendmode(u->d2_handle, d2_bm_alpha, d2_bm_one);  //Additive blending
    }
    else if(LV_BLEND_MODE_SUBTRACTIVE == draw_dsc->blend_mode) { /**< Subtract the foreground from the background*/
        /* TODO */
        __NOP();
    }
    else { //LV_BLEND_MODE_MULTIPLY,   /**< Multiply the foreground and background*/
        /* TODO */
        __NOP();
    }

    lv_point_t p[4] = { //Points in clockwise order
        {0, 0},
        {decoder_dsc->header.w - 1, 0},
        {decoder_dsc->header.w - 1, decoder_dsc->header.h - 1},
        {0, decoder_dsc->header.h - 1},
    };

    d2_settexture(u->d2_handle, (void *)src_buf,
                  (d2_s32)(decoder_dsc->header.stride / lv_color_format_get_size(header->cf)),
                  decoder_dsc->header.w,  decoder_dsc->header.h, lv_draw_dave2d_lv_colour_fmt_to_d2_fmt(header->cf));

    d2_settexturemode(u->d2_handle, d2_tm_filter);
    d2_setfillmode(u->d2_handle, d2_fm_texture);

    d2_s32 dxu = D2_FIX16(1);
    d2_s32 dxv = D2_FIX16(0);
    d2_s32 dyu = D2_FIX16(0);
    d2_s32 dyv = D2_FIX16(1);

    if(transformed) {
        lv_point_transform(&p[0], draw_dsc->rotation, draw_dsc->scale_x, draw_dsc->scale_y, &draw_dsc->pivot, true);
        lv_point_transform(&p[1], draw_dsc->rotation, draw_dsc->scale_x, draw_dsc->scale_y, &draw_dsc->pivot, true);
        lv_point_transform(&p[2], draw_dsc->rotation, draw_dsc->scale_x, draw_dsc->scale_y, &draw_dsc->pivot, true);
        lv_point_transform(&p[3], draw_dsc->rotation, draw_dsc->scale_x, draw_dsc->scale_y, &draw_dsc->pivot, true);

        int32_t sin_xv = 0; //0 degrees
        int32_t cos_xu = (1 << 15);
        int32_t sin_yv = (1 << 15); //90 degress
        int32_t cos_yu = 0;

        int32_t angle_limited = draw_dsc->rotation;
        if(angle_limited > 3600) angle_limited -= 3600;
        if(angle_limited < 0) angle_limited += 3600;

        int32_t angle_low = angle_limited / 10;

        if(0 != angle_low) {
            sin_xv = lv_trigo_sin((int16_t)angle_low);
            cos_xu = lv_trigo_cos((int16_t)angle_low);
            sin_yv = lv_trigo_sin((int16_t)angle_low + 90);
            cos_yu = lv_trigo_cos((int16_t)angle_low + 90);
        }

        /* LV_TRIGO_SHIFT is 15, so only need to shift by 1 to get 16:16 fixed point */
        dxu = (d2_s32)((1 << 1) *
                       cos_xu); /* TODO - Need to handle scaling of the texture based on draw_dsc->scale_x, draw_dsc->scale_y, currently no scaling it is 1:1 */
        dxv = (d2_s32)((1 << 1) * sin_xv);
        dyu = (d2_s32)((1 << 1) * cos_yu);
        dyv = (d2_s32)((1 << 1) * sin_yv);
    }

    p[0].x += draw_area.x1;
    p[0].y += draw_area.y1;
    p[1].x += draw_area.x1;
    p[1].y += draw_area.y1;
    p[2].x += draw_area.x1;
    p[2].y += draw_area.y1;
    p[3].x += draw_area.x1;
    p[3].y += draw_area.y1;

    d2_settexturemapping(u->d2_handle, D2_FIX4(p[0].x), D2_FIX4(p[0].y), D2_FIX16(0), D2_FIX16(0), dxu, dxv, dyu,  dyv);

    d2_renderquad(u->d2_handle,
                  (d2_point)D2_FIX4(p[0].x),
                  (d2_point)D2_FIX4(p[0].y),
                  (d2_point)D2_FIX4(p[1].x),
                  (d2_point)D2_FIX4(p[1].y),
                  (d2_point)D2_FIX4(p[2].x),
                  (d2_point)D2_FIX4(p[2].y),
                  (d2_point)D2_FIX4(p[3].x),
                  (d2_point)D2_FIX4(p[3].y),
                  0);

    //
    // Execute render operations
    //
#if D2_RENDER_EACH_OPERATION
    d2_executerenderbuffer(u->d2_handle, u->renderbuffer, 0);
    d2_flushframe(u->d2_handle);
#endif

    d2_setfillmode(u->d2_handle, current_fill_mode);
    d2_settextureoperation(u->d2_handle, a_texture_op, r_texture_op, g_texture_op, b_texture_op);
    d2_setblendmode(u->d2_handle, src_blend_mode, dst_blend_mode);

#if LV_USE_OS
    status = lv_mutex_unlock(u->pd2Mutex);
    if(LV_RESULT_OK != status) {
        __BKPT(0);
    }
#endif

}

static void sw_fallback_img_draw_core(lv_draw_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc,
                                      const lv_image_decoder_dsc_t * decoder_dsc, lv_draw_image_sup_t * sup,
                                      const lv_area_t * img_coords, const lv_area_t * clipped_img_area)
{
    bool transformed = draw_dsc->rotation != 0 || draw_dsc->scale_x != LV_SCALE_NONE ||
                       draw_dsc->scale_y != LV_SCALE_NONE ? true : false;

    lv_draw_sw_blend_dsc_t blend_dsc;
    const uint8_t * src_buf = decoder_dsc->img_data;
    const lv_image_header_t * header = &decoder_dsc->header;
    uint32_t img_stride = header->stride;
    lv_color_format_t cf = header->cf;

    cf = LV_COLOR_FORMAT_IS_INDEXED(cf) ? LV_COLOR_FORMAT_ARGB8888 : cf,

    lv_memzero(&blend_dsc, sizeof(lv_draw_sw_blend_dsc_t));
    blend_dsc.opa = draw_dsc->opa;
    blend_dsc.blend_mode = draw_dsc->blend_mode;
    blend_dsc.src_stride = img_stride;

    if(!transformed && cf == LV_COLOR_FORMAT_A8) {
        lv_area_t clipped_coords;
        if(!_lv_area_intersect(&clipped_coords, img_coords, draw_unit->clip_area)) return;

        blend_dsc.mask_buf = (lv_opa_t *)src_buf;
        blend_dsc.mask_area = img_coords;
        blend_dsc.mask_stride = img_stride;
        blend_dsc.src_buf = NULL;
        blend_dsc.color = draw_dsc->recolor;
        blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;

        blend_dsc.blend_area = img_coords;
        lv_draw_sw_blend(draw_unit, &blend_dsc);
    }
    else if(!transformed && cf == LV_COLOR_FORMAT_RGB565A8 && draw_dsc->recolor_opa <= LV_OPA_MIN) {
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
    /*The simplest case just copy the pixels into the draw_buf. Blending will convert the colors if needed*/
    else if(!transformed && draw_dsc->recolor_opa <= LV_OPA_MIN) {
        blend_dsc.src_area = img_coords;
        blend_dsc.src_buf = src_buf;
        blend_dsc.blend_area = img_coords;
        blend_dsc.src_color_format = cf;
        lv_draw_sw_blend(draw_unit, &blend_dsc);
    }
    /*In the other cases every pixel need to be checked one-by-one*/
    else {
        lv_area_t blend_area = *clipped_img_area;
        blend_dsc.blend_area = &blend_area;

        int32_t src_w = lv_area_get_width(img_coords);
        int32_t src_h = lv_area_get_height(img_coords);
        int32_t blend_w = lv_area_get_width(&blend_area);
        int32_t blend_h = lv_area_get_height(&blend_area);

        lv_color_format_t cf_final = cf;
        if(transformed) {
            if(cf == LV_COLOR_FORMAT_RGB888 || cf == LV_COLOR_FORMAT_XRGB8888) cf_final = LV_COLOR_FORMAT_ARGB8888;
            else if(cf == LV_COLOR_FORMAT_RGB565) cf_final = LV_COLOR_FORMAT_RGB565A8;
        }
        uint8_t * tmp_buf;
        uint32_t px_size = lv_color_format_get_size(cf_final);
        int32_t buf_h;
        if(cf_final == LV_COLOR_FORMAT_RGB565A8) {
            uint32_t buf_stride = blend_w * 3;
            buf_h = MAX_BUF_SIZE / buf_stride;
            if(buf_h > blend_h) buf_h = blend_h;
            tmp_buf = lv_malloc(buf_stride * buf_h);
        }
        else {
            uint32_t buf_stride = blend_w * lv_color_format_get_size(cf_final);
            buf_h = MAX_BUF_SIZE / buf_stride;
            if(buf_h > blend_h) buf_h = blend_h;
            tmp_buf = lv_malloc(buf_stride * buf_h);
        }

        blend_dsc.src_buf = tmp_buf;
        blend_dsc.src_color_format = cf_final;
        int32_t y_last = blend_area.y2;
        blend_area.y2 = blend_area.y1 + buf_h - 1;

        blend_dsc.src_area = &blend_area;
        if(cf_final == LV_COLOR_FORMAT_RGB565A8) {
            /*RGB565A8 images will blended as RGB565 + mask
             *Therefore the stride can be different. */
            blend_dsc.src_stride = blend_w * 2;
            blend_dsc.mask_buf =  tmp_buf + blend_w * 2 * buf_h;
            blend_dsc.mask_stride = blend_w;
            blend_dsc.mask_area = &blend_area;
            blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;
            blend_dsc.src_color_format = LV_COLOR_FORMAT_RGB565;
        }
        else if(cf_final == LV_COLOR_FORMAT_A8) {
            blend_dsc.mask_buf = blend_dsc.src_buf;
            blend_dsc.mask_stride = blend_w;
            blend_dsc.mask_area = &blend_area;
            blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;
            blend_dsc.color = draw_dsc->recolor;
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
            if(transformed) {
                lv_draw_sw_transform(draw_unit, &relative_area, src_buf, src_w, src_h, img_stride,
                                     draw_dsc, sup, cf, tmp_buf);
            }
            else if(draw_dsc->recolor_opa >= LV_OPA_MIN) {
                int32_t h = lv_area_get_height(&relative_area);
                if(cf_final == LV_COLOR_FORMAT_RGB565A8) {
                    uint32_t stride_px = img_stride / 2;
                    const uint8_t * rgb_src_buf = src_buf + stride_px * 2 * relative_area.y1 + relative_area.x1 * 2;
                    const uint8_t * a_src_buf = src_buf + stride_px * 2 * src_h + stride_px * relative_area.y1 +
                                                relative_area.x1;
                    uint8_t * rgb_dest_buf = tmp_buf;
                    uint8_t * a_dest_buf = (uint8_t *)blend_dsc.mask_buf;
                    int32_t i;
                    for(i = 0; i < h; i++) {
                        lv_memcpy(rgb_dest_buf, rgb_src_buf, blend_w * 2);
                        lv_memcpy(a_dest_buf, a_src_buf, blend_w);
                        rgb_src_buf += stride_px * 2;
                        a_src_buf += stride_px;
                        rgb_dest_buf +=  blend_w * 2;
                        a_dest_buf += blend_w;
                    }
                }
                else if(cf_final != LV_COLOR_FORMAT_A8) {
                    const uint8_t * src_buf_tmp = src_buf + img_stride * relative_area.y1 + relative_area.x1 * px_size;
                    uint8_t * dest_buf_tmp = tmp_buf;
                    int32_t i;
                    for(i = 0; i < h; i++) {
                        lv_memcpy(dest_buf_tmp, src_buf_tmp, blend_w * px_size);
                        dest_buf_tmp += blend_w * px_size;
                        src_buf_tmp += img_stride;
                    }
                }
            }

            /*Apply recolor*/
            if(draw_dsc->recolor_opa > LV_OPA_MIN) {
                lv_color_t color = draw_dsc->recolor;
                lv_opa_t mix = draw_dsc->recolor_opa;
                lv_opa_t mix_inv = 255 - mix;
                if(cf_final == LV_COLOR_FORMAT_RGB565A8 || cf_final == LV_COLOR_FORMAT_RGB565) {
                    uint16_t c_mult[3];
                    c_mult[0] = (color.blue >> 3) * mix;
                    c_mult[1] = (color.green >> 2) * mix;
                    c_mult[2] = (color.red >> 3) * mix;
                    uint16_t * buf16 = (uint16_t *)tmp_buf;
                    int32_t i;
                    int32_t size = lv_area_get_size(&blend_area);
                    for(i = 0; i < size; i++) {
                        buf16[i] = (((c_mult[2] + ((buf16[i] >> 11) & 0x1F) * mix_inv) << 3) & 0xF800) +
                                   (((c_mult[1] + ((buf16[i] >> 5) & 0x3F) * mix_inv) >> 3) & 0x07E0) +
                                   ((c_mult[0] + (buf16[i] & 0x1F) * mix_inv) >> 8);
                    }
                }
                else  if(cf_final != LV_COLOR_FORMAT_A8) {
                    uint32_t size = lv_area_get_size(&blend_area);
                    uint32_t i;
                    uint16_t c_mult[3];
                    c_mult[0] = color.blue * mix;
                    c_mult[1] = color.green * mix;
                    c_mult[2] = color.red * mix;
                    uint8_t * tmp_buf_2 = tmp_buf;
                    for(i = 0; i < size * px_size; i += px_size) {
                        tmp_buf_2[i + 0] = (c_mult[0] + (tmp_buf_2[i + 0] * mix_inv)) >> 8;
                        tmp_buf_2[i + 1] = (c_mult[1] + (tmp_buf_2[i + 1] * mix_inv)) >> 8;
                        tmp_buf_2[i + 2] = (c_mult[2] + (tmp_buf_2[i + 2] * mix_inv)) >> 8;
                    }
                }
            }

            /*Blend*/
            lv_draw_sw_blend(draw_unit, &blend_dsc);

            /*Go to the next area*/
            blend_area.y1 = blend_area.y2 + 1;
            blend_area.y2 = blend_area.y1 + buf_h - 1;
            if(blend_area.y2 > y_last) {
                blend_area.y2 = y_last;
                if(cf_final == LV_COLOR_FORMAT_RGB565A8) {
                    blend_dsc.mask_buf =  tmp_buf + blend_w * 2 * lv_area_get_height(&blend_area);
                }
            }
        }

        lv_free(tmp_buf);
    }
}

#endif //LV_USE_DRAW_DAVE2D
