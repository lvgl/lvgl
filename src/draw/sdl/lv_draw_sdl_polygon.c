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

static void dump_masks(SDL_Texture * texture, const lv_area_t * coords);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_draw_sdl_polygon(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * draw_dsc, const lv_point_t * points,
                         uint16_t point_cnt)
{
    if(point_cnt < 3) return;
    if(points == NULL) return;

    lv_draw_mask_polygon_param_t polygon_param;
    lv_draw_mask_polygon_init(&polygon_param, points, point_cnt);

    if(polygon_param.cfg.point_cnt < 3) {
        lv_draw_mask_free_param(&polygon_param);
        return;
    }

    lv_area_t poly_coords = {.x1 = LV_COORD_MAX, .y1 = LV_COORD_MAX, .x2 = LV_COORD_MIN, .y2 = LV_COORD_MIN};

    uint16_t i;
    for(i = 0; i < point_cnt; i++) {
        poly_coords.x1 = LV_MIN(poly_coords.x1, polygon_param.cfg.points[i].x);
        poly_coords.y1 = LV_MIN(poly_coords.y1, polygon_param.cfg.points[i].y);
        poly_coords.x2 = LV_MAX(poly_coords.x2, polygon_param.cfg.points[i].x);
        poly_coords.y2 = LV_MAX(poly_coords.y2, polygon_param.cfg.points[i].y);
    }

    bool is_common;
    lv_area_t draw_area;
    is_common = _lv_area_intersect(&draw_area, &poly_coords, draw_ctx->clip_area);
    if(!is_common) {
        lv_draw_mask_free_param(&polygon_param);
        return;
    }

    lv_draw_sdl_ctx_t * ctx = (lv_draw_sdl_ctx_t *) draw_ctx;

    int16_t mask_id = lv_draw_mask_add(&polygon_param, NULL);

    lv_coord_t w = lv_area_get_width(&draw_area), h = lv_area_get_height(&draw_area);
    SDL_Texture * texture = lv_draw_sdl_composite_texture_obtain(ctx, LV_DRAW_SDL_COMPOSITE_TEXTURE_ID_STREAM1, w, h);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    dump_masks(texture, &draw_area);

    lv_draw_mask_remove_id(mask_id);
    lv_draw_mask_free_param(&polygon_param);

    SDL_Rect srcrect = {0, 0, w, h}, dstrect;
    lv_area_to_sdl_rect(&draw_area, &dstrect);
    SDL_Color color;
    lv_color_to_sdl_color(&draw_dsc->bg_color, &color);
    SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
    SDL_SetTextureAlphaMod(texture, draw_dsc->bg_opa);
    SDL_RenderCopy(ctx->renderer, texture, &srcrect, &dstrect);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void dump_masks(SDL_Texture * texture, const lv_area_t * coords)
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
        res = lv_draw_mask_apply(line_buf, abs_x, abs_y, len);
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
    }
    lv_mem_buf_release(line_buf);
    SDL_UnlockTexture(texture);
}

#endif /*LV_USE_GPU_SDL*/
