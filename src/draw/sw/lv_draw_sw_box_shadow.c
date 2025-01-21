/**
 * @file lv_draw_sw_box_shadow.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../misc/lv_area_private.h"
#include "lv_draw_sw_mask_private.h"
#include "../lv_draw_private.h"
#include "lv_draw_sw.h"
#if LV_USE_DRAW_SW

#if LV_DRAW_SW_COMPLEX

#include "blend/lv_draw_sw_blend_private.h"
#include "../../core/lv_global.h"
#include "../../misc/lv_math.h"
#include "../../core/lv_refr.h"
#include "../../misc/lv_assert.h"
#include "../../stdlib/lv_string.h"
#include "../lv_draw_mask.h"

/*********************
 *      DEFINES
 *********************/
#define SHADOW_UPSCALE_SHIFT    6
#define SHADOW_ENHANCE          1

#if defined(LV_DRAW_SW_SHADOW_CACHE_SIZE) && LV_DRAW_SW_SHADOW_CACHE_SIZE > 0
    #define shadow_cache LV_GLOBAL_DEFAULT()->sw_shadow_cache
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void /* LV_ATTRIBUTE_FAST_MEM */ shadow_draw_corner_buf(const lv_area_t * coords, uint16_t * sh_buf, int32_t s,
                                                               int32_t r);
