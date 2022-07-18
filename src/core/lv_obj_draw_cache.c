/**
 * @file lv_obj_draw_cache.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_obj_draw_cache.h"
#include "../extra/others/snapshot/lv_snapshot.h"

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
    bool invalidate;
    bool draw_normal;
    bool enable;
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
    lv_obj_allocate_spec_attr(obj);

    if(obj->spec_attr->draw_cache == NULL) {
        lv_obj_draw_cache_t * draw_cache = lv_mem_alloc(sizeof(lv_obj_draw_cache_t));
        LV_ASSERT_MALLOC(draw_cache);

        if(draw_cache == NULL) {
            return;
        }

        lv_memset_00(draw_cache, sizeof(lv_obj_draw_cache_t));
        obj->spec_attr->draw_cache = draw_cache;
    }

    obj->spec_attr->draw_cache->enable = en;
}

bool lv_obj_draw_cache_get_enable(lv_obj_t * obj)
{
    LV_ASSERT_NULL(obj);
    if(obj->spec_attr == NULL || obj->spec_attr->draw_cache == NULL) {
        return false;
    }

    return obj->spec_attr->draw_cache->enable;
}

void lv_obj_draw_cache_invalidate(lv_obj_t * obj)
{
    LV_ASSERT_NULL(obj);
    if(obj->spec_attr == NULL || obj->spec_attr->draw_cache == NULL) {
        return;
    }

    obj->spec_attr->draw_cache->invalidate = true;
    lv_obj_invalidate(obj);
}

lv_res_t _lv_obj_draw_cache(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_current_target(e);

    if(obj->spec_attr == NULL || obj->spec_attr->draw_cache == NULL) {
        return LV_RES_INV;
    }

    lv_obj_draw_cache_t * draw_cache = obj->spec_attr->draw_cache;

    if(draw_cache->draw_normal || !draw_cache->enable) {
        return LV_RES_INV;
    }

    lv_img_dsc_t * img_dsc = &draw_cache->img_dsc;
    lv_draw_ctx_t * draw_ctx = lv_event_get_draw_ctx(e);

    if(draw_cache->invalidate) {
        draw_cache->invalidate = false;

        uint32_t buf_size = lv_snapshot_buf_size_needed(obj, LV_IMG_CF_TRUE_COLOR_ALPHA);

        if(buf_size == 0) {
            LV_LOG_WARN("can't get snapshot buffer size");
            return LV_RES_INV;
        }

        if(buf_size != img_dsc->data_size) {
            img_dsc->data = lv_mem_realloc(img_dsc->data, buf_size);
            img_dsc->data_size = buf_size;
        }

        LV_LOG_INFO("obj(%p) take snapshot: buffer = %p, size = %d",
                    obj,
                    img_dsc->data,
                    img_dsc->data_size);

        draw_cache->draw_normal = true;

        lv_res_t res = lv_snapshot_take_to_buf(
                           obj,
                           LV_IMG_CF_TRUE_COLOR_ALPHA,
                           img_dsc,
                           img_dsc->data,
                           img_dsc->data_size);

        draw_cache->draw_normal = false;

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

    LV_LOG_INFO("obj(%p) draw cache", obj);

    return LV_RES_OK;
}

void _lv_obj_draw_cache_free(lv_obj_t * obj)
{
    LV_ASSERT_NULL(obj);
    if(obj->spec_attr == NULL || obj->spec_attr->draw_cache == NULL) {
        return;
    }

    lv_obj_draw_cache_t * draw_cache = obj->spec_attr->draw_cache;

    if(draw_cache->img_dsc.data) {
        lv_mem_free(draw_cache->img_dsc.data);
        draw_cache->img_dsc.data = NULL;
    }

    lv_mem_free(draw_cache);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_OBJ_DRAW_CACHE*/
