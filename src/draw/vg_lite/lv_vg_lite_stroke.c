/**
 * @file lv_vg_lite_stroke.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_vg_lite_stroke.h"

#if LV_USE_DRAW_VG_LITE && LV_USE_VECTOR_GRAPHIC

#include "lv_vg_lite_path.h"
#include "lv_draw_vg_lite_type.h"
#include "lv_vg_lite_math.h"
#include "../lv_draw_vector_private.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    /* stroke path */
    lv_vg_lite_path_t * path;

    /* stroke parameters */
    float width;
    lv_vector_stroke_cap_t cap;
    lv_vector_stroke_join_t join;
    uint16_t miter_limit;
    lv_array_t dash_pattern;
} stroke_item_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static bool stroke_create_cb(stroke_item_t * item, void * user_data);
static void stroke_free_cb(stroke_item_t * item, void * user_data);
static lv_cache_compare_res_t stroke_compare_cb(const stroke_item_t * lhs, const stroke_item_t * rhs);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_vg_lite_stroke_init(struct lv_draw_vg_lite_unit_t * unit, uint32_t cache_cnt)
{
    LV_ASSERT_NULL(unit);

    const lv_cache_ops_t ops = {
        .compare_cb = (lv_cache_compare_cb_t)stroke_compare_cb,
        .create_cb = (lv_cache_create_cb_t)stroke_create_cb,
        .free_cb = (lv_cache_free_cb_t)stroke_free_cb,
    };

    unit->stroke_cache = lv_cache_create(&lv_cache_class_lru_rb_count, sizeof(stroke_item_t), cache_cnt, ops);
    lv_cache_set_name(unit->stroke_cache, "VG_STROKE");
}

void lv_vg_lite_stroke_deinit(struct lv_draw_vg_lite_unit_t * unit)
{
    LV_ASSERT_NULL(unit);
    LV_ASSERT_NULL(unit->stroke_cache);
    lv_cache_destroy(unit->stroke_cache, NULL);
    unit->stroke_cache = NULL;
}

static vg_lite_cap_style_t lv_stroke_cap_to_vg(lv_vector_stroke_cap_t cap)
{
    switch(cap) {
        case LV_VECTOR_STROKE_CAP_SQUARE:
            return VG_LITE_CAP_SQUARE;
        case LV_VECTOR_STROKE_CAP_ROUND:
            return VG_LITE_CAP_ROUND;
        case LV_VECTOR_STROKE_CAP_BUTT:
            return VG_LITE_CAP_BUTT;
        default:
            return VG_LITE_CAP_SQUARE;
    }
}

static vg_lite_join_style_t lv_stroke_join_to_vg(lv_vector_stroke_join_t join)
{
    switch(join) {
        case LV_VECTOR_STROKE_JOIN_BEVEL:
            return VG_LITE_JOIN_BEVEL;
        case LV_VECTOR_STROKE_JOIN_ROUND:
            return VG_LITE_JOIN_ROUND;
        case LV_VECTOR_STROKE_JOIN_MITER:
            return VG_LITE_JOIN_MITER;
        default:
            return VG_LITE_JOIN_BEVEL;
    }
}

lv_cache_entry_t * lv_vg_lite_stroke_get(struct lv_draw_vg_lite_unit_t * unit,
                                         struct lv_vg_lite_path_t * path,
                                         const lv_vector_stroke_dsc_t * dsc)
{
    LV_ASSERT_NULL(unit);
    LV_ASSERT_NULL(path);
    LV_ASSERT_NULL(dsc);

    vg_lite_path_t * vg_path = lv_vg_lite_path_get_path(path);

    if(vg_path->format != VG_LITE_FP32) {
        LV_LOG_ERROR("only support VG_LITE_FP32 format");
        return NULL;
    }

    /* prepare search key */
    stroke_item_t search_key;
    lv_memzero(&search_key, sizeof(search_key));
    search_key.cap = dsc->cap;
    search_key.join = dsc->join;
    search_key.width = dsc->width;
    search_key.miter_limit = dsc->miter_limit;

    /* A one-time read-only array that only copies the pointer but not the content */
    search_key.dash_pattern = dsc->dash_pattern;
    search_key.path = path;

    lv_cache_entry_t * cache_node_entry = lv_cache_acquire(unit->stroke_cache, &search_key, &search_key);
    if(cache_node_entry) {
        return cache_node_entry;
    }

    cache_node_entry = lv_cache_acquire_or_create(unit->stroke_cache, &search_key, &search_key);
    if(cache_node_entry == NULL) {
        LV_LOG_ERROR("stroke cache creating failed");
        return NULL;
    }

    return cache_node_entry;
}

struct lv_vg_lite_path_t * lv_vg_lite_stroke_get_path(lv_cache_entry_t * cache_entry)
{
    LV_ASSERT_NULL(cache_entry);

    stroke_item_t * stroke_item = lv_cache_entry_get_data(cache_entry);
    LV_ASSERT_NULL(stroke_item);
    return stroke_item->path;
}

