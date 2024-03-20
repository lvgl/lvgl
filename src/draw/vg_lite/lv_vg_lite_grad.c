/**
 * @file lv_vg_lite_grad.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_vg_lite_grad.h"

#if LV_USE_DRAW_VG_LITE

#include "lv_draw_vg_lite_type.h"
#include "lv_vg_lite_pending.h"
#include "lv_vg_lite_math.h"
#include "../../misc/lv_types.h"
#include "../../stdlib/lv_string.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_grad_dsc_t lv_grad;
    vg_lite_linear_gradient_t vg_grad;
} linear_grad_item_t;

#if LV_USE_VECTOR_GRAPHIC

typedef struct {
    lv_vector_gradient_t lv_grad;
    vg_lite_radial_gradient_t vg_grad;
} radial_grad_item_t;

#endif /* LV_USE_VECTOR_GRAPHIC */

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void grad_cache_release_cb(void * entry, void * user_data);

/* Linear gradient */

static vg_lite_linear_gradient_t * linear_grad_get(struct _lv_draw_vg_lite_unit_t * u,
                                                   const lv_grad_dsc_t * grad);
static bool linear_grad_create_cb(linear_grad_item_t * item, void * user_data);
static void linear_grad_free_cb(linear_grad_item_t * item, void * user_data);
static lv_cache_compare_res_t linear_grad_compare_cb(const linear_grad_item_t * lhs, const linear_grad_item_t * rhs);

#if LV_USE_VECTOR_GRAPHIC

/* Radial gradient */

static vg_lite_radial_gradient_t * radial_grad_get(struct _lv_draw_vg_lite_unit_t * u,
                                                   const lv_vector_gradient_t * grad);
static bool radial_grad_create_cb(radial_grad_item_t * item, void * user_data);
static void radial_grad_free_cb(radial_grad_item_t * item, void * user_data);
static lv_cache_compare_res_t radial_grad_compare_cb(const radial_grad_item_t * lhs, const radial_grad_item_t * rhs);

static vg_lite_gradient_spreadmode_t lv_spread_to_vg(lv_vector_gradient_spread_t spread);

#endif /* LV_USE_VECTOR_GRAPHIC */

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_vg_lite_grad_init(
    struct _lv_draw_vg_lite_unit_t * u,
    uint32_t linear_grad_cache_cnt,
    uint32_t radial_grad_cache_cnt)
{
    LV_ASSERT_NULL(u);
    LV_UNUSED(radial_grad_cache_cnt);

    /* Create the cache for linear gradients */
    {
        lv_cache_ops_t ops = {
            .compare_cb = (lv_cache_compare_cb_t)linear_grad_compare_cb,
            .create_cb = (lv_cache_create_cb_t)linear_grad_create_cb,
            .free_cb = (lv_cache_free_cb_t)linear_grad_free_cb,
        };

        u->linear_grad_cache = lv_cache_create(&lv_cache_class_lru_rb_count, sizeof(linear_grad_item_t),
                                               linear_grad_cache_cnt,
                                               ops);
        u->linear_grad_pending = lv_vg_lite_pending_create(sizeof(lv_cache_entry_t *), 4);
        lv_vg_lite_pending_set_free_cb(u->linear_grad_pending, grad_cache_release_cb, u->linear_grad_cache);
    }

#if LV_USE_VECTOR_GRAPHIC

    /* Create the cache for radial gradients */
    if(vg_lite_query_feature(gcFEATURE_BIT_VG_RADIAL_GRADIENT)) {
        lv_cache_ops_t ops = {
            .compare_cb = (lv_cache_compare_cb_t)radial_grad_compare_cb,
            .create_cb = (lv_cache_create_cb_t)radial_grad_create_cb,
            .free_cb = (lv_cache_free_cb_t)radial_grad_free_cb,
        };

        u->radial_grad_cache = lv_cache_create(&lv_cache_class_lru_rb_count, sizeof(radial_grad_item_t),
                                               radial_grad_cache_cnt,
                                               ops);
        u->radial_grad_pending = lv_vg_lite_pending_create(sizeof(lv_cache_entry_t *), 4);
        lv_vg_lite_pending_set_free_cb(u->radial_grad_pending, grad_cache_release_cb, u->radial_grad_cache);
    }

#endif /* LV_USE_VECTOR_GRAPHIC */
}

