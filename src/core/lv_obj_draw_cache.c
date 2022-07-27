/**
 * @file lv_obj_draw_cache.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_obj_draw_cache.h"
#include "../others/snapshot/lv_snapshot.h"

/*********************
 *      DEFINES
 *********************/
#if LV_USE_OBJ_DRAW_CACHE

#if !LV_USE_SNAPSHOT
    # error "Need to enable 'LV_USE_SNAPSHOT'"
#endif

/**********************
 *      TYPEDEFS
 **********************/

typedef struct _lv_obj_draw_cache_t {
    lv_img_dsc_t img_dsc;
    lv_img_cf_t cf;
    bool invalid;
    bool skip_cache;
} lv_obj_draw_cache_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_obj_draw_cache_set_enable(lv_obj_t * obj, bool en)
{
    LV_ASSERT_NULL(obj);

    if(en) {
        lv_obj_allocate_spec_attr(obj);

        if(obj->spec_attr->draw_cache == NULL) {
            lv_obj_draw_cache_t * draw_cache = lv_malloc(sizeof(lv_obj_draw_cache_t));
            LV_ASSERT_MALLOC(draw_cache);

            if(draw_cache == NULL) {
                return;
            }

            lv_memset(draw_cache, 0, sizeof(lv_obj_draw_cache_t));
            draw_cache->cf = LV_IMG_CF_TRUE_COLOR_ALPHA;
            obj->spec_attr->draw_cache = draw_cache;
        }
    }
    else {
        _lv_obj_draw_cache_free(obj);
    }

    lv_obj_invalidate(obj);
}

void lv_obj_draw_cache_set_img_cf(lv_obj_t * obj, lv_img_cf_t cf)
{
    LV_ASSERT_NULL(obj);
    if(!lv_obj_has_draw_cache(obj)) {
        return;
    }

    obj->spec_attr->draw_cache->cf = cf;
}

lv_img_cf_t lv_obj_draw_cache_get_img_cf(lv_obj_t * obj)
{
    LV_ASSERT_NULL(obj);
    if(!lv_obj_has_draw_cache(obj)) {
        return LV_IMG_CF_UNKNOWN;
    }

    return obj->spec_attr->draw_cache->cf;
}

const lv_img_dsc_t * lv_obj_draw_cache_get_img_dsc(lv_obj_t * obj)
{
    LV_ASSERT_NULL(obj);
    if(!lv_obj_has_draw_cache(obj)) {
        return NULL;
    }

    return &obj->spec_attr->draw_cache->img_dsc;
}

void lv_obj_draw_cache_invalidate(lv_obj_t * obj)
{
    LV_ASSERT_NULL(obj);
    if(!lv_obj_has_draw_cache(obj)) {
        return;
    }

    obj->spec_attr->draw_cache->invalid = true;
}

void lv_obj_draw_cache_invalidate_cancel(lv_obj_t * obj)
{
    LV_ASSERT_NULL(obj);
    if(!lv_obj_has_draw_cache(obj)) {
        return;
    }

    obj->spec_attr->draw_cache->invalid = false;
}

lv_res_t _lv_obj_draw_cache(lv_obj_t * obj, lv_draw_ctx_t * draw_ctx)
{
    if(!lv_obj_has_draw_cache(obj)) {
        return LV_RES_INV;
    }

    lv_obj_draw_cache_t * draw_cache = obj->spec_attr->draw_cache;

    if(draw_cache->skip_cache) {
        return LV_RES_INV;
    }

    lv_img_dsc_t * img_dsc = &draw_cache->img_dsc;

    if(draw_cache->invalid) {
        draw_cache->invalid = false;

        uint32_t buf_size = lv_snapshot_buf_size_needed(obj, draw_cache->cf);

        if(buf_size == 0) {
            LV_LOG_WARN("can't get snapshot buffer size");
            return LV_RES_INV;
        }

        if(buf_size != img_dsc->data_size) {
            img_dsc->data = lv_realloc(img_dsc->data, buf_size);
            img_dsc->data_size = buf_size;
        }

        if(img_dsc->data == NULL) {
            LV_LOG_WARN("can't alloc buffer for snapshot");
            return LV_RES_INV;
        }

        LV_LOG_TRACE("obj(%p) take snapshot: buffer = %p, size = %d",
                     obj,
                     img_dsc->data,
                     img_dsc->data_size);

        draw_cache->skip_cache = true;

        lv_res_t res = lv_snapshot_take_to_buf(
                           obj,
                           draw_cache->cf,
                           img_dsc,
                           img_dsc->data,
                           img_dsc->data_size);

        draw_cache->skip_cache = false;

        if(res != LV_RES_OK) {
            LV_LOG_WARN("snapshot failed");
            return LV_RES_INV;
        }
    }

    lv_coord_t w = lv_obj_get_style_transform_width(obj, LV_PART_MAIN);
    lv_coord_t h = lv_obj_get_style_transform_height(obj, LV_PART_MAIN);
    lv_area_t coords;
    lv_area_copy(&coords, &obj->coords);
    coords.x1 -= w;
    coords.x2 += w;
    coords.y1 -= h;
    coords.y2 += h;

    lv_draw_img_dsc_t draw_img_dsc;
    lv_draw_img_dsc_init(&draw_img_dsc);
    lv_draw_img(draw_ctx, &draw_img_dsc, &coords, img_dsc);

    LV_LOG_TRACE("obj(%p) draw cached img(%d x %d, cf = %d)",
                 obj,
                 lv_area_get_width(&coords),
                 lv_area_get_height(&coords),
                 draw_cache->cf);

    return LV_RES_OK;
}

void _lv_obj_draw_cache_free(lv_obj_t * obj)
{
    LV_ASSERT_NULL(obj);
    if(!lv_obj_has_draw_cache(obj)) {
        return;
    }

    lv_obj_draw_cache_t * draw_cache = obj->spec_attr->draw_cache;

    if(draw_cache->img_dsc.data) {
        lv_free(draw_cache->img_dsc.data);
        draw_cache->img_dsc.data = NULL;
    }

    lv_free(draw_cache);

    obj->spec_attr->draw_cache = NULL;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_OBJ_DRAW_CACHE*/
