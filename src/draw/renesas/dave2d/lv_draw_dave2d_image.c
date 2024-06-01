/**
 * @file lv_draw_dave2d_image.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_dave2d.h"
#if LV_USE_DRAW_DAVE2D

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void img_draw_core(lv_draw_unit_t * u_base, const lv_draw_image_dsc_t * draw_dsc,
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

void lv_draw_dave2d_image(lv_draw_dave2d_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc,
                          const lv_area_t * coords)
{
    if(!draw_dsc->tile) {
        lv_draw_image_normal_helper((lv_draw_unit_t *)draw_unit, draw_dsc, coords, img_draw_core);
    }
    else {
        lv_draw_image_tiled_helper((lv_draw_unit_t *)draw_unit, draw_dsc, coords, img_draw_core);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void img_draw_core(lv_draw_unit_t * u_base, const lv_draw_image_dsc_t * draw_dsc,
                          const lv_image_decoder_dsc_t * decoder_dsc, lv_draw_image_sup_t * sup,
                          const lv_area_t * img_coords, const lv_area_t * clipped_img_area)
{

    lv_draw_dave2d_unit_t * u = (lv_draw_dave2d_unit_t *)u_base;

    (void)sup; //remove warning about unused parameter

    bool transformed = draw_dsc->rotation != 0 || draw_dsc->scale_x != LV_SCALE_NONE ||
                       draw_dsc->scale_y != LV_SCALE_NONE ? true : false;

    const lv_draw_buf_t * decoded = decoder_dsc->decoded;
    const uint8_t * src_buf = decoded->data;
    const lv_image_header_t * header = &decoded->header;
    uint32_t img_stride = decoded->header.stride;
    lv_color_format_t cf = decoded->header.cf;
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
    void * p_intermediate_buf = NULL;

#if LV_USE_OS
    lv_result_t  status;
    status = lv_mutex_lock(u->pd2Mutex);
    LV_ASSERT(LV_RESULT_OK == status);
#endif

    buffer_area = u->base_unit.target_layer->buf_area;
    draw_area = *img_coords;
    clipped_area = *clipped_img_area;

    x = 0 - u->base_unit.target_layer->buf_area.x1;
    y = 0 - u->base_unit.target_layer->buf_area.y1;

    lv_area_move(&draw_area, x, y);
    lv_area_move(&buffer_area, x, y);
    lv_area_move(&clipped_area, x, y);

    /* Generate render operations*/
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

#if defined(RENESAS_CORTEX_M85)
#if (BSP_CFG_DCACHE_ENABLED)
    d1_cacheblockflush(u->d2_handle, 0, src_buf,
                       img_stride * header->h); //Stride is in bytes, not pixels/texels