void lv_vg_lite_grad_deinit(struct _lv_draw_vg_lite_unit_t * u)
{
    LV_ASSERT_NULL(u);
    lv_vg_lite_pending_destroy(u->linear_grad_pending);
    u->linear_grad_pending = NULL;
    lv_cache_destroy(u->linear_grad_cache, NULL);
    u->linear_grad_cache = NULL;

    if(u->radial_grad_pending) {
        lv_vg_lite_pending_destroy(u->radial_grad_pending);
        u->radial_grad_pending = NULL;
        lv_cache_destroy(u->radial_grad_cache, NULL);
        u->radial_grad_cache = NULL;
    }
}

void lv_vg_lite_grad_area_to_matrix(vg_lite_matrix_t * grad_matrix, const lv_area_t * area, lv_grad_dir_t dir)
{
    LV_ASSERT_NULL(grad_matrix);
    LV_ASSERT_NULL(area);

    vg_lite_identity(grad_matrix);
    vg_lite_translate(area->x1, area->y1, grad_matrix);

    switch(dir) {
        case LV_GRAD_DIR_VER:
            vg_lite_scale(1, lv_area_get_height(area) / 256.0f, grad_matrix);
            vg_lite_rotate(90, grad_matrix);
            break;

        case LV_GRAD_DIR_HOR:
            vg_lite_scale(lv_area_get_width(area) / 256.0f, 1, grad_matrix);
            break;

        default:
            break;
    }
}

void lv_vg_lite_draw_linear_grad(
    struct _lv_draw_vg_lite_unit_t * u,
    vg_lite_buffer_t * buffer,
    vg_lite_path_t * path,
    const lv_grad_dsc_t * grad,
    const vg_lite_matrix_t * grad_matrix,
    const vg_lite_matrix_t * matrix,
    vg_lite_fill_t fill,
    vg_lite_blend_t blend)
{
    LV_ASSERT_NULL(u);
    LV_ASSERT_NULL(buffer);
    LV_ASSERT_NULL(path);
    LV_ASSERT_NULL(grad);
    LV_ASSERT_NULL(grad_matrix);
    LV_ASSERT_NULL(matrix);

    LV_PROFILER_BEGIN;

    vg_lite_linear_gradient_t * linear_grad = linear_grad_get(u, grad);
    LV_ASSERT_NULL(linear_grad);
    if(!linear_grad) {
        LV_LOG_ERROR("Failed to get linear gradient");
        LV_PROFILER_END;
        return;
    }

    vg_lite_matrix_t * grad_mat_p = vg_lite_get_grad_matrix(linear_grad);
    LV_ASSERT_NULL(grad_mat_p);
    *grad_mat_p = *grad_matrix;

    LV_VG_LITE_ASSERT_DEST_BUFFER(buffer);
    LV_VG_LITE_ASSERT_SRC_BUFFER(&linear_grad->image);
    LV_VG_LITE_ASSERT_PATH(path);
    LV_VG_LITE_ASSERT_MATRIX(grad_mat_p);
    LV_VG_LITE_ASSERT_MATRIX(matrix);

    LV_PROFILER_BEGIN_TAG("vg_lite_draw_grad");
    LV_VG_LITE_CHECK_ERROR(vg_lite_draw_grad(
                               buffer,
                               path,
                               fill,
                               (vg_lite_matrix_t *)matrix,
                               linear_grad,
                               blend));
    LV_PROFILER_END_TAG("vg_lite_draw_grad");

    LV_PROFILER_END;
}

#if LV_USE_VECTOR_GRAPHIC

