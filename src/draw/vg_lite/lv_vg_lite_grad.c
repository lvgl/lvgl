/**
 * @file lv_vg_lite_grad.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../lv_draw_vector_private.h"
#include "lv_vg_lite_grad.h"

#if LV_USE_DRAW_VG_LITE && LV_USE_VECTOR_GRAPHIC

#include "lv_draw_vg_lite_type.h"
#include "lv_vg_lite_pending.h"
#include "lv_vg_lite_math.h"
#include "../../misc/lv_types.h"
#include "../../stdlib/lv_string.h"

/*********************
 *      DEFINES
 *********************/

#define SQUARE(x) ((x)*(x))

#ifndef M_PI
    #define M_PI 3.1415926f
#endif

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    GRAD_TYPE_FREE,
    GRAD_TYPE_LINEAR,
    GRAD_TYPE_LINEAR_EXT,
    GRAD_TYPE_RADIAL,
    GRAD_TYPE_UNKNOWN,
} grad_type_t;

struct _lv_vg_lite_grad_ctx_t;

typedef struct {
    struct _lv_vg_lite_grad_ctx_t * ctx;
    grad_type_t type;
    lv_vector_gradient_t lv;
    union {
        vg_lite_linear_gradient_t linear;
        vg_lite_ext_linear_gradient_t linear_ext;
        vg_lite_radial_gradient_t radial;
    } vg;
} grad_item_t;

typedef grad_item_t * grad_item_ref_t;

typedef struct _lv_vg_lite_grad_ctx_t {
    struct _lv_draw_vg_lite_unit_t * unit;
    lv_cache_t * cache;
    struct _lv_vg_lite_pending_t * pending;
    lv_ll_t item_pool;

    /**
     * Temporary reuse of data to reduce the use of
     * large memory allocations on the heap and stack during runtime
     */
    grad_item_t local_grad_item;
    vg_lite_color_ramp_t local_color_ramp[VLC_MAX_COLOR_RAMP_STOPS];
} lv_vg_lite_grad_ctx_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static grad_item_t * grad_get(lv_vg_lite_grad_ctx_t * ctx, const lv_vector_gradient_t * grad);
static void grad_cache_release_cb(void * entry, void * user_data);
static bool grad_create_cb(grad_item_ref_t * item_ref, void * user_data);
static void grad_free_cb(grad_item_ref_t * item_ref, void * user_data);
static lv_cache_compare_res_t grad_compare_cb(const grad_item_ref_t * lhs_ref, const grad_item_ref_t * rhs_ref);

static grad_type_t lv_grad_style_to_type(lv_vector_gradient_style_t style);
static void grad_point_to_matrix(vg_lite_matrix_t * grad_matrix, float x1, float y1, float x2, float y2);
static vg_lite_gradient_spreadmode_t lv_spread_to_vg(lv_vector_gradient_spread_t spread);

static void lv_vg_lite_linear_gradient_dump_info(const vg_lite_linear_gradient_t * grad);
static void lv_vg_lite_ext_linear_gradient_dump_info(const vg_lite_ext_linear_gradient_t * grad);
static void lv_vg_lite_radial_gradient_dump_info(const vg_lite_radial_gradient_t * grad);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

struct _lv_vg_lite_grad_ctx_t * lv_vg_lite_grad_ctx_create(uint32_t cache_cnt, struct _lv_draw_vg_lite_unit_t * unit)
{
    LV_ASSERT_MSG(cache_cnt > 0, "cache_cnt should be greater than 0");

    lv_cache_ops_t ops = {
        .compare_cb = (lv_cache_compare_cb_t)grad_compare_cb,
        .create_cb = (lv_cache_create_cb_t)grad_create_cb,
        .free_cb = (lv_cache_free_cb_t)grad_free_cb,
    };

    lv_vg_lite_grad_ctx_t * ctx = lv_malloc_zeroed(sizeof(lv_vg_lite_grad_ctx_t));
    LV_ASSERT_MALLOC(ctx);
    ctx->unit = unit;

    lv_ll_init(&ctx->item_pool, sizeof(grad_item_t));

    ctx->cache = lv_cache_create(&lv_cache_class_lru_ll_count, sizeof(grad_item_ref_t), cache_cnt, ops);
    lv_cache_set_name(ctx->cache, "VG_GRAD");
    ctx->pending = lv_vg_lite_pending_create(sizeof(lv_cache_entry_t *), 4);
    lv_vg_lite_pending_set_free_cb(ctx->pending, grad_cache_release_cb, ctx->cache);

    return ctx;
}

