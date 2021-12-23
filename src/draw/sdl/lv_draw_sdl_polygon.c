/**
 * @file lv_draw_sdl_polygon.c
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
#include "lv_draw_sdl_mask.h"

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
void lv_draw_sdl_polygon(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * draw_dsc, const lv_point_t * points,
                        uint16_t point_cnt)
{
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

    cir_x = ((radius - thick_half) * lv_trigo_sin(90 - angle)) >> (LV_TRIGO_SHIFT - ps);
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