void lv_vg_lite_stroke_drop(struct lv_draw_vg_lite_unit_t * unit,
                            lv_cache_entry_t * cache_entry)
{
    LV_ASSERT_NULL(unit);
    LV_ASSERT_NULL(cache_entry);
    lv_cache_release(unit->stroke_cache, cache_entry, NULL);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static bool stroke_create_cb(stroke_item_t * item, void * user_data)
{
    LV_ASSERT_NULL(item);

    stroke_item_t * src = user_data;
    LV_ASSERT_NULL(src);

    lv_memzero(item, sizeof(stroke_item_t));

    /* copy path */
    item->path = lv_vg_lite_path_create(VG_LITE_FP32);
    lv_vg_lite_path_append_path(item->path, src->path);

    /* copy parameters */
    item->cap = src->cap;
    item->join = src->join;
    item->width = src->width;
    item->miter_limit = src->miter_limit;

    /* copy dash pattern */
    uint32_t size = lv_array_size(&src->dash_pattern);
    if(size) {
        lv_array_init(&item->dash_pattern, size, sizeof(float));
        lv_array_copy(&item->dash_pattern, &src->dash_pattern);
    }

    /* update parameters */
    vg_lite_path_t * vg_path = lv_vg_lite_path_get_path(item->path);
    LV_VG_LITE_CHECK_ERROR(vg_lite_set_path_type(vg_path, VG_LITE_DRAW_STROKE_PATH));

    vg_lite_error_t error = vg_lite_set_stroke(
                                vg_path,
                                lv_stroke_cap_to_vg(item->cap),
                                lv_stroke_join_to_vg(item->join),
                                item->width,
                                item->miter_limit,
                                lv_array_front(&item->dash_pattern),
                                size,
                                item->width / 2,
                                0);

    if(error != VG_LITE_SUCCESS) {
        LV_LOG_ERROR("vg_lite_set_stroke failed: %d(%s)", (int)error, lv_vg_lite_error_string(error));
        stroke_free_cb(item, NULL);
        return false;
    }

    const vg_lite_pointer * ori_path = vg_path->path;
    const vg_lite_uint32_t ori_path_length = vg_path->path_length;

    LV_PROFILER_BEGIN_TAG("vg_lite_update_stroke");
    error = vg_lite_update_stroke(vg_path);
    LV_PROFILER_END_TAG("vg_lite_update_stroke");

    /* check if path is changed */
    LV_ASSERT_MSG(vg_path->path_length == ori_path_length, "vg_path->path_length should not change");
    LV_ASSERT_MSG(vg_path->path == ori_path, "vg_path->path should not change");

    if(error != VG_LITE_SUCCESS) {
        LV_LOG_ERROR("vg_lite_update_stroke failed: %d(%s)", (int)error, lv_vg_lite_error_string(error));
        stroke_free_cb(item, NULL);
        return false;
    }

    return true;
}

static void stroke_free_cb(stroke_item_t * item, void * user_data)
{
    LV_UNUSED(user_data);
    LV_ASSERT_NULL(item);

    lv_array_deinit(&item->dash_pattern);
    lv_vg_lite_path_destroy(item->path);
    lv_memzero(item, sizeof(stroke_item_t));
}

static lv_cache_compare_res_t path_compare(const vg_lite_path_t * lhs, const vg_lite_path_t * rhs)
{
    LV_VG_LITE_ASSERT_PATH(lhs);
    LV_VG_LITE_ASSERT_PATH(rhs);

    LV_ASSERT(lhs->format == VG_LITE_FP32);
    LV_ASSERT(rhs->format == VG_LITE_FP32);

    if(lhs->path_length != rhs->path_length) {
        return lhs->path_length > rhs->path_length ? 1 : -1;
    }

    int cmp_res = lv_memcmp(lhs->path, rhs->path, lhs->path_length);
    if(cmp_res != 0) {
        return cmp_res > 0 ? 1 : -1;
    }

    return 0;
}

static lv_cache_compare_res_t stroke_compare_cb(const stroke_item_t * lhs, const stroke_item_t * rhs)
{
    if(lhs->width != lhs->width) {
        return lhs->width > lhs->width ? 1 : -1;
    }

    if(lhs->cap != rhs->cap) {
        return lhs->cap > rhs->cap ? 1 : -1;
    }

    if(lhs->join != rhs->join) {
        return lhs->join > rhs->join ? 1 : -1;
    }

    if(lhs->miter_limit != rhs->miter_limit) {
        return lhs->miter_limit > rhs->miter_limit ? 1 : -1;
    }

    uint32_t lhs_dash_pattern_size = lv_array_size(&lhs->dash_pattern);
    uint32_t rhs_dash_pattern_size = lv_array_size(&rhs->dash_pattern);

    if(lhs_dash_pattern_size != rhs_dash_pattern_size) {
        return lhs_dash_pattern_size > rhs_dash_pattern_size ? 1 : -1;
    }

    if(lhs_dash_pattern_size > 0 && rhs_dash_pattern_size > 0) {
        LV_ASSERT(lhs->dash_pattern.element_size == sizeof(float));
        LV_ASSERT(rhs->dash_pattern.element_size == sizeof(float));

        const float * lhs_dash_pattern = lv_array_front(&lhs->dash_pattern);
        const float * rhs_dash_pattern = lv_array_front(&rhs->dash_pattern);

        /* compare dash pattern */
        int cmp_res = lv_memcmp(lhs_dash_pattern, rhs_dash_pattern, lhs_dash_pattern_size * sizeof(float));
        if(cmp_res != 0) {
            return cmp_res > 0 ? 1 : -1;
        }
    }

    return path_compare(lv_vg_lite_path_get_path(lhs->path), lv_vg_lite_path_get_path(rhs->path));
}

#endif /*LV_USE_DRAW_VG_LITE && LV_USE_VECTOR_GRAPHIC*/
