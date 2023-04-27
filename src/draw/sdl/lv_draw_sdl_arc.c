/**
 * @file lv_draw_sdl_arc.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf_internal.h"

#if LV_USE_GPU_SDL

#include "lv_draw_sdl.h"
#include "lv_draw_sdl_utils.h"
#include "lv_draw_sdl_texture_cache.h"
#include "lv_draw_sdl_composite.h"

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

/**********************
 *      MACROS
 **********************/

static void dump_masks(SDL_Texture * texture, const lv_area_t * coords, const int16_t * ids, int16_t ids_count,
                       const int16_t * caps);

static void get_cap_area(int16_t angle, lv_coord_t thickness, uint16_t radius, const lv_point_t * center,
                         lv_area_t * out);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_draw_sdl_draw_arc(lv_draw_ctx_t * draw_ctx, const lv_draw_arc_dsc_t * dsc, const lv_point_t * center,
                          uint16_t radius, uint16_t start_angle, uint16_t end_angle)
{
    lv_draw_sdl_ctx_t * ctx = (lv_draw_sdl_ctx_t *) draw_ctx;

    lv_area_t area_out;
    area_out.x1 = center->x - radius;
    area_out.y1 = center->y - radius;
    area_out.x2 = center->x + radius - 1;  /*-1 because the center already belongs to the left/bottom part*/
    area_out.y2 = center->y + radius - 1;

    lv_area_t draw_area;
    if(!_lv_area_intersect(&draw_area, &area_out, draw_ctx->clip_area)) {
        return;
    }

    lv_area_t area_in;
    lv_area_copy(&area_in, &area_out);
    area_in.x1 += dsc->width;
    area_in.y1 += dsc->width;
    area_in.x2 -= dsc->width;
    area_in.y2 -= dsc->width;


    while(start_angle >= 360) start_angle -= 360;
    while(end_angle >= 360) end_angle -= 360;

    int16_t mask_ids[3] = {LV_MASK_ID_INV, LV_MASK_ID_INV, LV_MASK_ID_INV}, mask_ids_count = 1;
    int16_t cap_ids[2] = {LV_MASK_ID_INV, LV_MASK_ID_INV};

    lv_draw_mask_radius_param_t mask_out_param;
    lv_draw_mask_radius_init(&mask_out_param, &area_out, LV_RADIUS_CIRCLE, false);
    mask_ids[0] = lv_draw_mask_add(&mask_out_param, NULL);

    lv_draw_mask_radius_param_t mask_in_param;
    if(lv_area_get_width(&area_in) > 0 && lv_area_get_height(&area_in) > 0) {
        lv_draw_mask_radius_init(&mask_in_param, &area_in, LV_RADIUS_CIRCLE, true);
        mask_ids[1] = lv_draw_mask_add(&mask_in_param, NULL);
        mask_ids_count++;
    }

    lv_draw_mask_angle_param_t mask_angle_param;
    if((start_angle - end_angle) % 360) {
        lv_draw_mask_angle_init(&mask_angle_param, center->x, center->y, start_angle, end_angle);
        mask_ids[2] = lv_draw_mask_add(&mask_angle_param, NULL);
        mask_ids_count++;
    }

    lv_draw_mask_radius_param_t cap_start_param, cap_end_param;
    if(mask_ids_count == 3 && dsc->rounded) {
        lv_area_t start_area, end_area;
        get_cap_area((int16_t) start_angle, dsc->width, radius, center, &start_area);
        get_cap_area((int16_t) end_angle, dsc->width, radius, center, &end_area);
        lv_draw_mask_radius_init(&cap_start_param, &start_area, dsc->width / 2, false);
        cap_ids[0] = lv_draw_mask_add(&cap_start_param, NULL);
        lv_draw_mask_radius_init(&cap_end_param, &end_area, dsc->width / 2, false);
        cap_ids[1] = lv_draw_mask_add(&cap_end_param, NULL);
    }

    lv_coord_t w = lv_area_get_width(&draw_area), h = lv_area_get_height(&draw_area);
    bool texture_cached = false;
    SDL_Texture * texture = lv_draw_sdl_composite_texture_obtain(ctx, LV_DRAW_SDL_COMPOSITE_TEXTURE_ID_STREAM1, w, h,
                                                                 &texture_cached);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    dump_masks(texture, &draw_area, mask_ids, mask_ids_count, cap_ids[0] != LV_MASK_ID_INV ? cap_ids : NULL);

    lv_draw_mask_remove_id(mask_ids[0]);
    lv_draw_mask_free_param(&mask_out_param);

    if(mask_ids_count > 1) {
        lv_draw_mask_remove_id(mask_ids[1]);
        lv_draw_mask_free_param(&mask_in_param);
    }

    if(mask_ids_count > 2) {
        lv_draw_mask_remove_id(mask_ids[2]);
        lv_draw_mask_free_param(&mask_angle_param);
    }

    if(cap_ids[0] != LV_MASK_ID_INV) {
        lv_draw_mask_remove_id(cap_ids[0]);
        lv_draw_mask_remove_id(cap_ids[1]);
        lv_draw_mask_free_param(&cap_start_param);
        lv_draw_mask_free_param(&cap_end_param);
    }

    SDL_Rect srcrect = {0, 0, w, h}, dstrect;
    lv_area_to_sdl_rect(&draw_area, &dstrect);
    SDL_Color color;
    lv_color_to_sdl_color(&dsc->color, &color);
    SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
    SDL_SetTextureAlphaMod(texture, dsc->opa);
    SDL_RenderCopy(ctx->renderer, texture, &srcrect, &dstrect);

    if(!texture_cached) {
        LV_LOG_WARN("Texture is not cached, this will impact performance.");
        SDL_DestroyTexture(texture);
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void dump_masks(SDL_Texture * texture, const lv_area_t * coords, const int16_t * ids, int16_t ids_count,
                       const int16_t * caps)
{
    lv_coord_t w = lv_area_get_width(coords), h = lv_area_get_height(coords);
    SDL_assert(w > 0 && h > 0);
    SDL_Rect rect = {0, 0, w, h};
    uint8_t * pixels;
    int pitch;
    if(SDL_LockTexture(texture, &rect, (void **) &pixels, &pitch) != 0) return;

    lv_opa_t * line_buf = lv_mem_buf_get(rect.w);
    for(lv_coord_t y = 0; y < rect.h; y++) {
        lv_memset_ff(line_buf, rect.w);
        lv_coord_t abs_x = (lv_coord_t) coords->x1, abs_y = (lv_coord_t)(y + coords->y1), len = (lv_coord_t) rect.w;
        lv_draw_mask_res_t res;
        res = lv_draw_mask_apply_ids(line_buf, abs_x, abs_y, len, ids, ids_count);
        if(res == LV_DRAW_MASK_RES_TRANSP) {
            lv_memset_00(&pixels[y * pitch], 4 * rect.w);
        }
        else if(res == LV_DRAW_MASK_RES_FULL_COVER) {
            lv_memset_ff(&pixels[y * pitch], 4 * rect.w);
        }
        else {
            for(int x = 0; x < rect.w; x++) {
                uint8_t * pixel = &pixels[y * pitch + x * 4];
                *pixel = line_buf[x];
                pixel[1] = pixel[2] = pixel[3] = 0xFF;
            }
        }
        if(caps) {
            for(int i = 0; i < 2; i++) {
                lv_memset_ff(line_buf, rect.w);
                res = lv_draw_mask_apply_ids(line_buf, abs_x, abs_y, len, &caps[i], 1);
                if(res == LV_DRAW_MASK_RES_TRANSP) {
                    /* Ignore */
                }
                else if(res == LV_DRAW_MASK_RES_FULL_COVER) {
                    lv_memset_ff(&pixels[y * pitch], 4 * rect.w);
                }
                else {
                    for(int x = 0; x < rect.w; x++) {
                        uint8_t * pixel = &pixels[y * pitch + x * 4];
                        uint16_t old_opa = line_buf[x] + *pixel;
                        *pixel = LV_MIN(old_opa, 0xFF);
                        pixel[1] = pixel[2] = pixel[3] = 0xFF;
                    }
                }
            }
        }
    }
    lv_mem_buf_release(line_buf);
    SDL_UnlockTexture(texture);
}

static void get_cap_area(int16_t angle, lv_coord_t thickness, uint16_t radius, const lv_point_t * center,
                         lv_area_t * out)
{
    const uint8_t ps = 8;
    const uint8_t pa = 127;

    int32_t thick_half = thickness / 2;
    uint8_t thick_corr = (thickness & 0x01) ? 0 : 1;

    int32_t cir_x;
    int32_t cir_y;

    cir_x = ((radius - thick_half) * lv_trigo_sin((int16_t)(90 - angle))) >> (LV_TRIGO_SHIFT - ps);
    cir_y = ((radius - thick_half) * lv_trigo_sin(angle)) >> (LV_TRIGO_SHIFT - ps);

    /*Actually the center of the pixel need to be calculated so apply 1/2 px offset*/
    if(cir_x > 0) {
        cir_x = (cir_x - pa) >> ps;
        out->x1 = cir_x - thick_half + thick_corr;
        out->x2 = cir_x + thick_half;
    }
    else {
        cir_x = (cir_x + pa) >> ps;
        out->x1 = cir_x - thick_half;
        out->x2 = cir_x + thick_half - thick_corr;
    }

    if(cir_y > 0) {
        cir_y = (cir_y - pa) >> ps;
        out->y1 = cir_y - thick_half + thick_corr;
        out->y2 = cir_y + thick_half;
    }
    else {
        cir_y = (cir_y + pa) >> ps;
        out->y1 = cir_y - thick_half;
        out->y2 = cir_y + thick_half - thick_corr;
    }
    lv_area_move(out, center->x, center->y);
}

#endif /*LV_USE_GPU_SDL*/