void lv_vg_lite_grad_ctx_delete(struct _lv_vg_lite_grad_ctx_t * ctx)
{
    LV_ASSERT_NULL(ctx);
    lv_vg_lite_pending_destroy(ctx->pending);
    lv_cache_destroy(ctx->cache, NULL);
    lv_ll_clear(&ctx->item_pool);

    lv_memzero(ctx, sizeof(lv_vg_lite_grad_ctx_t));
    lv_free(ctx);
}

struct _lv_vg_lite_pending_t * lv_vg_lite_grad_ctx_get_pending(struct _lv_vg_lite_grad_ctx_t * ctx)
{
    LV_ASSERT_NULL(ctx);
    return ctx->pending;
}

bool lv_vg_lite_draw_grad(
    struct _lv_vg_lite_grad_ctx_t * ctx,
    vg_lite_buffer_t * buffer,
    vg_lite_path_t * path,
    const lv_vector_gradient_t * grad,
    const vg_lite_matrix_t * grad_matrix,
    const vg_lite_matrix_t * matrix,
    vg_lite_fill_t fill,
    vg_lite_blend_t blend)
{
    LV_ASSERT_NULL(ctx);
    LV_VG_LITE_ASSERT_DEST_BUFFER(buffer);
    LV_VG_LITE_ASSERT_PATH(path);
    LV_ASSERT_NULL(grad);
    LV_VG_LITE_ASSERT_MATRIX(grad_matrix);
    LV_VG_LITE_ASSERT_MATRIX(matrix);

    /* check radial gradient is supported */
    if(grad->style == LV_VECTOR_GRADIENT_STYLE_RADIAL) {
        if(!vg_lite_query_feature(gcFEATURE_BIT_VG_RADIAL_GRADIENT)) {
            LV_LOG_INFO("radial gradient is not supported");
            return false;
        }

        /* check if the radius is valid */
        if(grad->cr <= 0) {
            LV_LOG_INFO("radius: %f is not valid", grad->cr);
            return false;
        }
    }

    /* check spread mode is supported */
    if(grad->spread == LV_VECTOR_GRADIENT_SPREAD_REPEAT || grad->spread == LV_VECTOR_GRADIENT_SPREAD_REFLECT) {
        if(!vg_lite_query_feature(gcFEATURE_BIT_VG_IM_REPEAT_REFLECT)) {
            LV_LOG_INFO("repeat/reflect spread(%d) is not supported", grad->spread);
            return false;
        }
    }

    grad_item_t * grad_item = grad_get(ctx, grad);
    if(!grad_item) {
        LV_LOG_WARN("Failed to get gradient, style: %d", grad->style);
        return false;
    }
    LV_ASSERT(grad_item->lv.style == grad->style);

    switch(grad_item->type) {
        case GRAD_TYPE_LINEAR: {
                vg_lite_linear_gradient_t * linear_grad = &grad_item->vg.linear;
                vg_lite_matrix_t * grad_mat_p = vg_lite_get_grad_matrix(linear_grad);
                LV_ASSERT_NULL(grad_mat_p);
                vg_lite_identity(grad_mat_p);
                lv_vg_lite_matrix_multiply(grad_mat_p, grad_matrix);
                grad_point_to_matrix(grad_mat_p, grad->x1, grad->y1, grad->x2, grad->y2);

                LV_PROFILER_DRAW_BEGIN_TAG("vg_lite_draw_grad");
                LV_VG_LITE_CHECK_ERROR(vg_lite_draw_grad(
                                           buffer,
                                           path,
                                           fill,
                                           (vg_lite_matrix_t *)matrix,
                                           linear_grad,
                                           blend),
                                       /*Error handler*/
                {
                    lv_vg_lite_buffer_dump_info(buffer);
                    lv_vg_lite_path_dump_info(path);
                    LV_LOG_ERROR("fill_rule: 0x%X", (int)fill);
                    lv_vg_lite_matrix_dump_info(matrix);
                    lv_vg_lite_linear_gradient_dump_info(linear_grad);
                    LV_LOG_ERROR("blend_mode: 0x%X", (int)blend);
                });
                LV_PROFILER_DRAW_END_TAG("vg_lite_draw_grad");
            }
            break;
        case GRAD_TYPE_LINEAR_EXT: {
                vg_lite_ext_linear_gradient_t * linear_grad = &grad_item->vg.linear_ext;
                vg_lite_matrix_t * grad_mat_p = vg_lite_get_linear_grad_matrix(linear_grad);
                LV_ASSERT_NULL(grad_mat_p);
                *grad_mat_p = *grad_matrix;

                LV_PROFILER_DRAW_BEGIN_TAG("vg_lite_draw_linear_grad");
                LV_VG_LITE_CHECK_ERROR(vg_lite_draw_linear_grad(
                                           buffer,
                                           path,
                                           fill,
                                           (vg_lite_matrix_t *)matrix,
                                           linear_grad,
                                           0,
                                           blend,
                                           VG_LITE_FILTER_LINEAR),
                                       /*Error handler*/
                {
                    lv_vg_lite_buffer_dump_info(buffer);
                    lv_vg_lite_path_dump_info(path);
                    LV_LOG_ERROR("fill_rule: 0x%X", (int)fill);
                    lv_vg_lite_matrix_dump_info(matrix);
                    lv_vg_lite_ext_linear_gradient_dump_info(linear_grad);
                    LV_LOG_ERROR("blend_mode: 0x%X", (int)blend);
                });
                LV_PROFILER_DRAW_END_TAG("vg_lite_draw_linear_grad");
            }
            break;

        case GRAD_TYPE_RADIAL: {
                vg_lite_radial_gradient_t * radial_gradient = &grad_item->vg.radial;
                vg_lite_matrix_t * grad_mat_p = vg_lite_get_radial_grad_matrix(radial_gradient);
                LV_ASSERT_NULL(grad_mat_p);
                *grad_mat_p = *grad_matrix;

                LV_PROFILER_DRAW_BEGIN_TAG("vg_lite_draw_radial_grad");
                LV_VG_LITE_CHECK_ERROR(
                    vg_lite_draw_radial_grad(
                        buffer,
                        path,
                        fill,
                        (vg_lite_matrix_t *)matrix,
                        radial_gradient,
                        0,
                        blend,
                        VG_LITE_FILTER_LINEAR),
                    /*Error handler*/
                {
                    lv_vg_lite_buffer_dump_info(buffer);
                    lv_vg_lite_path_dump_info(path);
                    LV_LOG_ERROR("fill_rule: 0x%X", (int)fill);
                    lv_vg_lite_matrix_dump_info(matrix);
                    lv_vg_lite_radial_gradient_dump_info(radial_gradient);
                    LV_LOG_ERROR("blend_mode: 0x%X", (int)blend);
                });
                LV_PROFILER_DRAW_END_TAG("vg_lite_draw_radial_grad");
            }
            break;

        default:
            LV_LOG_ERROR("Unsupported gradient type: %d", grad_item->type);
            return false;
    }

    return true;
}