void lv_vg_lite_draw_radial_grad(
    struct _lv_draw_vg_lite_unit_t * u,
    vg_lite_buffer_t * buffer,
    vg_lite_path_t * path,
    const lv_vector_gradient_t * grad,
    const vg_lite_matrix_t * grad_matrix,
    const vg_lite_matrix_t * matrix,
    vg_lite_fill_t fill,
    vg_lite_blend_t blend)
{
    LV_ASSERT_NULL(u);
    LV_ASSERT_NULL(buffer);
    LV_ASSERT_NULL(path);
    LV_ASSERT_NULL(grad);
    LV_ASSERT_NULL(grad_matrix);
    LV_ASSERT_NULL(matrix);

    if(!vg_lite_query_feature(gcFEATURE_BIT_VG_RADIAL_GRADIENT)) {
        LV_LOG_INFO("radial gradient is not supported");
        return;
    }

    if(grad->spread == LV_VECTOR_GRADIENT_SPREAD_REPEAT || grad->spread == LV_VECTOR_GRADIENT_SPREAD_REFLECT) {
        if(!vg_lite_query_feature(gcFEATURE_BIT_VG_IM_REPEAT_REFLECT)) {
            LV_LOG_INFO("repeat/reflect spread(%d) is not supported", (int)grad->spread);
            return;
        }
    }

    LV_PROFILER_BEGIN;

    vg_lite_radial_gradient_t * radial_grad = radial_grad_get(u, grad);

    vg_lite_matrix_t * grad_mat_p = vg_lite_get_radial_grad_matrix(radial_grad);
    LV_ASSERT_NULL(grad_mat_p);
    *grad_mat_p = *grad_matrix;

    LV_VG_LITE_ASSERT_DEST_BUFFER(buffer);
    LV_VG_LITE_ASSERT_SRC_BUFFER(&radial_grad->image);
    LV_VG_LITE_ASSERT_PATH(path);
    LV_VG_LITE_ASSERT_MATRIX(grad_mat_p);
    LV_VG_LITE_ASSERT_MATRIX(matrix);

    LV_PROFILER_BEGIN_TAG("vg_lite_draw_radial_grad");
    LV_VG_LITE_CHECK_ERROR(
        vg_lite_draw_radial_grad(
            buffer,
            path,
            fill,
            (vg_lite_matrix_t *)matrix,
            radial_grad,
            0,
            blend,
            VG_LITE_FILTER_LINEAR));
    LV_PROFILER_END_TAG("vg_lite_draw_radial_grad");

    LV_PROFILER_END;
}

#endif /* LV_USE_VECTOR_GRAPHIC */

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void * grad_get(
    struct _lv_draw_vg_lite_unit_t * u,
    lv_cache_t * cache,
    lv_vg_lite_pending_t * pending,
    const void * key)
{
    LV_ASSERT_NULL(u);
    LV_ASSERT_NULL(cache);
    LV_ASSERT_NULL(pending);
    LV_ASSERT_NULL(key);

    lv_cache_entry_t * cache_node_entry = lv_cache_acquire(cache, key, NULL);
    if(cache_node_entry == NULL) {
        /* check if the cache is full */
        size_t free_size = lv_cache_get_free_size(cache, NULL);
        if(free_size == 0) {
            LV_LOG_INFO("grad cache is full, release all pending cache entries");
            lv_vg_lite_finish(u);
        }

        cache_node_entry = lv_cache_acquire_or_create(cache, key, NULL);
        if(cache_node_entry == NULL) {
            LV_LOG_ERROR("grad cache creating failed");
            return NULL;
        }
    }

    /* Add the new entry to the pending list */
    lv_vg_lite_pending_add(pending, &cache_node_entry);

    return lv_cache_entry_get_data(cache_node_entry);
}

static void grad_cache_release_cb(void * entry, void * user_data)
{
    lv_cache_entry_t ** entry_p = entry;
    lv_cache_t * cache = user_data;
    lv_cache_release(cache, *entry_p, NULL);
}

/* Linear gradient */

static vg_lite_linear_gradient_t * linear_grad_get(struct _lv_draw_vg_lite_unit_t * u,
                                                   const lv_grad_dsc_t * grad)
{
    linear_grad_item_t search_key;
    lv_memzero(&search_key, sizeof(search_key));
    search_key.lv_grad = *grad;

    linear_grad_item_t * item = grad_get(u, u->linear_grad_cache, u->linear_grad_pending, &search_key);
    if(!item) {
        return NULL;
    }

    return &item->vg_grad;
}