static void /* LV_ATTRIBUTE_FAST_MEM */ shadow_blur_corner(int32_t size, int32_t sw, uint16_t * sh_ups_buf);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_sw_box_shadow(lv_draw_task_t * t, const lv_draw_box_shadow_dsc_t * dsc, const lv_area_t * coords)
{
    /*Calculate the rectangle which is blurred to get the shadow in `shadow_area`*/
    lv_area_t core_area;
    core_area.x1 = coords->x1  + dsc->ofs_x - dsc->spread;
    core_area.x2 = coords->x2  + dsc->ofs_x + dsc->spread;
    core_area.y1 = coords->y1  + dsc->ofs_y - dsc->spread;
    core_area.y2 = coords->y2  + dsc->ofs_y + dsc->spread;

    /*Calculate the bounding box of the shadow*/
    lv_area_t shadow_area;
    shadow_area.x1 = core_area.x1 - dsc->width / 2 - 1;
    shadow_area.x2 = core_area.x2 + dsc->width / 2 + 1;
    shadow_area.y1 = core_area.y1 - dsc->width / 2 - 1;
    shadow_area.y2 = core_area.y2 + dsc->width / 2 + 1;

    lv_opa_t opa = dsc->opa;
    if(opa > LV_OPA_MAX) opa = LV_OPA_COVER;

    /*Get clipped draw area which is the real draw area.
     *It is always the same or inside `shadow_area`*/
    lv_area_t draw_area;
    if(!lv_area_intersect(&draw_area, &shadow_area, &t->clip_area)) return;

    /*Consider 1 px smaller bg to be sure the edge will be covered by the shadow*/
    lv_area_t bg_area;
    lv_area_copy(&bg_area, coords);
    lv_area_increase(&bg_area, -1, -1);

    /*Get the clamped radius*/
    int32_t r_bg = dsc->radius;
    int32_t short_side = LV_MIN(lv_area_get_width(&bg_area), lv_area_get_height(&bg_area));
    if(r_bg > short_side >> 1) r_bg = short_side >> 1;

    /*Get the clamped radius*/
    int32_t r_sh = dsc->radius;
    short_side = LV_MIN(lv_area_get_width(&core_area), lv_area_get_height(&core_area));
    if(r_sh > short_side >> 1) r_sh = short_side >> 1;

    /*Get how many pixels are affected by the blur on the corners*/
    int32_t corner_size = dsc->width  + r_sh;

    lv_opa_t * sh_buf;

#if LV_DRAW_SW_SHADOW_CACHE_SIZE
    lv_draw_sw_shadow_cache_t * cache = &shadow_cache;
    if(cache->cache_size == corner_size && cache->cache_r == r_sh) {
        /*Use the cache if available*/
        sh_buf = lv_malloc(corner_size * corner_size);
        LV_ASSERT_MALLOC(sh_buf);
        lv_memcpy(sh_buf, cache->cache, corner_size * corner_size);
    }
    else {
        /*A larger buffer is required for calculation*/
        sh_buf = lv_malloc(corner_size * corner_size * sizeof(uint16_t));
        LV_ASSERT_MALLOC(sh_buf);
        shadow_draw_corner_buf(&core_area, (uint16_t *)sh_buf, dsc->width, r_sh);

        /*Cache the corner if it fits into the cache size*/
        if((uint32_t)corner_size * corner_size < sizeof(cache->cache)) {
            lv_memcpy(cache->cache, sh_buf, corner_size * corner_size);
            cache->cache_size = corner_size;
            cache->cache_r = r_sh;
        }
    }
#else
    sh_buf = lv_malloc(corner_size * corner_size * sizeof(uint16_t));
    LV_ASSERT_MALLOC(sh_buf);
    shadow_draw_corner_buf(&core_area, (uint16_t *)sh_buf, dsc->width, r_sh);
#endif /*LV_DRAW_SW_SHADOW_CACHE_SIZE*/

    /*Skip a lot of masking if the background will cover the shadow that would be masked out*/
    bool simple = dsc->bg_cover;

    /*Create a radius mask to clip remove shadow on the bg area*/

    lv_draw_sw_mask_radius_param_t mask_rout_param;
    void * masks[2] = {0};
    if(!simple) {
        lv_draw_sw_mask_radius_init(&mask_rout_param, &bg_area, r_bg, true);
        masks[0] = &mask_rout_param;
    }

    lv_opa_t * mask_buf = lv_malloc(lv_area_get_width(&shadow_area));
    lv_area_t blend_area;
    lv_area_t clip_area_sub;
    lv_opa_t * sh_buf_tmp;
    int32_t y;
    bool simple_sub;

    lv_draw_sw_blend_dsc_t blend_dsc;
    lv_memzero(&blend_dsc, sizeof(blend_dsc));
    blend_dsc.blend_area = &blend_area;
    blend_dsc.mask_area = &blend_area;
    blend_dsc.mask_buf = mask_buf;
    blend_dsc.color = dsc->color;
    blend_dsc.opa = dsc->opa;

    int32_t w_half = shadow_area.x1 + lv_area_get_width(&shadow_area) / 2;
    int32_t h_half = shadow_area.y1 + lv_area_get_height(&shadow_area) / 2;

    /*Draw the corners if they are on the current clip area and not fully covered by the bg*/

    /*Top right corner*/
    blend_area.x2 = shadow_area.x2;
    blend_area.x1 = shadow_area.x2 - corner_size + 1;
    blend_area.y1 = shadow_area.y1;
    blend_area.y2 = shadow_area.y1 + corner_size - 1;
    /*Do not overdraw the other top corners*/
    blend_area.x1 = LV_MAX(blend_area.x1, w_half);
    blend_area.y2 = LV_MIN(blend_area.y2, h_half);

    if(lv_area_intersect(&clip_area_sub, &blend_area, &t->clip_area) &&
       !lv_area_is_in(&clip_area_sub, &bg_area, r_bg)) {
        int32_t w = lv_area_get_width(&clip_area_sub);
        sh_buf_tmp = sh_buf;
        sh_buf_tmp += (clip_area_sub.y1 - shadow_area.y1) * corner_size;
        sh_buf_tmp += clip_area_sub.x1 - (shadow_area.x2 - corner_size + 1);

        /*Do not mask if out of the bg*/
        if(simple && lv_area_is_out(&clip_area_sub, &bg_area, r_bg)) simple_sub = true;
        else simple_sub = simple;
        if(w > 0) {
            blend_dsc.mask_buf = mask_buf;
            blend_area.x1 = clip_area_sub.x1;
            blend_area.x2 = clip_area_sub.x2;
            blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;    /*In simple mode it won't be overwritten*/
            for(y = clip_area_sub.y1; y <= clip_area_sub.y2; y++) {
                blend_area.y1 = y;
                blend_area.y2 = y;

                if(!simple_sub) {
                    lv_memcpy(mask_buf, sh_buf_tmp, corner_size);
                    blend_dsc.mask_res = lv_draw_sw_mask_apply(masks, mask_buf, clip_area_sub.x1, y, w);
                    if(blend_dsc.mask_res == LV_DRAW_SW_MASK_RES_FULL_COVER) blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;
                }
                else {
                    blend_dsc.mask_buf = sh_buf_tmp;
                }
                lv_draw_sw_blend(t, &blend_dsc);
                sh_buf_tmp += corner_size;
            }
        }
    }

    /*Bottom right corner.
     *Almost the same as top right just read the lines of `sh_buf` from then end*/
    blend_area.x2 = shadow_area.x2;
    blend_area.x1 = shadow_area.x2 - corner_size + 1;
    blend_area.y1 = shadow_area.y2 - corner_size + 1;
    blend_area.y2 = shadow_area.y2;
    /*Do not overdraw the other corners*/
    blend_area.x1 = LV_MAX(blend_area.x1, w_half);
    blend_area.y1 = LV_MAX(blend_area.y1, h_half + 1);

    if(lv_area_intersect(&clip_area_sub, &blend_area, &t->clip_area) &&
       !lv_area_is_in(&clip_area_sub, &bg_area, r_bg)) {
        int32_t w = lv_area_get_width(&clip_area_sub);
        sh_buf_tmp = sh_buf;
        sh_buf_tmp += (blend_area.y2 - clip_area_sub.y2) * corner_size;
        sh_buf_tmp += clip_area_sub.x1 - (shadow_area.x2 - corner_size + 1);
        /*Do not mask if out of the bg*/
        if(simple && lv_area_is_out(&clip_area_sub, &bg_area, r_bg)) simple_sub = true;
        else simple_sub = simple;

        if(w > 0) {
            blend_dsc.mask_buf = mask_buf;
            blend_area.x1 = clip_area_sub.x1;
            blend_area.x2 = clip_area_sub.x2;
            blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;    /*In simple mode it won't be overwritten*/
            for(y = clip_area_sub.y2; y >= clip_area_sub.y1; y--) {
                blend_area.y1 = y;
                blend_area.y2 = y;

                if(!simple_sub) {
                    lv_memcpy(mask_buf, sh_buf_tmp, corner_size);
                    blend_dsc.mask_res = lv_draw_sw_mask_apply(masks, mask_buf, clip_area_sub.x1, y, w);
                    if(blend_dsc.mask_res == LV_DRAW_SW_MASK_RES_FULL_COVER) blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;
                }
                else {
                    blend_dsc.mask_buf = sh_buf_tmp;
                }
                lv_draw_sw_blend(t, &blend_dsc);
                sh_buf_tmp += corner_size;
            }
        }
    }

    /*Top side*/
    blend_area.x1 = shadow_area.x1 + corner_size;
    blend_area.x2 = shadow_area.x2 - corner_size;
    blend_area.y1 = shadow_area.y1;
    blend_area.y2 = shadow_area.y1 + corner_size - 1;
    blend_area.y2 = LV_MIN(blend_area.y2, h_half);

    if(lv_area_intersect(&clip_area_sub, &blend_area, &t->clip_area) &&
       !lv_area_is_in(&clip_area_sub, &bg_area, r_bg)) {
        int32_t w = lv_area_get_width(&clip_area_sub);
        sh_buf_tmp = sh_buf;
        sh_buf_tmp += (clip_area_sub.y1 - blend_area.y1) * corner_size;

        /*Do not mask if out of the bg*/
        if(simple && lv_area_is_out(&clip_area_sub, &bg_area, r_bg)) simple_sub = true;
        else simple_sub = simple;

        if(w > 0) {
            if(!simple_sub) {
                blend_dsc.mask_buf = mask_buf;
            }
            else {
                blend_dsc.mask_buf = NULL;
            }
            blend_area.x1 = clip_area_sub.x1;
            blend_area.x2 = clip_area_sub.x2;

            for(y = clip_area_sub.y1; y <= clip_area_sub.y2; y++) {
                blend_area.y1 = y;
                blend_area.y2 = y;

                if(!simple_sub) {
                    lv_memset(mask_buf, sh_buf_tmp[0], w);
                    blend_dsc.mask_res = lv_draw_sw_mask_apply(masks, mask_buf, clip_area_sub.x1, y, w);
                    if(blend_dsc.mask_res == LV_DRAW_SW_MASK_RES_FULL_COVER) blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;
                    lv_draw_sw_blend(t, &blend_dsc);
                }
                else {
                    blend_dsc.opa = opa == LV_OPA_COVER ? sh_buf_tmp[0] : LV_OPA_MIX2(sh_buf_tmp[0], dsc->opa);
                    lv_draw_sw_blend(t, &blend_dsc);
                }
                sh_buf_tmp += corner_size;
            }
        }
    }
    blend_dsc.opa = dsc->opa;    /*Restore*/

    /*Bottom side*/
    blend_area.x1 = shadow_area.x1 + corner_size;
    blend_area.x2 = shadow_area.x2 - corner_size;
    blend_area.y1 = shadow_area.y2 - corner_size + 1;
    blend_area.y2 = shadow_area.y2;
    blend_area.y1 = LV_MAX(blend_area.y1, h_half + 1);

    if(lv_area_intersect(&clip_area_sub, &blend_area, &t->clip_area) &&
       !lv_area_is_in(&clip_area_sub, &bg_area, r_bg)) {
        int32_t w = lv_area_get_width(&clip_area_sub);
        sh_buf_tmp = sh_buf;
        sh_buf_tmp += (blend_area.y2 - clip_area_sub.y2) * corner_size;
        if(w > 0) {
            /*Do not mask if out of the bg*/
            if(simple && lv_area_is_out(&clip_area_sub, &bg_area, r_bg)) simple_sub = true;
            else simple_sub = simple;

            if(!simple_sub) {
                blend_dsc.mask_buf = mask_buf;
            }
            else {
                blend_dsc.mask_buf = NULL;
            }
            blend_area.x1 = clip_area_sub.x1;
            blend_area.x2 = clip_area_sub.x2;

            for(y = clip_area_sub.y2; y >= clip_area_sub.y1; y--) {
                blend_area.y1 = y;
                blend_area.y2 = y;

                /*Do not mask if out of the bg*/
                if(simple && lv_area_is_out(&clip_area_sub, &bg_area, r_bg)) simple_sub = true;
                else simple_sub = simple;

                if(!simple_sub) {
                    lv_memset(mask_buf, sh_buf_tmp[0], w);
                    blend_dsc.mask_res = lv_draw_sw_mask_apply(masks, mask_buf, clip_area_sub.x1, y, w);
                    if(blend_dsc.mask_res == LV_DRAW_SW_MASK_RES_FULL_COVER) blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;
                    lv_draw_sw_blend(t, &blend_dsc);
                }
                else {
                    blend_dsc.opa = opa == LV_OPA_COVER ? sh_buf_tmp[0] : (sh_buf_tmp[0] * dsc->opa) >> 8;
                    lv_draw_sw_blend(t, &blend_dsc);

                }
                sh_buf_tmp += corner_size;
            }
        }
    }

    blend_dsc.opa = dsc->opa;    /*Restore*/

    /*Right side*/
    blend_area.x1 = shadow_area.x2 - corner_size + 1;
    blend_area.x2 = shadow_area.x2;
    blend_area.y1 = shadow_area.y1 + corner_size;
    blend_area.y2 = shadow_area.y2 - corner_size;
    /*Do not overdraw the other corners*/
    blend_area.y1 = LV_MIN(blend_area.y1, h_half + 1);
    blend_area.y2 = LV_MAX(blend_area.y2, h_half);
    blend_area.x1 = LV_MAX(blend_area.x1, w_half);

    if(lv_area_intersect(&clip_area_sub, &blend_area, &t->clip_area) &&
       !lv_area_is_in(&clip_area_sub, &bg_area, r_bg)) {
        int32_t w = lv_area_get_width(&clip_area_sub);
        sh_buf_tmp = sh_buf;
        sh_buf_tmp += (corner_size - 1) * corner_size;
        sh_buf_tmp += clip_area_sub.x1 - (shadow_area.x2 - corner_size + 1);

        /*Do not mask if out of the bg*/
        if(simple && lv_area_is_out(&clip_area_sub, &bg_area, r_bg)) simple_sub = true;
        else simple_sub = simple;
        blend_dsc.mask_buf = simple_sub ? sh_buf_tmp : mask_buf;

        if(w > 0) {
            blend_area.x1 = clip_area_sub.x1;
            blend_area.x2 = clip_area_sub.x2;
            blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;    /*In simple mode it won't be overwritten*/
            for(y = clip_area_sub.y1; y <= clip_area_sub.y2; y++) {
                blend_area.y1 = y;
                blend_area.y2 = y;

                if(!simple_sub) {
                    lv_memcpy(mask_buf, sh_buf_tmp, w);
                    blend_dsc.mask_res = lv_draw_sw_mask_apply(masks, mask_buf, clip_area_sub.x1, y, w);
                    if(blend_dsc.mask_res == LV_DRAW_SW_MASK_RES_FULL_COVER) blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;
                }
                lv_draw_sw_blend(t, &blend_dsc);
            }
        }
    }

    /*Mirror the shadow corner buffer horizontally*/
    sh_buf_tmp = sh_buf ;
    for(y = 0; y < corner_size; y++) {
        int32_t x;
        lv_opa_t * start = sh_buf_tmp;
        lv_opa_t * end = sh_buf_tmp + corner_size - 1;
        for(x = 0; x < corner_size / 2; x++) {
            lv_opa_t tmp = *start;
            *start = *end;
            *end = tmp;

            start++;
            end--;
        }
        sh_buf_tmp += corner_size;
    }

    /*Left side*/
    blend_area.x1 = shadow_area.x1;
    blend_area.x2 = shadow_area.x1 + corner_size - 1;
    blend_area.y1 = shadow_area.y1 + corner_size;
    blend_area.y2 = shadow_area.y2 - corner_size;
    /*Do not overdraw the other corners*/
    blend_area.y1 = LV_MIN(blend_area.y1, h_half + 1);
    blend_area.y2 = LV_MAX(blend_area.y2, h_half);
    blend_area.x2 = LV_MIN(blend_area.x2, w_half - 1);

    if(lv_area_intersect(&clip_area_sub, &blend_area, &t->clip_area) &&
       !lv_area_is_in(&clip_area_sub, &bg_area, r_bg)) {
        int32_t w = lv_area_get_width(&clip_area_sub);
        sh_buf_tmp = sh_buf;
        sh_buf_tmp += (corner_size - 1) * corner_size;
        sh_buf_tmp += clip_area_sub.x1 - blend_area.x1;

        /*Do not mask if out of the bg*/
        if(simple && lv_area_is_out(&clip_area_sub, &bg_area, r_bg)) simple_sub = true;
        else simple_sub = simple;
        blend_dsc.mask_buf = simple_sub ? sh_buf_tmp : mask_buf;
        if(w > 0) {
            blend_area.x1 = clip_area_sub.x1;
            blend_area.x2 = clip_area_sub.x2;
            blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;    /*In simple mode it won't be overwritten*/
            for(y = clip_area_sub.y1; y <= clip_area_sub.y2; y++) {
                blend_area.y1 = y;
                blend_area.y2 = y;

                if(!simple_sub) {
                    lv_memcpy(mask_buf, sh_buf_tmp, w);
                    blend_dsc.mask_res = lv_draw_sw_mask_apply(masks, mask_buf, clip_area_sub.x1, y, w);
                    if(blend_dsc.mask_res == LV_DRAW_SW_MASK_RES_FULL_COVER) blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;
                }

                lv_draw_sw_blend(t, &blend_dsc);
            }
        }
    }

    /*Top left corner*/
    blend_area.x1 = shadow_area.x1;
    blend_area.x2 = shadow_area.x1 + corner_size - 1;
    blend_area.y1 = shadow_area.y1;
    blend_area.y2 = shadow_area.y1 + corner_size - 1;
    /*Do not overdraw the other corners*/
    blend_area.x2 = LV_MIN(blend_area.x2, w_half - 1);
    blend_area.y2 = LV_MIN(blend_area.y2, h_half);

    if(lv_area_intersect(&clip_area_sub, &blend_area, &t->clip_area) &&
       !lv_area_is_in(&clip_area_sub, &bg_area, r_bg)) {
        int32_t w = lv_area_get_width(&clip_area_sub);
        sh_buf_tmp = sh_buf;
        sh_buf_tmp += (clip_area_sub.y1 - blend_area.y1) * corner_size;
        sh_buf_tmp += clip_area_sub.x1 - blend_area.x1;

        /*Do not mask if out of the bg*/
        if(simple && lv_area_is_out(&clip_area_sub, &bg_area, r_bg)) simple_sub = true;
        else simple_sub = simple;
        blend_dsc.mask_buf = mask_buf;

        if(w > 0) {
            blend_area.x1 = clip_area_sub.x1;
            blend_area.x2 = clip_area_sub.x2;
            blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;    /*In simple mode it won't be overwritten*/
            for(y = clip_area_sub.y1; y <= clip_area_sub.y2; y++) {
                blend_area.y1 = y;
                blend_area.y2 = y;

                if(!simple_sub) {
                    lv_memcpy(mask_buf, sh_buf_tmp, corner_size);
                    blend_dsc.mask_res = lv_draw_sw_mask_apply(masks, mask_buf, clip_area_sub.x1, y, w);
                    if(blend_dsc.mask_res == LV_DRAW_SW_MASK_RES_FULL_COVER) blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;
                }
                else {
                    blend_dsc.mask_buf = sh_buf_tmp;
                }

                lv_draw_sw_blend(t, &blend_dsc);
                sh_buf_tmp += corner_size;
            }
        }
    }

    /*Bottom left corner.
     *Almost the same as bottom right just read the lines of `sh_buf` from then end*/
    blend_area.x1 = shadow_area.x1 ;
    blend_area.x2 = shadow_area.x1 + corner_size - 1;
    blend_area.y1 = shadow_area.y2 - corner_size + 1;
    blend_area.y2 = shadow_area.y2;
    /*Do not overdraw the other corners*/
    blend_area.y1 = LV_MAX(blend_area.y1, h_half + 1);
    blend_area.x2 = LV_MIN(blend_area.x2, w_half - 1);

    if(lv_area_intersect(&clip_area_sub, &blend_area, &t->clip_area) &&
       !lv_area_is_in(&clip_area_sub, &bg_area, r_bg)) {
        int32_t w = lv_area_get_width(&clip_area_sub);
        sh_buf_tmp = sh_buf;
        sh_buf_tmp += (blend_area.y2 - clip_area_sub.y2) * corner_size;
        sh_buf_tmp += clip_area_sub.x1 - blend_area.x1;

        /*Do not mask if out of the bg*/
        if(simple && lv_area_is_out(&clip_area_sub, &bg_area, r_bg)) simple_sub = true;
        else simple_sub = simple;
        blend_dsc.mask_buf = mask_buf;
        if(w > 0) {
            blend_area.x1 = clip_area_sub.x1;
            blend_area.x2 = clip_area_sub.x2;
            blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;    /*In simple mode it won't be overwritten*/
            for(y = clip_area_sub.y2; y >= clip_area_sub.y1; y--) {
                blend_area.y1 = y;
                blend_area.y2 = y;

                if(!simple_sub) {
                    lv_memcpy(mask_buf, sh_buf_tmp, corner_size);
                    blend_dsc.mask_res = lv_draw_sw_mask_apply(masks, mask_buf, clip_area_sub.x1, y, w);
                    if(blend_dsc.mask_res == LV_DRAW_SW_MASK_RES_FULL_COVER) blend_dsc.mask_res = LV_DRAW_SW_MASK_RES_CHANGED;
                }
                else {
                    blend_dsc.mask_buf = sh_buf_tmp;
                }
                lv_draw_sw_blend(t, &blend_dsc);
                sh_buf_tmp += corner_size;
            }
        }
    }

    /*Draw the center rectangle.*/
    blend_area.x1 = shadow_area.x1 + corner_size ;
    blend_area.x2 = shadow_area.x2 - corner_size;
    blend_area.y1 = shadow_area.y1 + corner_size;
    blend_area.y2 = shadow_area.y2 - corner_size;
    blend_area.y1 = LV_MIN(blend_area.y1, h_half + 1);
    blend_area.y2 = LV_MAX(blend_area.y2, h_half);
    blend_dsc.mask_buf = mask_buf;

    if(lv_area_intersect(&clip_area_sub, &blend_area, &t->clip_area) &&
       !lv_area_is_in(&clip_area_sub, &bg_area, r_bg)) {
        int32_t w = lv_area_get_width(&clip_area_sub);
        if(w > 0) {
            blend_area.x1 = clip_area_sub.x1;
            blend_area.x2 = clip_area_sub.x2;
            for(y = clip_area_sub.y1; y <= clip_area_sub.y2; y++) {
                blend_area.y1 = y;
                blend_area.y2 = y;

                lv_memset(mask_buf, 0xff, w);
                blend_dsc.mask_res = lv_draw_sw_mask_apply(masks, mask_buf, clip_area_sub.x1, y, w);
                lv_draw_sw_blend(t, &blend_dsc);
            }
        }
    }

    if(!simple) {
        lv_draw_sw_mask_free_param(&mask_rout_param);
    }
    lv_free(sh_buf);
    lv_free(mask_buf);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Calculate a blurred corner
 * @param coords Coordinates of the shadow
 * @param sh_buf a buffer to store the result. Its size should be `(sw + r)^2 * 2`
 * @param sw shadow width
 * @param r radius
 */
static void LV_ATTRIBUTE_FAST_MEM shadow_draw_corner_buf(const lv_area_t * coords, uint16_t * sh_buf, int32_t sw,
                                                         int32_t r)
{
    int32_t sw_ori = sw;
    int32_t size = sw_ori  + r;

    lv_area_t sh_area;
    lv_area_copy(&sh_area, coords);
    sh_area.x2 = sw / 2 + r - 1  - ((sw & 1) ? 0 : 1);
    sh_area.y1 = sw / 2 + 1;

    sh_area.x1 = sh_area.x2 - lv_area_get_width(coords);
    sh_area.y2 = sh_area.y1 + lv_area_get_height(coords);

    lv_draw_sw_mask_radius_param_t mask_param;
    lv_draw_sw_mask_radius_init(&mask_param, &sh_area, r, false);

#if SHADOW_ENHANCE
    /*Set half shadow width because blur will be repeated*/
    if(sw_ori == 1) sw = 1;
    else sw = sw_ori >> 1;
#endif /*SHADOW_ENHANCE*/

    int32_t y;
    lv_opa_t * mask_line = lv_malloc(size);
    uint16_t * sh_ups_tmp_buf = (uint16_t *)sh_buf;
    for(y = 0; y < size; y++) {
        lv_memset(mask_line, 0xff, size);
        lv_draw_sw_mask_res_t mask_res = mask_param.dsc.cb(mask_line, 0, y, size, &mask_param);
        if(mask_res == LV_DRAW_SW_MASK_RES_TRANSP) {
            lv_memzero(sh_ups_tmp_buf, size * sizeof(sh_ups_tmp_buf[0]));
        }
        else {
            int32_t i;
            sh_ups_tmp_buf[0] = (mask_line[0] << SHADOW_UPSCALE_SHIFT) / sw;
            for(i = 1; i < size; i++) {
                if(mask_line[i] == mask_line[i - 1]) sh_ups_tmp_buf[i] = sh_ups_tmp_buf[i - 1];
                else  sh_ups_tmp_buf[i] = (mask_line[i] << SHADOW_UPSCALE_SHIFT) / sw;
            }
        }

        sh_ups_tmp_buf += size;
    }
    lv_free(mask_line);

    lv_draw_sw_mask_free_param(&mask_param);

    if(sw == 1) {
        int32_t i;
        lv_opa_t * res_buf = (lv_opa_t *)sh_buf;
        for(i = 0; i < size * size; i++) {
            res_buf[i] = (sh_buf[i] >> SHADOW_UPSCALE_SHIFT);
        }
        return;
    }

    shadow_blur_corner(size, sw, sh_buf);

#if SHADOW_ENHANCE == 0
    /*The result is required in lv_opa_t not uint16_t*/
    uint32_t x;
    lv_opa_t * res_buf = (lv_opa_t *)sh_buf;
    for(x = 0; x < size * size; x++) {
        res_buf[x] = sh_buf[x];
    }
#else
    sw += sw_ori & 1;
    if(sw > 1) {
        uint32_t i;
        uint32_t max_v_div = (LV_OPA_COVER << SHADOW_UPSCALE_SHIFT) / sw;
        for(i = 0; i < (uint32_t)size * size; i++) {
            if(sh_buf[i] == 0) continue;
            else if(sh_buf[i] == LV_OPA_COVER) sh_buf[i] = max_v_div;
            else  sh_buf[i] = (sh_buf[i] << SHADOW_UPSCALE_SHIFT) / sw;
        }

        shadow_blur_corner(size, sw, sh_buf);
    }
    int32_t x;
    lv_opa_t * res_buf = (lv_opa_t *)sh_buf;
    for(x = 0; x < size * size; x++) {
        res_buf[x] = (lv_opa_t) sh_buf[x];
    }
#endif

}

static void LV_ATTRIBUTE_FAST_MEM shadow_blur_corner(int32_t size, int32_t sw, uint16_t * sh_ups_buf)
{
    int32_t s_left = sw >> 1;
    int32_t s_right = (sw >> 1);
    if((sw & 1) == 0) s_left--;

    /*Horizontal blur*/
    uint16_t * sh_ups_blur_buf = lv_malloc(size * sizeof(uint16_t));

    int32_t x;
    int32_t y;

    uint16_t * sh_ups_tmp_buf = sh_ups_buf;

    for(y = 0; y < size; y++) {
        int32_t v = sh_ups_tmp_buf[size - 1] * sw;
        for(x = size - 1; x >= 0; x--) {
            sh_ups_blur_buf[x] = v;

            /*Forget the right pixel*/
            uint32_t right_val = 0;
            if(x + s_right < size) right_val = sh_ups_tmp_buf[x + s_right];
            v -= right_val;

            /*Add the left pixel*/
            uint32_t left_val;
            if(x - s_left - 1 < 0) left_val = sh_ups_tmp_buf[0];
            else left_val = sh_ups_tmp_buf[x - s_left - 1];
            v += left_val;
        }
        lv_memcpy(sh_ups_tmp_buf, sh_ups_blur_buf, size * sizeof(uint16_t));
        sh_ups_tmp_buf += size;
    }

    /*Vertical blur*/
    uint32_t i;
    uint32_t max_v = LV_OPA_COVER << SHADOW_UPSCALE_SHIFT;
    uint32_t max_v_div = max_v / sw;
    for(i = 0; i < (uint32_t)size * size; i++) {
        if(sh_ups_buf[i] == 0) continue;
        else if(sh_ups_buf[i] == max_v) sh_ups_buf[i] = max_v_div;
        else sh_ups_buf[i] = sh_ups_buf[i] / sw;
    }

    for(x = 0; x < size; x++) {
        sh_ups_tmp_buf = &sh_ups_buf[x];
        int32_t v = sh_ups_tmp_buf[0] * sw;
        for(y = 0; y < size ; y++, sh_ups_tmp_buf += size) {
            sh_ups_blur_buf[y] = v < 0 ? 0 : (v >> SHADOW_UPSCALE_SHIFT);

            /*Forget the top pixel*/
            uint32_t top_val;
            if(y - s_right <= 0) top_val = sh_ups_tmp_buf[0];
            else top_val = sh_ups_buf[(y - s_right) * size + x];
            v -= top_val;

            /*Add the bottom pixel*/
            uint32_t bottom_val;
            if(y + s_left + 1 < size) bottom_val = sh_ups_buf[(y + s_left + 1) * size + x];
            else bottom_val = sh_ups_buf[(size - 1) * size + x];
            v += bottom_val;
        }

        /*Write back the result into `sh_ups_buf`*/
        sh_ups_tmp_buf = &sh_ups_buf[x];
        for(y = 0; y < size; y++, sh_ups_tmp_buf += size) {
            (*sh_ups_tmp_buf) = sh_ups_blur_buf[y];
        }
    }

    lv_free(sh_ups_blur_buf);
}

#else /*LV_DRAW_SW_COMPLEX*/

void lv_draw_sw_box_shadow(lv_draw_unit_t * draw_unit, const lv_draw_box_shadow_dsc_t * dsc, const lv_area_t * coords)
{
    LV_UNUSED(draw_unit);
    LV_UNUSED(dsc);
    LV_UNUSED(coords);

    LV_LOG_WARN("LV_DRAW_SW_COMPLEX needs to be enabled");
}

#endif /*LV_DRAW_SW_COMPLEX*/

#endif /*LV_DRAW_USE_SW*/
