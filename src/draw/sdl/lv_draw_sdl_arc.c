/**
 * @file lv_templ.c
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

typedef struct {
    lv_sdl_cache_key_magic_t magic;
    uint16_t radius;
    uint16_t angle;
    lv_coord_t width;
    uint8_t rounded;
} lv_draw_arc_key_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

static lv_draw_arc_key_t arc_key_create(const lv_draw_arc_dsc_t *dsc, uint16_t radius, uint16_t start_angle,
                                 uint16_t end_angle);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_draw_sdl_draw_arc(lv_draw_ctx_t * draw_ctx, const lv_draw_arc_dsc_t * dsc, const lv_point_t * center,
                          uint16_t radius, uint16_t start_angle, uint16_t end_angle)
{
    lv_draw_sdl_ctx_t *ctx = (lv_draw_sdl_ctx_t *) draw_ctx;
    SDL_Renderer * renderer = ctx->renderer;

    lv_area_t area_out;
    area_out.x1 = center->x - radius;
    area_out.y1 = center->y - radius;
    area_out.x2 = center->x + radius - 1;  /*-1 because the center already belongs to the left/bottom part*/
    area_out.y2 = center->y + radius - 1;

    lv_area_t draw_area;
    if (!_lv_area_intersect(&draw_area, &area_out, draw_ctx->clip_area)) {
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

    int16_t mask_ids[3] = {LV_MASK_ID_INV, LV_MASK_ID_INV, LV_MASK_ID_INV};

    lv_draw_mask_radius_param_t mask_in_param;
    if(lv_area_get_width(&area_in) > 0 && lv_area_get_height(&area_in) > 0) {
        lv_draw_mask_radius_init(&mask_in_param, &area_in, LV_RADIUS_CIRCLE, true);
        mask_ids[2] = lv_draw_mask_add(&mask_in_param, NULL);
    }

    lv_draw_mask_radius_param_t mask_out_param;
    lv_draw_mask_radius_init(&mask_out_param, &area_out, LV_RADIUS_CIRCLE, false);
    mask_ids[1] = lv_draw_mask_add(&mask_out_param, NULL);

    lv_draw_mask_angle_param_t mask_angle_param;
    lv_draw_mask_angle_init(&mask_angle_param, center->x, center->y, start_angle, end_angle);
    mask_ids[0] = lv_draw_mask_add(&mask_angle_param, NULL);

    lv_coord_t w = lv_area_get_width(&draw_area), h = lv_area_get_height(&draw_area);
    SDL_Texture *texture = lv_draw_sdl_composite_tmp_obtain(ctx, LV_DRAW_SDL_COMPOSITE_KEY_ID_MASK, w, h);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    lv_draw_sdl_mask_dump_to_texture(texture, &draw_area, mask_ids, mask_ids[2] != LV_MASK_ID_INV ? 3 : 2);

    lv_draw_mask_remove_id(mask_ids[0]);
    lv_draw_mask_remove_id(mask_ids[1]);
    lv_draw_mask_free_param(&mask_angle_param);
    lv_draw_mask_free_param(&mask_out_param);

    if(mask_ids[2] != LV_MASK_ID_INV) {
        lv_draw_mask_remove_id(mask_ids[2]);
        lv_draw_mask_free_param(&mask_in_param);
    }

    SDL_Rect srcrect = {0, 0, w, h}, dstrect;
    lv_area_to_sdl_rect(&draw_area, &dstrect);
    SDL_Color color;
    lv_color_to_sdl_color(&dsc->color, &color);
    SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
    SDL_SetTextureAlphaMod(texture, dsc->opa);
    SDL_RenderCopy(ctx->renderer, texture, &srcrect, &dstrect);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_draw_arc_key_t arc_key_create(const lv_draw_arc_dsc_t *dsc, uint16_t radius, uint16_t start_angle,
                                        uint16_t end_angle)
{
    lv_draw_arc_key_t key;
    lv_memset_00(&key, sizeof(lv_draw_arc_key_t));
    key.magic = LV_GPU_CACHE_KEY_MAGIC_ARC;
    key.radius = radius;
    key.angle = ((end_angle - start_angle) % 360 + 360) % 360;
    key.width = dsc->width;
    key.rounded = dsc->rounded;
    return key;
}
#endif /*LV_USE_GPU_SDL*/
