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


/**
 * Since the key-value data structure of lv_cache is integrated, the kv data structure
 * will be saved at the same time when the cache is successfully created.
 * In order to pursue efficiency during the matching process, the primary key (lv) used for matching
 * will not dup the dash_pattern secondary pointer, so when the creation is successful, dash_pattern needs
 * to be dup to the child key (vg), so type is added here to distinguish where the real data of dash_pattern exists.
 */
typedef enum {
    DASH_PATTERN_TYPE_LV,
    DASH_PATTERN_TYPE_VG
} dash_pattern_type_t;

typedef struct {
    dash_pattern_type_t dash_pattern_type;

    struct {
        /* path data */
        lv_vg_lite_path_t * path;

        /* stroke parameters */
        float width;
        lv_vector_stroke_cap_t cap;
        lv_vector_stroke_join_t join;
        uint16_t miter_limit;
        lv_array_t dash_pattern;
    } lv;

    struct {
        /* stroke path */
        lv_vg_lite_path_t * path;

        /* dash pattern, for comparison only */
        lv_array_t dash_pattern;
    } vg;
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

void lv_vg_lite_stroke_init(struct _lv_draw_vg_lite_unit_t * unit, uint32_t cache_cnt)
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

void lv_vg_lite_stroke_deinit(struct _lv_draw_vg_lite_unit_t * unit)
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

lv_cache_entry_t * lv_vg_lite_stroke_get(struct _lv_draw_vg_lite_unit_t * unit,
                                         struct _lv_vg_lite_path_t * path,
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
    search_key.lv.cap = dsc->cap;
    search_key.lv.join = dsc->join;
    search_key.lv.width = dsc->width;
    search_key.lv.miter_limit = dsc->miter_limit;

    /* A one-time read-only array that only copies the pointer but not the content */
    search_key.lv.dash_pattern = dsc->dash_pattern;
    search_key.lv.path = path;

    lv_cache_entry_t * cache_node_entry = lv_cache_acquire(unit->stroke_cache, &search_key, NULL);
    if(cache_node_entry) {
        return cache_node_entry;
    }

    cache_node_entry = lv_cache_acquire_or_create(unit->stroke_cache, &search_key, NULL);
    if(cache_node_entry == NULL) {
        LV_LOG_ERROR("stroke cache creating failed");
        return NULL;
    }

    return cache_node_entry;
}

struct _lv_vg_lite_path_t * lv_vg_lite_stroke_get_path(lv_cache_entry_t * cache_entry)
{
    LV_ASSERT_NULL(cache_entry);

    stroke_item_t * stroke_item = lv_cache_entry_get_data(cache_entry);
    LV_ASSERT_NULL(stroke_item);

    if(lv_array_size(&stroke_item->vg.dash_pattern)) {
        /* check if dash pattern must be duped */
        LV_ASSERT(stroke_item->dash_pattern_type == DASH_PATTERN_TYPE_VG);
    }

    return stroke_item->vg.path;
}

void lv_vg_lite_stroke_drop(struct _lv_draw_vg_lite_unit_t * unit,
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
    LV_UNUSED(user_data);
    LV_ASSERT_NULL(item);

    /* Check if stroke width is valid */
    if(item->lv.width <= 0) {
        LV_LOG_WARN("stroke width error: %f", item->lv.width);
        return false;
    }

    /* Reset the dash pattern type */
    item->dash_pattern_type = DASH_PATTERN_TYPE_LV;

    /* dup the path */
    item->vg.path = lv_vg_lite_path_create(VG_LITE_FP32);
    lv_vg_lite_path_append_path(item->vg.path, item->lv.path);

    /* dup the dash pattern */
    vg_lite_float_t * vg_dash_pattern = NULL;
    const uint32_t size = lv_array_size(&item->lv.dash_pattern);
    if(size) {
        /* Only support float dash pattern */
        LV_ASSERT(item->lv.dash_pattern.element_size == sizeof(float));
        lv_array_init(&item->vg.dash_pattern, size, sizeof(float));
        lv_array_copy(&item->vg.dash_pattern, &item->lv.dash_pattern);

        /* mark dash pattern has been duped */
        item->dash_pattern_type = DASH_PATTERN_TYPE_VG;
        vg_dash_pattern = lv_array_front(&item->vg.dash_pattern);
        LV_ASSERT_NULL(vg_dash_pattern);
    }

    /* update parameters */
    vg_lite_path_t * vg_path = lv_vg_lite_path_get_path(item->vg.path);
    LV_VG_LITE_CHECK_ERROR(vg_lite_set_path_type(vg_path, VG_LITE_DRAW_STROKE_PATH), {});

    vg_lite_error_t error = vg_lite_set_stroke(
                                vg_path,
                                lv_stroke_cap_to_vg(item->lv.cap),
                                lv_stroke_join_to_vg(item->lv.join),
                                item->lv.width,
                                item->lv.miter_limit,
                                vg_dash_pattern,
                                size,
                                item->lv.width / 2,
                                0);

    if(error != VG_LITE_SUCCESS) {
        LV_LOG_ERROR("vg_lite_set_stroke error: %d", (int)error);
        lv_vg_lite_error_dump_info(error);
        stroke_free_cb(item, NULL);
        return false;
    }

    const vg_lite_pointer * ori_path = vg_path->path;
    const vg_lite_uint32_t ori_path_length = vg_path->path_length;

    LV_PROFILER_DRAW_BEGIN_TAG("vg_lite_update_stroke");
    error = vg_lite_update_stroke(vg_path);
    LV_PROFILER_DRAW_END_TAG("vg_lite_update_stroke");

    /* check if path is changed */
    LV_ASSERT_MSG(vg_path->path_length == ori_path_length, "vg_path->path_length should not change");
    LV_ASSERT_MSG(vg_path->path == ori_path, "vg_path->path should not change");

    if(error != VG_LITE_SUCCESS) {
        LV_LOG_ERROR("vg_lite_update_stroke error: %d", (int)error);
        lv_vg_lite_error_dump_info(error);
        stroke_free_cb(item, NULL);
        return false;
    }

    return true;
}

static void stroke_free_cb(stroke_item_t * item, void * user_data)
{
    LV_UNUSED(user_data);
    LV_ASSERT_NULL(item);

    if(item->vg.path) {
        lv_vg_lite_path_destroy(item->vg.path);
        item->vg.path = NULL;
    }

    if(item->dash_pattern_type == DASH_PATTERN_TYPE_VG) {
        lv_array_deinit(&item->vg.dash_pattern);
        item->dash_pattern_type = DASH_PATTERN_TYPE_LV;
    }
}

static lv_cache_compare_res_t dash_pattern_compare(const stroke_item_t * lhs, const stroke_item_t * rhs)
{
    /* Select the dash pattern to compare */
    const lv_array_t * lhs_dash_pattern = lhs->dash_pattern_type == DASH_PATTERN_TYPE_LV ?
                                          &lhs->lv.dash_pattern :
                                          &lhs->vg.dash_pattern;
    const lv_array_t * rhs_dash_pattern = rhs->dash_pattern_type == DASH_PATTERN_TYPE_LV ?
                                          &rhs->lv.dash_pattern :
                                          &rhs->vg.dash_pattern;

    const uint32_t lhs_dash_pattern_size = lv_array_size(lhs_dash_pattern);
    const uint32_t rhs_dash_pattern_size = lv_array_size(rhs_dash_pattern);

    if(lhs_dash_pattern_size != rhs_dash_pattern_size) {
        return lhs_dash_pattern_size > rhs_dash_pattern_size ? 1 : -1;
    }

    if(lhs_dash_pattern_size == 0 && rhs_dash_pattern_size == 0) {
        return 0;
    }

    /* Both dash pattern has the same size, compare them */
    LV_ASSERT(lhs_dash_pattern->element_size == sizeof(float));
    LV_ASSERT(rhs_dash_pattern->element_size == sizeof(float));

    /* compare dash pattern data */
    int cmp_res = lv_memcmp(
                      lv_array_front(lhs_dash_pattern),
                      lv_array_front(rhs_dash_pattern),
                      lhs_dash_pattern_size * sizeof(float));

    if(cmp_res != 0) {
        return cmp_res > 0 ? 1 : -1;
    }

    return 0;
}

static lv_cache_compare_res_t path_compare(const stroke_item_t * lhs, const stroke_item_t * rhs)
{
    /* Give priority to using dup vg.path */
    const vg_lite_path_t * lhs_path = lhs->vg.path ?
                                      lv_vg_lite_path_get_path(lhs->vg.path) :
                                      lv_vg_lite_path_get_path(lhs->lv.path);
    const vg_lite_path_t * rhs_path = rhs->vg.path ?
                                      lv_vg_lite_path_get_path(rhs->vg.path) :
                                      lv_vg_lite_path_get_path(rhs->lv.path);

    LV_ASSERT(lhs_path->format == VG_LITE_FP32);
    LV_ASSERT(rhs_path->format == VG_LITE_FP32);

    if(lhs_path->path_length != rhs_path->path_length) {
        return lhs_path->path_length > rhs_path->path_length ? 1 : -1;
    }

    int cmp_res = lv_memcmp(lhs_path->path, rhs_path->path, lhs_path->path_length);
    if(cmp_res != 0) {
        return cmp_res > 0 ? 1 : -1;
    }

    return 0;
}

static lv_cache_compare_res_t stroke_compare_cb(const stroke_item_t * lhs, const stroke_item_t * rhs)
{
    if(lhs->lv.width != rhs->lv.width) {
        return lhs->lv.width > rhs->lv.width ? 1 : -1;
    }

    if(lhs->lv.cap != rhs->lv.cap) {
        return lhs->lv.cap > rhs->lv.cap ? 1 : -1;
    }

    if(lhs->lv.join != rhs->lv.join) {
        return lhs->lv.join > rhs->lv.join ? 1 : -1;
    }

    if(lhs->lv.miter_limit != rhs->lv.miter_limit) {
        return lhs->lv.miter_limit > rhs->lv.miter_limit ? 1 : -1;
    }

    lv_cache_compare_res_t dash_pattern_res = dash_pattern_compare(lhs, rhs);
    if(dash_pattern_res != 0) {
        return dash_pattern_res;
    }

    return path_compare(lhs, rhs);
}

#endif /*LV_USE_DRAW_VG_LITE && LV_USE_VECTOR_GRAPHIC*/