#endif
#endif

    if(LV_COLOR_FORMAT_RGB565A8 == cf) {

        lv_point_t p1[4] = { //Points in clockwise order
            {0, 0},
            {header->w - 1, 0},
            {header->w - 1, header->h - 1},
            {0, header->h - 1},
        };

        d2_s32 dxu1 = D2_FIX16(1);
        d2_s32 dxv1 = D2_FIX16(0);
        d2_s32 dyu1 = D2_FIX16(0);
        d2_s32 dyv1 = D2_FIX16(1);

        uint32_t size = header->h * (header->w * lv_color_format_get_size(LV_COLOR_FORMAT_ARGB8888));
        p_intermediate_buf = lv_malloc(size);

        d2_framebuffer(u->d2_handle,
                       p_intermediate_buf,
                       (d2_s32)header->w,
                       (d2_u32)header->w,
                       (d2_u32)header->h,
                       lv_draw_dave2d_lv_colour_fmt_to_d2_fmt(LV_COLOR_FORMAT_ARGB8888));

        d2_cliprect(u->d2_handle, (d2_border)0, (d2_border)0, (d2_border)header->w - 1,
                    (d2_border)header->h - 1);

        d2_settexopparam(u->d2_handle, d2_cc_alpha, draw_dsc->opa, 0);

        d2_settextureoperation(u->d2_handle, d2_to_replace, d2_to_copy, d2_to_copy, d2_to_copy);

        d2_settexturemapping(u->d2_handle, D2_FIX4(p1[0].x), D2_FIX4(p1[0].y), D2_FIX16(0), D2_FIX16(0), dxu1, dxv1, dyu1,
                             dyv1);
        d2_settexturemode(u->d2_handle, d2_tm_filter);
        d2_setfillmode(u->d2_handle, d2_fm_texture);

        d2_settexture(u->d2_handle, (void *)src_buf,
                      (d2_s32)(img_stride / lv_color_format_get_size(LV_COLOR_FORMAT_RGB565)),
                      header->w,  header->h, lv_draw_dave2d_lv_colour_fmt_to_d2_fmt(LV_COLOR_FORMAT_RGB565));

        d2_setblendmode(u->d2_handle, d2_bm_one, d2_bm_zero);

        d2_renderquad(u->d2_handle,
                      (d2_point)D2_FIX4(p1[0].x),
                      (d2_point)D2_FIX4(p1[0].y),
                      (d2_point)D2_FIX4(p1[1].x),
                      (d2_point)D2_FIX4(p1[1].y),
                      (d2_point)D2_FIX4(p1[2].x),
                      (d2_point)D2_FIX4(p1[2].y),
                      (d2_point)D2_FIX4(p1[3].x),
                      (d2_point)D2_FIX4(p1[3].y),
                      0);

        d2_setblendmode(u->d2_handle, d2_bm_zero, d2_bm_one); //Keep the RGB data in the intermediate buffer

        d2_setalphablendmode(u->d2_handle, d2_bm_one, d2_bm_zero);    //Write SRC alpha, i.e. A8 data

        d2_settextureoperation(u->d2_handle, d2_to_copy, d2_to_copy, d2_to_copy, d2_to_copy);

        d2_settexture(u->d2_handle, (void *)(src_buf +  header->h * (header->w * lv_color_format_get_size(
                                                                         LV_COLOR_FORMAT_RGB565))),
                      (d2_s32)(img_stride / lv_color_format_get_size(LV_COLOR_FORMAT_RGB565)),
                      header->w,  header->h, lv_draw_dave2d_lv_colour_fmt_to_d2_fmt(LV_COLOR_FORMAT_A8));

        d2_renderquad(u->d2_handle,
                      (d2_point)D2_FIX4(p1[0].x),
                      (d2_point)D2_FIX4(p1[0].y),
                      (d2_point)D2_FIX4(p1[1].x),
                      (d2_point)D2_FIX4(p1[1].y),
                      (d2_point)D2_FIX4(p1[2].x),
                      (d2_point)D2_FIX4(p1[2].y),
                      (d2_point)D2_FIX4(p1[3].x),
                      (d2_point)D2_FIX4(p1[3].y),
                      0);

        cf = LV_COLOR_FORMAT_ARGB8888;
        src_buf = p_intermediate_buf;
        img_stride = header->w * lv_color_format_get_size(cf);

    }

    d2_framebuffer_from_layer(u->d2_handle, u->base_unit.target_layer);

    d2_cliprect(u->d2_handle, (d2_border)clipped_area.x1, (d2_border)clipped_area.y1, (d2_border)clipped_area.x2,
                (d2_border)clipped_area.y2);

    d2_settexopparam(u->d2_handle, d2_cc_alpha, draw_dsc->opa, 0);

    if(LV_COLOR_FORMAT_RGB565 == cf) {
        d2_settextureoperation(u->d2_handle, d2_to_replace, d2_to_copy, d2_to_copy, d2_to_copy);
    }
    else { //Formats with an alpha channel,
        d2_settextureoperation(u->d2_handle, d2_to_copy, d2_to_copy, d2_to_copy, d2_to_copy);
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
    }
    else { //LV_BLEND_MODE_MULTIPLY,   /**< Multiply the foreground and background*/
        /* TODO */
    }

    lv_point_t p[4] = { //Points in clockwise order
        {0, 0},
        {header->w - 1, 0},
        {header->w - 1, header->h - 1},
        {0, header->h - 1},
    };

    d2_settexture(u->d2_handle, (void *)src_buf,
                  (d2_s32)(img_stride / lv_color_format_get_size(cf)),
                  header->w,  header->h, lv_draw_dave2d_lv_colour_fmt_to_d2_fmt(cf));

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

        int32_t angle_limited = draw_dsc->rotation;
        if(angle_limited > 3600) angle_limited -= 3600;
        if(angle_limited < 0) angle_limited += 3600;

        int32_t angle_low = angle_limited / 10;

        if(0 != angle_low) {
            /* LV_TRIGO_SHIFT is 15, so only need to shift by 1 to get 16:16 fixed point */
            dxv = (d2_s32)((1 << 1) * lv_trigo_sin((int16_t)angle_low));
            dxu = (d2_s32)((1 << 1) * lv_trigo_cos((int16_t)angle_low));
            dyv = (d2_s32)((1 << 1) * lv_trigo_sin((int16_t)angle_low + 90));
            dyu = (d2_s32)((1 << 1) * lv_trigo_cos((int16_t)angle_low + 90));
        }

        if(LV_SCALE_NONE != draw_dsc->scale_x) {
            dxu = (dxu * LV_SCALE_NONE) / draw_dsc->scale_x;
            dxv = (dxv * LV_SCALE_NONE) / draw_dsc->scale_x;
        }
        if(LV_SCALE_NONE != draw_dsc->scale_y) {
            dyu = (dyu * LV_SCALE_NONE) / draw_dsc->scale_y;
            dyv = (dyv * LV_SCALE_NONE) / draw_dsc->scale_y;
        }
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

    if(NULL != p_intermediate_buf) {
        lv_free(p_intermediate_buf);
    }

#if LV_USE_OS
    status = lv_mutex_unlock(u->pd2Mutex);
    LV_ASSERT(LV_RESULT_OK == status);
#endif

}

#endif //LV_USE_DRAW_DAVE2D