static bool linear_grad_create_cb(linear_grad_item_t * item, void * user_data)
{
    LV_UNUSED(user_data);

    LV_PROFILER_BEGIN;

    vg_lite_error_t err = vg_lite_init_grad(&item->vg_grad);
    if(err != VG_LITE_SUCCESS) {
        LV_PROFILER_END;
        LV_LOG_ERROR("init grad error(%d): %s", (int)err, lv_vg_lite_error_string(err));
        return false;
    }

    vg_lite_uint32_t colors[VLC_MAX_GRADIENT_STOPS];
    vg_lite_uint32_t stops[VLC_MAX_GRADIENT_STOPS];

    /* Gradient setup */
    uint8_t cnt = item->lv_grad.stops_count;
    LV_ASSERT(cnt < VLC_MAX_GRADIENT_STOPS);
    for(uint8_t i = 0; i < cnt; i++) {
        stops[i] = item->lv_grad.stops[i].frac;
        const lv_color_t * c = &item->lv_grad.stops[i].color;
        lv_opa_t opa = item->lv_grad.stops[i].opa;

        /* lvgl color -> gradient color */
        lv_color_t grad_color = lv_color_make(c->blue, c->green, c->red);
        colors[i] = lv_vg_lite_color(grad_color, opa, true);
    }

    LV_VG_LITE_CHECK_ERROR(vg_lite_set_grad(&item->vg_grad, cnt, colors, stops));

    LV_PROFILER_BEGIN_TAG("vg_lite_update_grad");
    LV_VG_LITE_CHECK_ERROR(vg_lite_update_grad(&item->vg_grad));
    LV_PROFILER_END_TAG("vg_lite_update_grad");

    LV_PROFILER_END;
    return true;
}

static void linear_grad_free_cb(linear_grad_item_t * item, void * user_data)
{
    LV_UNUSED(user_data);
    LV_VG_LITE_CHECK_ERROR(vg_lite_clear_grad(&item->vg_grad));
}

static lv_cache_compare_res_t linear_grad_compare_cb(const linear_grad_item_t * lhs, const linear_grad_item_t * rhs)
{
    if(lhs->lv_grad.stops_count != rhs->lv_grad.stops_count) {
        return lhs->lv_grad.stops_count > rhs->lv_grad.stops_count ? 1 : -1;
    }

    int cmp_res = lv_memcmp(lhs->lv_grad.stops, rhs->lv_grad.stops,
                            sizeof(lv_gradient_stop_t) * lhs->lv_grad.stops_count);
    if(cmp_res != 0) {
        return cmp_res > 0 ? 1 : -1;
    }

    return 0;
}

#if LV_USE_VECTOR_GRAPHIC

/* Radial gradient */

static vg_lite_radial_gradient_t * radial_grad_get(struct _lv_draw_vg_lite_unit_t * u,
                                                   const lv_vector_gradient_t * grad)
{
    radial_grad_item_t search_key;
    lv_memzero(&search_key, sizeof(search_key));
    search_key.lv_grad = *grad;

    radial_grad_item_t * item = grad_get(u, u->radial_grad_cache, u->radial_grad_pending, &search_key);
    if(!item) {
        return NULL;
    }

    return &item->vg_grad;
}