bool lv_vg_lite_draw_grad_helper(
    struct _lv_vg_lite_grad_ctx_t * ctx,
    vg_lite_buffer_t * buffer,
    vg_lite_path_t * path,
    const lv_area_t * area,
    const lv_grad_dsc_t * grad_dsc,
    const vg_lite_matrix_t * matrix,
    vg_lite_fill_t fill,
    vg_lite_blend_t blend)
{
    LV_ASSERT_NULL(ctx);
    LV_VG_LITE_ASSERT_DEST_BUFFER(buffer);
    LV_VG_LITE_ASSERT_PATH(path);
    LV_ASSERT_NULL(area);
    LV_ASSERT_NULL(grad_dsc);
    LV_VG_LITE_ASSERT_MATRIX(matrix);

    lv_vector_gradient_t grad;
    lv_memzero(&grad, sizeof(grad));

    grad.style = LV_VECTOR_GRADIENT_STYLE_LINEAR;
    grad.stops_count = grad_dsc->stops_count;
    lv_memcpy(grad.stops, grad_dsc->stops, sizeof(lv_grad_stop_t) * grad_dsc->stops_count);

    /*convert to spread mode*/
#if LV_USE_DRAW_SW_COMPLEX_GRADIENTS
    switch(grad_dsc->extend) {
        case LV_GRAD_EXTEND_PAD:
            grad.spread = LV_VECTOR_GRADIENT_SPREAD_PAD;
            break;
        case LV_GRAD_EXTEND_REPEAT:
            grad.spread = LV_VECTOR_GRADIENT_SPREAD_REPEAT;
            break;
        case LV_GRAD_EXTEND_REFLECT:
            grad.spread = LV_VECTOR_GRADIENT_SPREAD_REFLECT;
            break;
        default:
            LV_LOG_WARN("Unsupported gradient extend mode: %d", grad_dsc->extend);
            grad.spread = LV_VECTOR_GRADIENT_SPREAD_PAD;
            break;
    }
#else
    grad.spread = LV_VECTOR_GRADIENT_SPREAD_PAD;
#endif

    switch(grad_dsc->dir) {
        case LV_GRAD_DIR_VER:
            grad.x1 = area->x1;
            grad.y1 = area->y1;
            grad.x2 = area->x1;
            grad.y2 = area->y2 + 1;
            break;

        case LV_GRAD_DIR_HOR:
            grad.x1 = area->x1;
            grad.y1 = area->y1;
            grad.x2 = area->x2 + 1;
            grad.y2 = area->y1;
            break;

#if LV_USE_DRAW_SW_COMPLEX_GRADIENTS
        case LV_GRAD_DIR_LINEAR: {
                int32_t w = lv_area_get_width(area);
                int32_t h = lv_area_get_height(area);

                grad.x1 = lv_pct_to_px(grad_dsc->params.linear.start.x, w) + area->x1;
                grad.y1 = lv_pct_to_px(grad_dsc->params.linear.start.y, h) + area->y1;
                grad.x2 = lv_pct_to_px(grad_dsc->params.linear.end.x, w) + area->x1;
                grad.y2 = lv_pct_to_px(grad_dsc->params.linear.end.y, h) + area->y1;
            }
            break;

        case LV_GRAD_DIR_RADIAL: {
                grad.style = LV_VECTOR_GRADIENT_STYLE_RADIAL;
                int32_t w = lv_area_get_width(area);
                int32_t h = lv_area_get_height(area);

                grad.cx = lv_pct_to_px(grad_dsc->params.radial.focal.x, w) + area->x1;
                grad.cy = lv_pct_to_px(grad_dsc->params.radial.focal.y, h) + area->y1;
                int32_t end_extent_x = lv_pct_to_px(grad_dsc->params.radial.end_extent.x, w) + area->x1;
                int32_t end_extent_y = lv_pct_to_px(grad_dsc->params.radial.end_extent.y, h) + area->y1;
                grad.cr = LV_MAX(end_extent_x - grad.cx, end_extent_y - grad.cy);
            }
            break;
#endif

        default:
            LV_LOG_WARN("Unsupported gradient direction: %d", grad_dsc->dir);
            return false;
    }

    return lv_vg_lite_draw_grad(ctx, buffer, path, &grad, matrix, matrix, fill, blend);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static grad_item_t * grad_get(lv_vg_lite_grad_ctx_t * ctx, const lv_vector_gradient_t * grad)
{
    LV_PROFILER_DRAW_BEGIN;
    LV_ASSERT_NULL(ctx);
    LV_ASSERT_NULL(grad);

    grad_item_ref_t search_key = &ctx->local_grad_item;
    lv_memzero(search_key, sizeof(ctx->local_grad_item));
    search_key->type = lv_grad_style_to_type(grad->style);
    search_key->lv = *grad;

    lv_cache_entry_t * cache_node_entry = lv_cache_acquire(ctx->cache, &search_key, ctx);
    if(cache_node_entry == NULL) {
        /* check if the cache is full */
        size_t free_size = lv_cache_get_free_size(ctx->cache, NULL);
        if(free_size == 0) {
            LV_LOG_INFO("grad cache is full, release all pending cache entries");
            lv_vg_lite_finish(ctx->unit);
        }

        cache_node_entry = lv_cache_acquire_or_create(ctx->cache, &search_key, ctx);
        if(cache_node_entry == NULL) {
            LV_LOG_ERROR("grad cache creating failed");
            LV_PROFILER_DRAW_END;
            return NULL;
        }
    }

    /* Add the new entry to the pending list */
    lv_vg_lite_pending_add(ctx->pending, &cache_node_entry);

    grad_item_ref_t * grad_item_ref = lv_cache_entry_get_data(cache_node_entry);

    LV_PROFILER_DRAW_END;
    return *grad_item_ref;
}

static grad_item_t * grad_item_pool_alloc(lv_vg_lite_grad_ctx_t * ctx, grad_type_t type)
{
    LV_ASSERT_NULL(ctx);

    grad_item_t * item = lv_ll_get_head(&ctx->item_pool);

    /* Try to obtain a free node from the head */
    if(item && item->type == GRAD_TYPE_FREE) {
        lv_ll_move_before(&ctx->item_pool, item, NULL);
        LV_LOG_TRACE("reuse item: %p, type: %d", (void *)item, type);
    }
    else {
        /* Allocate a new node if the pool is empty or all nodes are in use */
        item = lv_ll_ins_tail(&ctx->item_pool);
        LV_ASSERT_MALLOC(item);
        if(!item) {
            LV_LOG_ERROR("alloc grad item failed");
            return NULL;
        }

        LV_LOG_TRACE("alloc new item: %p, type: %d, pool size: %" LV_PRIu32,
                     (void *)item, type, lv_ll_get_len(&ctx->item_pool));
    }

    lv_memzero(item, sizeof(grad_item_t));
    item->ctx = ctx;
    item->type = type;
    return item;
}

static void grad_item_pool_free(grad_item_t * item)
{
    LV_ASSERT_NULL(item);
    LV_ASSERT_NULL(item->ctx);

    /* Move the free nodes to the head to ensure quick allocation */
    item->type = GRAD_TYPE_FREE;
    grad_item_t * head = lv_ll_get_head(&item->ctx->item_pool);
    lv_ll_move_before(&item->ctx->item_pool, item, head);
}

static void grad_cache_release_cb(void * entry, void * user_data)
{
    lv_cache_entry_t ** entry_p = entry;
    lv_cache_t * cache = user_data;
    lv_cache_release(cache, * entry_p, NULL);
}

static void convert_color_ramp(vg_lite_color_ramp_t * color_ramp, const lv_vector_gradient_t * grad)
{
    LV_ASSERT_NULL(color_ramp);
    LV_ASSERT_NULL(grad);

    for(uint16_t i = 0; i < grad->stops_count; i++) {
        color_ramp[i].stop = grad->stops[i].frac / 255.0f;
        lv_color_t c = grad->stops[i].color;

        color_ramp[i].red = c.red / 255.0f;
        color_ramp[i].green = c.green / 255.0f;
        color_ramp[i].blue = c.blue / 255.0f;
        color_ramp[i].alpha = grad->stops[i].opa / 255.0f;
    }
}

static bool linear_grad_create(grad_item_t * item)
{
    LV_PROFILER_DRAW_BEGIN;

    LV_VG_LITE_CHECK_ERROR(
        vg_lite_init_grad(&item->vg.linear),
        /* Error handler */
    {
        lv_vg_lite_linear_gradient_dump_info(&item->vg.linear);
        LV_PROFILER_DRAW_END;
        return false;
    });

    vg_lite_uint32_t colors[VLC_MAX_GRADIENT_STOPS];
    vg_lite_uint32_t stops[VLC_MAX_GRADIENT_STOPS];

    /* Gradient setup */
    if(item->lv.stops_count > VLC_MAX_GRADIENT_STOPS) {
        LV_LOG_WARN("Gradient stops limited: %d, max: %d", item->lv.stops_count, VLC_MAX_GRADIENT_STOPS);
        item->lv.stops_count = VLC_MAX_GRADIENT_STOPS;
    }

    for(uint16_t i = 0; i < item->lv.stops_count; i++) {
        stops[i] = item->lv.stops[i].frac;
        const lv_color_t * c = &item->lv.stops[i].color;
        lv_opa_t opa = item->lv.stops[i].opa;

        /* lvgl color -> gradient color */
        lv_color_t grad_color = lv_color_make(c->blue, c->green, c->red);
        colors[i] = lv_vg_lite_color(grad_color, opa, true);
    }

    LV_VG_LITE_CHECK_ERROR(
        vg_lite_set_grad(&item->vg.linear, item->lv.stops_count, colors, stops),
        /* Error handler */
    {
        lv_vg_lite_linear_gradient_dump_info(&item->vg.linear);
    });

    LV_PROFILER_DRAW_BEGIN_TAG("vg_lite_update_grad");
    LV_VG_LITE_CHECK_ERROR(
        vg_lite_update_grad(&item->vg.linear),
        /* Error handler */
    {
        lv_vg_lite_linear_gradient_dump_info(&item->vg.linear);
    });
    LV_PROFILER_DRAW_END_TAG("vg_lite_update_grad");

    LV_PROFILER_DRAW_END;
    return true;
}

static bool linear_ext_grad_create(grad_item_t * item, vg_lite_color_ramp_t * color_ramp)
{
    LV_PROFILER_DRAW_BEGIN;

    if(item->lv.stops_count > VLC_MAX_COLOR_RAMP_STOPS) {
        LV_LOG_WARN("Gradient stops limited: %d, max: %d", item->lv.stops_count, VLC_MAX_GRADIENT_STOPS);
        item->lv.stops_count = VLC_MAX_COLOR_RAMP_STOPS;
    }

    const vg_lite_linear_gradient_parameter_t grad_param = {
        .X0 = item->lv.x1,
        .Y0 = item->lv.y1,
        .X1 = item->lv.x2,
        .Y1 = item->lv.y2,
    };

    convert_color_ramp(color_ramp, &item->lv);

    LV_PROFILER_DRAW_BEGIN_TAG("vg_lite_set_linear_grad");
    LV_VG_LITE_CHECK_ERROR(
        vg_lite_set_linear_grad(
            &item->vg.linear_ext,
            item->lv.stops_count,
            color_ramp,
            grad_param,
            lv_spread_to_vg(item->lv.spread),
            1),
        /* Error handler */
    {
        lv_vg_lite_ext_linear_gradient_dump_info(&item->vg.linear_ext);
    });
    LV_PROFILER_DRAW_END_TAG("vg_lite_set_linear_grad");

    LV_PROFILER_DRAW_BEGIN_TAG("vg_lite_update_linear_grad");
    vg_lite_error_t err = vg_lite_update_linear_grad(&item->vg.linear_ext);
    LV_PROFILER_DRAW_END_TAG("vg_lite_update_linear_grad");
    if(err != VG_LITE_SUCCESS)  {
        LV_LOG_ERROR("vg_lite_update_linear_grad error: %d", (int)err);
        lv_vg_lite_error_dump_info(err);
        lv_vg_lite_ext_linear_gradient_dump_info(&item->vg.linear_ext);
    }

    LV_PROFILER_DRAW_END;
    return err == VG_LITE_SUCCESS;
}

static bool radial_grad_create(grad_item_t * item, vg_lite_color_ramp_t * color_ramp)
{
    LV_PROFILER_DRAW_BEGIN;

    if(item->lv.stops_count > VLC_MAX_COLOR_RAMP_STOPS) {
        LV_LOG_WARN("Gradient stops limited: %d, max: %d", item->lv.stops_count, VLC_MAX_GRADIENT_STOPS);
        item->lv.stops_count = VLC_MAX_COLOR_RAMP_STOPS;
    }

    convert_color_ramp(color_ramp, &item->lv);

    const vg_lite_radial_gradient_parameter_t grad_param = {
        .cx = item->lv.cx,
        .cy = item->lv.cy,
        .r = item->lv.cr,
        .fx = item->lv.cx,
        .fy = item->lv.cy,
    };

    LV_PROFILER_DRAW_BEGIN_TAG("vg_lite_set_radial_grad");
    LV_VG_LITE_CHECK_ERROR(
        vg_lite_set_radial_grad(
            &item->vg.radial,
            item->lv.stops_count,
            color_ramp,
            grad_param,
            lv_spread_to_vg(item->lv.spread),
            1),
        /* Error handler */
    {
        lv_vg_lite_radial_gradient_dump_info(&item->vg.radial);
    });
    LV_PROFILER_DRAW_END_TAG("vg_lite_set_radial_grad");

    LV_PROFILER_DRAW_BEGIN_TAG("vg_lite_update_radial_grad");
    vg_lite_error_t err = vg_lite_update_radial_grad(&item->vg.radial);
    LV_PROFILER_DRAW_END_TAG("vg_lite_update_radial_grad");
    if(err != VG_LITE_SUCCESS) {
        LV_LOG_ERROR("vg_lite_update_radial_grad error: %d", (int)err);
        lv_vg_lite_error_dump_info(err);
        lv_vg_lite_radial_gradient_dump_info(&item->vg.radial);
    }

    LV_PROFILER_DRAW_END;
    return err == VG_LITE_SUCCESS;
}

static grad_type_t lv_grad_style_to_type(lv_vector_gradient_style_t style)
{
    if(style == LV_VECTOR_GRADIENT_STYLE_LINEAR) {
        return vg_lite_query_feature(gcFEATURE_BIT_VG_LINEAR_GRADIENT_EXT) ? GRAD_TYPE_LINEAR_EXT : GRAD_TYPE_LINEAR;
    }

    if(style == LV_VECTOR_GRADIENT_STYLE_RADIAL) {
        return GRAD_TYPE_RADIAL;
    }

    LV_LOG_WARN("unknown gradient style: %d", style);
    return GRAD_TYPE_UNKNOWN;
}

static void grad_point_to_matrix(vg_lite_matrix_t * grad_matrix, float x1, float y1, float x2, float y2)
{
    vg_lite_translate(x1, y1, grad_matrix);

    float angle = atan2f(y2 - y1, x2 - x1) * 180.0f / (float)M_PI;
    vg_lite_rotate(angle, grad_matrix);
    float length = sqrtf(SQUARE(x2 - x1) + SQUARE(y2 - y1));
    vg_lite_scale(length / 256.0f, 1, grad_matrix);
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
            LV_LOG_WARN("unknown spread mode: %d", spread);
            break;
    }

    return VG_LITE_GRADIENT_SPREAD_FILL;
}

static bool grad_create_cb(grad_item_ref_t * item_ref, void * user_data)
{
    LV_PROFILER_DRAW_BEGIN;
    const grad_type_t type = lv_grad_style_to_type((*item_ref)->lv.style);
    if(type == GRAD_TYPE_UNKNOWN) {
        LV_PROFILER_DRAW_END;
        return false;
    }

    grad_item_t * item = grad_item_pool_alloc(user_data, type);
    if(item == NULL) {
        /* Should not happen */
        LV_PROFILER_DRAW_END;
        return false;
    }

    /* Copy key information */
    item->lv = (*item_ref)->lv;

    bool is_success = false;

    lv_vg_lite_grad_ctx_t * ctx = user_data;
    LV_ASSERT_NULL(ctx);

    switch(type) {
        case GRAD_TYPE_LINEAR:
            is_success = linear_grad_create(item);
            break;

        case GRAD_TYPE_LINEAR_EXT:
            is_success = linear_ext_grad_create(item, ctx->local_color_ramp);
            break;

        case GRAD_TYPE_RADIAL:
            is_success = radial_grad_create(item, ctx->local_color_ramp);
            break;

        default:
            LV_LOG_ERROR("unknown gradient type: %d", type);
            break;
    }

    if(is_success) {
        *item_ref = item;
    }
    else {
        grad_item_pool_free(item);
    }

    LV_PROFILER_DRAW_END;
    return is_success;
}