static bool radial_grad_create_cb(radial_grad_item_t * item, void * user_data)
{
    LV_UNUSED(user_data);

    LV_PROFILER_BEGIN;

    lv_vector_gradient_t * grad = &item->lv_grad;
    uint8_t stops_count = grad->grad.stops_count;
    vg_lite_color_ramp_t * color_ramp = lv_malloc(sizeof(vg_lite_color_ramp_t) * stops_count);
    LV_ASSERT_MALLOC(color_ramp);
    if(!color_ramp) {
        LV_LOG_ERROR("malloc failed for color_ramp");
        return false;
    }

    for(uint8_t i = 0; i < stops_count; i++) {
        color_ramp[i].stop = grad->grad.stops[i].frac / 255.0f;
        lv_color_t c = grad->grad.stops[i].color;

        color_ramp[i].red = c.red / 255.0f;
        color_ramp[i].green = c.green / 255.0f;
        color_ramp[i].blue = c.blue / 255.0f;
        color_ramp[i].alpha = grad->grad.stops[i].opa / 255.0f;
    }

    const vg_lite_radial_gradient_parameter_t grad_param = {
        .cx = grad->cx,
        .cy = grad->cy,
        .r = grad->cr,
        .fx = grad->cx,
        .fy = grad->cy,
    };

    vg_lite_radial_gradient_t radial_grad;
    lv_memzero(&radial_grad, sizeof(radial_grad));

    LV_PROFILER_BEGIN_TAG("vg_lite_set_radial_grad");
    LV_VG_LITE_CHECK_ERROR(
        vg_lite_set_radial_grad(
            &radial_grad,
            stops_count,
            color_ramp,
            grad_param,
            lv_spread_to_vg(grad->spread),
            1));
    LV_PROFILER_END_TAG("vg_lite_set_radial_grad");

    LV_PROFILER_BEGIN_TAG("vg_lite_update_radial_grad");
    vg_lite_error_t err = vg_lite_update_radial_grad(&radial_grad);
    LV_PROFILER_END_TAG("vg_lite_update_radial_grad");
    if(!err) {
        item->vg_grad = radial_grad;
    }
    else {
        LV_LOG_ERROR("update radial grad error(%d): %s", (int)err, lv_vg_lite_error_string(err));
    }

    lv_free(color_ramp);

    LV_PROFILER_END;
    return err == VG_LITE_SUCCESS;
}

static void radial_grad_free_cb(radial_grad_item_t * item, void * user_data)
{
    LV_UNUSED(user_data);
    LV_VG_LITE_CHECK_ERROR(vg_lite_clear_radial_grad(&item->vg_grad));
}

static lv_cache_compare_res_t radial_grad_compare_cb(const radial_grad_item_t * lhs, const radial_grad_item_t * rhs)
{
    if(!math_equal(lhs->lv_grad.cx, rhs->lv_grad.cx)) {
        return lhs->lv_grad.cx > rhs->lv_grad.cx ? 1 : -1;
    }

    if(!math_equal(lhs->lv_grad.cy, rhs->lv_grad.cy)) {
        return lhs->lv_grad.cy > rhs->lv_grad.cy ? 1 : -1;
    }

    if(!math_equal(lhs->lv_grad.cr, rhs->lv_grad.cr)) {
        return lhs->lv_grad.cr > rhs->lv_grad.cr ? 1 : -1;
    }

    if(lhs->lv_grad.spread != rhs->lv_grad.spread) {
        return lhs->lv_grad.spread > rhs->lv_grad.spread ? 1 : -1;
    }

    if(lhs->lv_grad.grad.stops_count != rhs->lv_grad.grad.stops_count) {
        return lhs->lv_grad.grad.stops_count > rhs->lv_grad.grad.stops_count ? 1 : -1;
    }

    int cmp_res = lv_memcmp(lhs->lv_grad.grad.stops, rhs->lv_grad.grad.stops,
                            sizeof(lv_gradient_stop_t) * lhs->lv_grad.grad.stops_count);
    if(cmp_res != 0) {
        return cmp_res > 0 ? 1 : -1;
    }

    return 0;
}

static vg_lite_gradient_spreadmode_t lv_spread_to_vg(lv_vector_gradient_spread_t spread)
{
    switch(spread) {
        case LV_VECTOR_GRADIENT_SPREAD_PAD:
            return VG_LITE_GRADIENT_SPREAD_PAD;
        case LV_VECTOR_GRADIENT_SPREAD_REPEAT:
            return VG_LITE_GRADIENT_SPREAD_REPEAT;
        case LV_VECTOR_GRADIENT_SPREAD_REFLECT:
            return VG_LITE_GRADIENT_SPREAD_REFLECT;
        default:
            return VG_LITE_GRADIENT_SPREAD_FILL;
    }
}

#endif /* LV_USE_VECTOR_GRAPHIC */

#endif /*LV_USE_DRAW_VG_LITE*/