static void grad_free_cb(grad_item_ref_t * item_ref, void * user_data)
{
    LV_UNUSED(user_data);
    LV_PROFILER_DRAW_BEGIN;
    grad_item_t * item = *item_ref;
    switch(item->type) {
        case GRAD_TYPE_LINEAR:
            LV_PROFILER_DRAW_BEGIN_TAG("vg_lite_clear_grad");
            LV_VG_LITE_CHECK_ERROR(vg_lite_clear_grad(&item->vg.linear), {});
            LV_PROFILER_DRAW_END_TAG("vg_lite_clear_grad");
            break;

        case GRAD_TYPE_LINEAR_EXT:
            LV_PROFILER_DRAW_BEGIN_TAG("vg_lite_clear_linear_grad");
            LV_VG_LITE_CHECK_ERROR(vg_lite_clear_linear_grad(&item->vg.linear_ext), {});
            LV_PROFILER_DRAW_END_TAG("vg_lite_clear_linear_grad");
            break;

        case GRAD_TYPE_RADIAL:
            LV_PROFILER_DRAW_BEGIN_TAG("vg_lite_clear_radial_grad");
            LV_VG_LITE_CHECK_ERROR(vg_lite_clear_radial_grad(&item->vg.radial), {});
            LV_PROFILER_DRAW_END_TAG("vg_lite_clear_radial_grad");
            break;

        default:
            LV_LOG_ERROR("unknown gradient type: %d", item->type);
            break;
    }

    grad_item_pool_free(item);
    LV_PROFILER_DRAW_END;
}

static lv_cache_compare_res_t grad_compare_cb(const grad_item_ref_t * lhs_ref, const grad_item_ref_t * rhs_ref)
{
    const grad_item_t * lhs = *lhs_ref;
    const grad_item_t * rhs = *rhs_ref;

    /* compare type first */
    if(lhs->type != rhs->type) {
        return lhs->type > rhs->type ? 1 : -1;
    }

    /* compare spread mode */
    if(lhs->lv.spread != rhs->lv.spread) {
        return lhs->lv.spread > rhs->lv.spread ? 1 : -1;
    }

    /* compare gradient parameters */
    switch(lhs->type) {
        case GRAD_TYPE_LINEAR:
            /* no extra compare needed */
            break;

        case GRAD_TYPE_LINEAR_EXT:
            if(!math_equal(lhs->lv.x1, rhs->lv.x1)) {
                return lhs->lv.x1 > rhs->lv.x1 ? 1 : -1;
            }

            if(!math_equal(lhs->lv.y1, rhs->lv.y1)) {
                return lhs->lv.y1 > rhs->lv.y1 ? 1 : -1;
            }

            if(!math_equal(lhs->lv.x2, rhs->lv.x2)) {
                return lhs->lv.x2 > rhs->lv.x2 ? 1 : -1;
            }

            if(!math_equal(lhs->lv.y2, rhs->lv.y2)) {
                return lhs->lv.y2 > rhs->lv.y2 ? 1 : -1;
            }
            break;

        case GRAD_TYPE_RADIAL:
            if(!math_equal(lhs->lv.cx, rhs->lv.cx)) {
                return lhs->lv.cx > rhs->lv.cx ? 1 : -1;
            }

            if(!math_equal(lhs->lv.cy, rhs->lv.cy)) {
                return lhs->lv.cy > rhs->lv.cy ? 1 : -1;
            }

            if(!math_equal(lhs->lv.cr, rhs->lv.cr)) {
                return lhs->lv.cr > rhs->lv.cr ? 1 : -1;
            }
            break;

        default:
            LV_LOG_ERROR("unknown gradient type: %d", lhs->type);
            break;
    }

    /* compare stops count and stops */
    if(lhs->lv.stops_count != rhs->lv.stops_count) {
        return lhs->lv.stops_count > rhs->lv.stops_count ? 1 : -1;
    }

    int cmp_res = lv_memcmp(lhs->lv.stops, rhs->lv.stops,
                            sizeof(lv_grad_stop_t) * lhs->lv.stops_count);
    if(cmp_res != 0) {
        return cmp_res > 0 ? 1 : -1;
    }

    return 0;
}


static void lv_vg_lite_linear_gradient_dump_info(const vg_lite_linear_gradient_t * grad)
{
    LV_LOG_USER("count: %d", (int)grad->count);

    for(vg_lite_uint32_t i = 0; i < grad->count; i++) {
        LV_LOG_USER("[%d] color: 0x%08X, stop: 0x%08X", (int)i, grad->colors[i], grad->stops[i]);
    }

    lv_vg_lite_matrix_dump_info(&grad->matrix);
    lv_vg_lite_buffer_dump_info(&grad->image);
}

static void lv_vg_lite_color_ramp_dump_info(const vg_lite_color_ramp_t * ramp, const vg_lite_uint32_t length)
{
    for(vg_lite_uint32_t i = 0; i < length; i++) {
        LV_LOG_USER("[%d] stop: %f, red: %f, green: %f, blue: %f, alpha: %f",
                    (int)i, ramp[i].stop, ramp[i].red, ramp[i].green, ramp[i].blue, ramp[i].alpha);
    }
}

static void lv_vg_lite_ext_linear_gradient_dump_info(const vg_lite_ext_linear_gradient_t * grad)
{
    LV_LOG_USER("count: %d", (int)grad->count);
    lv_vg_lite_matrix_dump_info(&grad->matrix);
    lv_vg_lite_buffer_dump_info(&grad->image);
    LV_LOG_USER("linear_grad: X0: %f, Y0: %f, X1: %f, Y1: %f",
                grad->linear_grad.X0, grad->linear_grad.Y0, grad->linear_grad.X1, grad->linear_grad.Y1);

    LV_LOG_USER("ramp_length: %d", (int)grad->ramp_length);
    lv_vg_lite_color_ramp_dump_info(grad->color_ramp, grad->ramp_length);

    LV_LOG_USER("converted_length: %d", (int)grad->converted_length);
    lv_vg_lite_color_ramp_dump_info(grad->converted_ramp, grad->converted_length);

    LV_LOG_USER("pre_multiplied: %d", grad->pre_multiplied);
    LV_LOG_USER("spread_mode: %d", (int)grad->spread_mode);
}

static void lv_vg_lite_radial_gradient_dump_info(const vg_lite_radial_gradient_t * grad)
{
    LV_LOG_USER("count: %d", (int)grad->count);
    lv_vg_lite_matrix_dump_info(&grad->matrix);
    lv_vg_lite_buffer_dump_info(&grad->image);
    LV_LOG_USER("radial_grad: cx: %f, cy: %f, r: %f, fx: %f, fy: %f",
                grad->radial_grad.cx, grad->radial_grad.cy, grad->radial_grad.r, grad->radial_grad.fx, grad->radial_grad.fy);

    LV_LOG_USER("ramp_length: %d", (int)grad->ramp_length);
    lv_vg_lite_color_ramp_dump_info(grad->color_ramp, grad->ramp_length);

    LV_LOG_USER("converted_length: %d", (int)grad->converted_length);
    lv_vg_lite_color_ramp_dump_info(grad->converted_ramp, grad->converted_length);

    LV_LOG_USER("pre_multiplied: %d", grad->pre_multiplied);
    LV_LOG_USER("spread_mode: %d", (int)grad->spread_mode);
}

#endif /*LV_USE_DRAW_VG_LITE && LV_USE_VECTOR_GRAPHIC*/
